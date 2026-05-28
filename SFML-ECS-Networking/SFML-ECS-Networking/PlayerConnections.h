#pragma once
#include <SFML\Network.hpp>
#include <deque>
#include "Message.h"
#include "Tank.h"
#include "Game.h"

struct PlayerConnections
{
	unsigned short portNumber;
	sf::IpAddress IPAddress;

	int uniqueID;

	sf::Vector2f position = { 0, 0 };

	float rotation = 0;

	int iterationCounter = 0;

	std::deque<Message> message_history;

	float timer = 0;

	float inter_t = 0; // This value goes from 0.f to 1.f during interpolation and will be accumulated based on time delta.

	//Game game;

	//std::unique_ptr<Tank> tank;

	//void CreateTank()
	//{
	//	game.AddTank("body_blue", "barrel_black", { 240, 240 }, uniqueID);
	//}

	sf::Vector2f Interpolate(float dt) {
		
		float step = 0.f;
		//sf::Vector2f velocity = sf::Vector2f(0, 0);
		
		if (message_history.size() > 1)
		{
			// Make sure we have at least 2 messages for Linear interpolation.
			step = 1.f / ((message_history[0].time - message_history[1].time) / dt);
		}
		else
			step = 0.f;

		// Add step to inter_t
		inter_t += step;

		// Clamp inter_t to max of 1.f
		if(inter_t > 1.f)
			inter_t = 1.f;

		// Linear interpolation based off https://www.geeksforgeeks.org/maths/linear-interpolation-formula/

		//float x1 = message_history[0].time;
		//float x2 = message_history[1].time;

		//sf::Vector2f y1 = message_history[0].position;
		//sf::Vector2f y2 = message_history[1].position;

		//// Time interpolation based off lab 6
		//float x = ((1 - inter_t) * x1 + inter_t * x2);

		if (message_history.size() > 1)
		{
			return (((1 - inter_t) * message_history[1].position) + (inter_t * message_history[0].position));
		}
		else
		{
			return message_history[0].position;
		}
		

		//if (message_history.size() > 1) { // Make sure we have at least 2 messages for Linear interpolation.
		//	// Calculate new position by interpolating between last two known positons.
		//	return y1 + ((x - x1) * (y2 - y1)) / (x2 - x1);
		//return ((1 - inter_t) * message_history[0].position + inter_t * message_history[1].position);
		//}
		//else
		//	return message_history[0].position; // Just return the last known position.
	}

};
