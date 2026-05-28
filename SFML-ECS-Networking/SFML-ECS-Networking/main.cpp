/*	CMP425/501 Assessment Project Template.
* 
*	This is an template project that renders an empty SFML window.
*	Use this as a starting point of your final assessment.
*	There are two build configurations -- client and server. 
*	You can include/exclude source files for each configuration, 
*	if you need to build different versions of your game.
*/

#include <SFML\Graphics.hpp>
#include <SFML\Network.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>
#include "Message.h"
#include "Game.h"
#include "utils.h"
//#include "Connection.h"
#include "BulletMessage.h"
#include "PlayerConnections.h"
#include "CreateMessage.h"
#include "ClientConnections.h"
#include "DeleteMessage.h"

int main() {
	Utils::printMsg("Game startup...");

	// Prepare window.
	sf::RenderWindow window(sf::VideoMode({ 640, 480 }), "CMP425/CMP501 - Assessment Template");
	window.setFramerateLimit(60);	//Request 60 frames per second
	Utils::printMsg("Window ready...");

	// Port variables
	unsigned short defaultPort = 53000;
	unsigned short playerPort = sf::Socket::AnyPort;

	// Clients stored here
	std::vector<ClientConnections> clientConnections;
	std::vector<PlayerConnections> playerConnections;
	//std::vector<int> existingValues;

	bool isServer = false;

	char input;

	bool gameStart = false;

	const int MAX_HISTORY = 3;

	// Timer variables
	float gameSpeed = 1.0f;
	float timeBetweenPackets = 0;
	float packetTimeout = 0.1f;
	float timeForConnection = 0;
	float timeForDisconnect = 1;

	float sendTimer = 0;

	bool isConnected = false;
	bool isTimeout = false;
	bool addToList = false;

	int initialRandomID = 0;

	float gameTime = 0;

	bool initialJoin = false;

	int tempNum = 0;

	bool startTimer = false;

	float time1 = 0;
	float time2 = 0;
	float latency = 0;
	bool pingCheck = false;
	bool timeSetup = false;

	float newPacketTimer = 0;

	// ADJUST PACKET SEND RATE HERE
	float packetSendRate = 0.1f;

	sf::Vector2f interpolatedPos = {0,0};

	Message prevMessage;

	bool outOfOrderPacket = false;
	float timeout = 5;

	// Clock for timing the 'dt' value.
	sf::Clock clock;

	Game game;

	// Custom clear colour for the window (light blue).
	sf::Color clear_colour(135, 206, 250);

	float initialdt = clock.restart().asSeconds() * gameSpeed;

	// Setup UDP socket and disable blocking to constantly recieve updates
	sf::UdpSocket socket;
	socket.setBlocking(false);

	sf::Packet packet;

	// Stores array of integers representing tank positions on the client
	std::vector<int> messageIDS;
	std::vector<float> clientTimers;

	// Get input from user for client or server
	std::cout << "Enter 's' for server or 'c' for client" << std::endl;
	std::cin >> input;
	while (input != 'c' && input != 's')
	{
		std::cout << "Enter 's' for server or 'c' for client" << std::endl;
		std::cin >> input;
	}
	// TODO: Fix error where multiple clients are initialised
	if (input == 's' && isServer != true) // Issue with this statement?
	{
		std::cout << "Server";
		isServer = true;

		// Timer for connecting server, if the port is already (meaning an active server) it will timeout
		while (timeForConnection < packetTimeout)
		{
			timeForConnection += initialdt;
			if (socket.bind(defaultPort) != sf::Socket::Status::Done)
			{
				socket.bind(defaultPort);
				
			}
			else
			{
				Utils::printMsg("SERVER CONNECTED", success);
				break;
			}

		}

		if (socket.bind(defaultPort) != sf::Socket::Status::Done)
		{
			Utils::printMsg("SERVER TIME OUT", error);
			isTimeout = true;
		}

	}
	else if (input == 'c')
	{
		// Binds the client socket to a random port
		socket.bind(playerPort);

		while (timeForConnection < packetTimeout)
		{
			timeForConnection += initialdt;
			if (socket.bind(playerPort) != sf::Socket::Status::Done)
			{

				socket.bind(playerPort);

			}
			else
			{
				Utils::printMsg("CLIENT CONNECTED", success);
				break;
			}

		}

		if (socket.bind(playerPort) != sf::Socket::Status::Done)
		{
			Utils::printMsg("CLIENT TIME OUT", error);
		}
	}

	while (window.isOpen()) {
		// Calculate dt.
		float dt = clock.restart().asSeconds() * gameSpeed;

		if (startTimer)
			sendTimer += dt;
		else
			sendTimer = 0;

		// Increment timers by delta time
		gameTime += dt;
		timeBetweenPackets += dt;
		newPacketTimer += dt;

		// Handle window events (e.g. key press).
		while (const std::optional event = window.pollEvent())	{
			// This event is triggered when the "x" button is pressed to close the window.
			if (event->is<sf::Event::Closed>()) {
				Utils::printMsg("Window closed...", MessageType::warning);
				window.close();
			}
			// This event checks key press and closes the window on "ESC".
			if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
				if (keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
					Utils::printMsg("ESC pressed, closing window...", MessageType::warning);
					window.close();
				}
			}
			if (!isServer) game.HandleEvents(event);
		}

		// SERVER
		if (isServer && !isTimeout)
		{

			// Incoming ports and IP from clients
			std::optional<sf::IpAddress> incoming_ip;
			unsigned short incoming_port;
			sf::IpAddress serverIP(127, 0, 0, 1);

			// Message used for packets
			Message message;
			CreateMessage createMessage;
			DeleteMessage deleteMessage;

			game.Update(dt);

			if (socket.receive(packet, incoming_ip, incoming_port) == sf::Socket::Status::Done)
			{

				// Peek at message header
				int header = 0;

				packet >> header;

				std::cout << "Packet type " << header << std::endl;

				// Packet header 1 or 0 meaning new connection, if 0 is recieved it will be from a new client sending empty messages before the server connects them
				if (header == 1 or header == 0)
				{
					packet >> createMessage.id >> createMessage.initialPos.x >> createMessage.initialPos.y; 

					// Runs first positional update once 2 players or more are connected, to get the client sending and recieving
					if (playerConnections.size() > 1 && tempNum == 0)
					{
						tempNum = 1000;
						initialJoin = true;
					}
					if (tempNum == 1000)
					{
						Utils::printMsg("Game time started", success);
						gameTime = 0;

						tempNum = 10;
					}

					// If there is no players, add one to the array as well as creating a new tank object with unique ID and let the client know
					if (playerConnections.size() == 0)
					{

						initialRandomID = (rand() % 5000) + 1;

						std::cout << "rand value " << createMessage.id << std::endl;

						game.AddTank("body_blue", "barrel_black", { createMessage.initialPos }, initialRandomID);

						// initialise new player connection in array with port, IP and random ID
						playerConnections.push_back({ incoming_port, incoming_ip.value(), initialRandomID, createMessage.initialPos });

						createMessage.id = initialRandomID;

						Utils::printMsg("port and IP " + std::to_string(incoming_port) + " " + incoming_ip.value().toString());

						packet.clear();

						packet << createMessage.packetHeader << createMessage.id << createMessage.initialPos.x << createMessage.initialPos.y; 

						socket.send(packet, playerConnections[0].IPAddress, playerConnections[0].portNumber);

					}

					// Checks whether the recieved packet is from a new port or IP (meaning new client)
					for (int j = 0; j < playerConnections.size(); j++)
					{

						if (playerConnections[j].IPAddress == incoming_ip && playerConnections[j].portNumber == incoming_port)
						{
							addToList = false;
							break;
						}
						else
						{
							addToList = true;
						}

					}

					if (addToList)
					{

						int randomID = (rand() % 5000) + 1;

						for (int j = 0; j < playerConnections.size(); j++)
						{
							if (randomID == playerConnections[j].uniqueID)
							{
								do
								{
									int randomID = (rand() % 5000) + 1;
								} while (playerConnections[j].uniqueID != randomID);
							}

						}

						game.AddTank("body_green", "barrel_black", { createMessage.initialPos }, randomID);

						playerConnections.push_back({ incoming_port, incoming_ip.value(), randomID, createMessage.initialPos });// -- setup player connections like so

						Utils::printMsg("port and IP " + std::to_string(incoming_port) + " " + incoming_ip.value().toString());

						createMessage.id = randomID;

						addToList = false;

						packet.clear();

						packet << createMessage.packetHeader << createMessage.id << createMessage.initialPos.x << createMessage.initialPos.y;

						// Let all other clients know about the new tank
						for (int i = 0; i < playerConnections.size(); i++)
						{
							
							std::cout << "Packet size " << packet.getDataSize() << std::endl;

							socket.send(packet, playerConnections[i].IPAddress, playerConnections[i].portNumber);
						}

						// If there is already a client, the new client it will need to know about previously connected tanks 
						if (playerConnections.size() > 1)
						{
							// Lets the new client know about all other spawned tanks
							for (int i = 0; i < playerConnections.size(); i++)
							{
								if (playerConnections[i].uniqueID != randomID)
								{
									// Sends all other tank IDs and positions to the new client
									createMessage.id = playerConnections[i].uniqueID;
									createMessage.initialPos = playerConnections[i].position;

									std::cout << "NEW player id " << createMessage.id << " Position " << createMessage.initialPos.x << " " << createMessage.initialPos.y << std::endl;

									packet.clear();

									packet << createMessage.packetHeader << createMessage.id << createMessage.initialPos.x << createMessage.initialPos.y;

									socket.send(packet, incoming_ip.value(), incoming_port);
								}

							}
						}

					}

				}
				// Header 2 is for positional updates
				if (header == 2 or initialJoin)
				{

					if (initialJoin)
					{
						initialJoin = false;
					}

					Utils::printMsg("Recieved message from: " + incoming_ip.value().toString() + " : " + std::to_string(incoming_port));

					packet >> message.time >> message.iterationCounter >> message.IDpos >> message.position.x >> message.position.y >> message.rotation >> message.numberOfTanks;

					// Loops over all players and checks which IP and port the message is recieved from and sets the corresponding client to its unique ID
					for (int i = 0; i < playerConnections.size(); i++)
					{
						if (incoming_ip == playerConnections[i].IPAddress && incoming_port == playerConnections[i].portNumber)
						{
							message.IDpos = playerConnections[i].uniqueID;
							playerConnections[i].timer = 0;
						}
					}

					if (message.IDpos != 0)
					{

						// Loops all players and sets the message history and positions on their server side storage
						// THIS DOESNT DO ANYTHING ANYMORE AS INTERPOLATION IS HANDLED CLIENT SIDE - except for the network update handling positions
						// This is where out of order packets wouldve been handled
						for (int i = 0; i < playerConnections.size(); i++)
						{
							if (playerConnections[i].uniqueID == message.IDpos)
							{
								playerConnections[i].position = message.position;

								playerConnections[i].rotation = message.rotation;

								playerConnections[i].iterationCounter = message.iterationCounter;

								if (playerConnections[i].message_history.size() == 0)
								{
									playerConnections[i].message_history.push_front(message); // Adds newest message to front
								}

								if (message.time < playerConnections[i].message_history.front().time)
								{
									outOfOrderPacket = true;
								}
								else
								{
									playerConnections[i].message_history.push_front(message);
								}

								if (playerConnections[i].message_history.size() > MAX_HISTORY)
								{
									playerConnections[i].message_history.pop_back(); // Removes oldest message off back
								}

								message.numberOfTanks = playerConnections.size();

								game.NetworkUpdate(dt, message); 

							}
						}

						packet.clear();

						packet << message.packetHeader << message.time << message.iterationCounter << message.IDpos << message.position.x << message.position.y <<
							message.rotation << message.numberOfTanks;

						// Sends a message to every client except the one that has sent
						for (int i = 0; i < playerConnections.size(); i++)
						{

							if (playerConnections[i].uniqueID != message.IDpos) 
							{

								Utils::printMsg("sent", success);

								if (socket.send(packet, playerConnections[i].IPAddress, playerConnections[i].portNumber) != sf::Socket::Status::Done)
								{

									Utils::printMsg("failed to send", error);
								}

							}
						}

					}
				}

				Utils::printMsg("END OF ITERATION", warning);

				// Loops over all clients and checks whether the server has recieved a packet from them within a certain amount of time,
				// if not removes them from the connections and no longer sends to the client.
				for (std::vector<PlayerConnections>::iterator it = playerConnections.begin(); it != playerConnections.end(); it++)
				{
					
					if (it->IPAddress == incoming_ip && it->portNumber == incoming_port)
					{
						it->timer = 0;
					}
					else
					{
						it->timer += dt;
					}

					if (it->timer > timeForDisconnect)
					{
						Utils::printMsg("Player Disconnected!", error);

						int newVal = it->uniqueID;

						deleteMessage.id = newVal;

						packet.clear();

						// deleteMessage contains packet header 3 which would be used to alert the client of another disconnected client
						packet << deleteMessage.header << deleteMessage.id;

						sf::IpAddress ip = it->IPAddress;
						int port = it->portNumber;

						playerConnections.erase(it);

						socket.send(packet, ip, port);

						break;
					}
				}

				newPacketTimer = 0;
			}

		}

		// CLIENT
		else if (!isTimeout)
		{
			sf::IpAddress serverIP(127, 0, 0, 1);
			
			std::optional<sf::IpAddress> incoming_ip;
			unsigned short incoming_port;

			bool addToList = false;

			float clientGameTime = gameTime;

			Message message;

			CreateMessage createMessage;

			DeleteMessage deleteMessage;

			sf::Packet initialPacket;

			// Creates a new tank for the new client upon connection
			if (!isConnected)
			{

				// Spams server with empty packets until server detects the new IP and Port and sets up the user with a connection
				socket.send(packet, serverIP, defaultPort);

				if (socket.receive(packet, incoming_ip, incoming_port) == sf::Socket::Status::Done)
				{

					int header = 0;
					packet >> header;

					// Sets up a new client with the ID sent over from the server
					// Client connections array behaves very similar to player connections array on the server
					if (header == 1)
					{

						packet >> createMessage.id >> createMessage.initialPos.x >> createMessage.initialPos.y;

						std::cout << "CLIENT CONNECTED ID " << createMessage.id << std::endl;

						game.AddTank("body_blue", "barrel_black", { createMessage.initialPos }, createMessage.id);

						clientConnections.push_back({ createMessage.id, createMessage.initialPos });

						isConnected = true;

						packet.clear();

						packet << createMessage.packetHeader << createMessage.id << createMessage.initialPos.x << createMessage.initialPos.y;

						// Wont send a new packet if the timing is less than the latest packets time in message history
						if (outOfOrderPacket)
						{
							Utils::printMsg("time is less, discaring message", error);
							outOfOrderPacket = false;
						}
						else
						{
							socket.send(packet, serverIP, defaultPort);
						}

					}

				}

			}
			else if (isConnected)
			{
				// Updates local game state
				game.Update(dt);

				// Interpolation done here outwith the recieve so it happens every frame
				// This interpolates the movement of every other clients tank between the last and current position this client has recieved from the server
				if (clientConnections.size() > 1)
				{
					for (int i = 0; i < clientConnections.size(); i++)
					{

						if (i != 0 && clientConnections[i].message_history.size() > 0)
						{

							interpolatedPos = clientConnections[i].Interpolate(dt);

							game.Interpolate(interpolatedPos, clientConnections[i].uniqueID, clientConnections[i].rotation);

						}

					}
				}

				if (socket.receive(packet, incoming_ip, incoming_port) == sf::Socket::Status::Done) // && newPacketTimer > packetSendRate
				{

					int header = 0;
					packet >> header;

					std::cout << "Packet type " << header << std::endl;

					if (header == 1)
					{
						packet >> createMessage.id >> createMessage.initialPos.x >> createMessage.initialPos.y;

						// Originally how I was going to sync time but it became uneeded
						if (!pingCheck)
						{
							time1 = gameTime;
							pingCheck = true;
						}
						else if (pingCheck)
						{
							time2 = gameTime;

							latency = (time2 - time1) / 2;
						}

						// Loops over all currently stored client connections and checks if a new one needs to be added 
						// (a new unique ID would be recieved from the server to signify this)
						addToList = true;
						for (int i = 0; i < clientConnections.size(); i++)
						{
							if (createMessage.id == clientConnections[i].uniqueID)
							{
								addToList = false;
							}
						}

						if (addToList)
						{

							clientConnections.push_back({ createMessage.id, createMessage.initialPos });

							std::cout << "OTHER CLIENT SPAWNED ID " << createMessage.id << std::endl;

							game.AddTank("body_green", "barrel_black", { createMessage.initialPos }, createMessage.id);

							addToList = false;
						}

						packet.clear();

						packet << createMessage.packetHeader << createMessage.id << createMessage.initialPos.x << createMessage.initialPos.y;

						socket.send(packet, serverIP, defaultPort);

					}

					if (header == 2) 
					{

						Utils::printMsg("RECIEVED", success);

						packet >> message.time >> message.iterationCounter >> message.IDpos >> message.position.x >> message.position.y >>
							message.rotation >> message.numberOfTanks;

						// Would be how I would setup synced game time
						if (message.numberOfTanks > 1 && !timeSetup)
						{
							Utils::printMsg("Game time started", success);
							timeSetup = true;
							gameTime = 0 + latency; 
							std::cout << "game time " << gameTime << std::endl;
						}

						message.time = gameTime;

						if (message.IDpos != 0)
						{

							// Loops all connected clients and checks their IDs
							for (int i = 0; i < clientConnections.size(); i++)
							{

								// Checks if the received ID from the server corresponds to the stored ID of one of the clients
								// Stores rotation, and latest messages in message history, these values will be used for interpolation
								if (message.IDpos == clientConnections[i].uniqueID)
								{
									clientConnections[i].rotation = message.rotation;

									if (clientConnections[i].message_history.size() == 0)
									{
										clientConnections[i].message_history.push_front(message); // Adds newest message to front
									}

									if (message.time < clientConnections[i].message_history.front().time)
									{
										Utils::printMsg("iteration counter less, discaring message", error);
									}
									else
									{
										clientConnections[i].message_history.push_front(message);
									}

									if (clientConnections[i].message_history.size() > MAX_HISTORY)
									{
										clientConnections[i].message_history.pop_back(); // Removes oldest message off back
									}

									clientConnections[i].inter_t = 0;
								}

							}

							// Iteration counter is obsolete but would have been used as a substitute for time to check for latest packets
							message.iterationCounter++;

							// Updates the messages position and rotation to be sent to the server with any movements or rotations the player would have made locally 
							message.position = game.GetThisTankPosition();

							message.rotation = game.GetThisTankRotation();

							// Stores all the information from the latest message recieved from the server, this is to be used after the delay for sending is over
							prevMessage =
							{
								message.packetHeader,
								message.time,
								message.iterationCounter,
								message.IDpos,
								message.position,
								message.rotation,
								message.numberOfTanks,
							};

						}
						else
						{
							Utils::printMsg("Message ID is 0", error);
						}


					}
					if (header == 3)
					{

						// Used to delete player from connections
						packet >> deleteMessage.id;

						for (std::vector<ClientConnections>::iterator it = clientConnections.begin(); it != clientConnections.end(); it++)
						{
							if (it->uniqueID == deleteMessage.id)
							{
								Utils::printMsg("Player Disconnected!", error);
								clientConnections.erase(it);
								break;
							}
							
						}
					}


				}

				// After a set send rate, send the latest packet to the server, ensures packets arent sent every frame
				if (newPacketTimer > packetSendRate && prevMessage.IDpos != 0)
				{

					packet.clear();

					packet << prevMessage.packetHeader << prevMessage.time << prevMessage.iterationCounter << prevMessage.IDpos << prevMessage.position.x << prevMessage.position.y <<
						prevMessage.rotation << prevMessage.numberOfTanks;

					socket.send(packet, serverIP, defaultPort);

					newPacketTimer = 0;
				}
				

			}

		}

		window.clear();

		game.Render(window);
		// Render you game objects here

		window.display();		
	}

	return 0;
}

