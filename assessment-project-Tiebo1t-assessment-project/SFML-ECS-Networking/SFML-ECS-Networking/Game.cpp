#include "Game.h"
#include <iostream>

Game::Game()
{
	LoadTextures();
	// Initialis sprite for background.
	background = std::make_unique<sf::Sprite>(*textures["ground_sand"]);
	background->setTextureRect(sf::IntRect({ 0, 0 }, { 640, 480 }));

	// We're not loading the tank by default any more.
}

void Game::LoadTextures() {
	textures["ground_sand"] = std::make_shared<sf::Texture>("Assets/tileSand1.png");
	textures["ground_sand"]->setRepeated(true);

	textures["barrel_black"] = std::make_shared<sf::Texture>("Assets/blackBarrel.png");
	textures["body_black"] = std::make_shared<sf::Texture>("Assets/blackTank.png");
	textures["barrel_blue"] = std::make_shared<sf::Texture>("Assets/blueBarrel.png");
	textures["body_blue"] = std::make_shared<sf::Texture>("Assets/blueTank.png");
	textures["barrel_green"] = std::make_shared<sf::Texture>("Assets/greenBarrel.png");
	textures["body_green"] = std::make_shared<sf::Texture>("Assets/greenTank.png");
}

void Game::HandleEvents(const std::optional<sf::Event> event)
{
	// Handle key press events passed from window.
	if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
		if (keyPressed->scancode == sf::Keyboard::Scancode::W) {
			tanks.at(0)->isMoving.forward = true;
			tanks.at(0)->isMoving.backward = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::S) {
			tanks.at(0)->isMoving.forward = false;
			tanks.at(0)->isMoving.backward = true;
		}
		if (keyPressed->scancode == sf::Keyboard::Scancode::A) {
			tanks.at(0)->isMoving.left = true;
			tanks.at(0)->isMoving.right = false;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::D) {
			tanks.at(0)->isMoving.left = false;
			tanks.at(0)->isMoving.right = true;
		}
		else if (keyPressed->scancode == sf::Keyboard::Scancode::O)
		{
			tanks.at(0)->isShooting = true;
		}

	}

	// Handle key release events passed from window.
	else if (const auto* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
		if (keyReleased->scancode == sf::Keyboard::Scancode::W)
			tanks.at(0)->isMoving.forward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::S)
			tanks.at(0)->isMoving.backward = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::A)
			tanks.at(0)->isMoving.left = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::D)
			tanks.at(0)->isMoving.right = false;
		if (keyReleased->scancode == sf::Keyboard::Scancode::O)
			tanks.at(0)->isShooting = false;
	}

	//if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	//{
	//	tanks.at(0)->isShooting = true;
	//	std::cout << "pressed" << std::endl;
	//}
	//else 
	//{
	//	tanks.at(0)->isShooting = false;
	//}
}

void Game::Update(float dt)
{

	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(i)->Update(dt); 
	}

}

// Returns tanks position and rotation to be used for interpolation
void Game::Interpolate(sf::Vector2f pos, int id, float rot)
{
	for (int i = 0; i < tanks.size(); i++)
	{
		if (tanks.at(i)->uniqueID == id)
		{
			tanks.at(i)->position = pos;

			tanks.at(i)->bodyRotation = sf::degrees(rot);
		}
	}
}

void Game::NetworkUpdate(float dt, Message data) {
	// Force position updates from network data.

	for (int i = 0; i < tanks.size(); i++)
	{

		if (tanks.at(i)->uniqueID == data.IDpos)
		{
			tanks.at(i)->position = data.position;

			tanks.at(i)->bodyRotation = sf::degrees(data.rotation);

			numberOfTanks = data.numberOfTanks;

			std::cout << data.numberOfTanks << std::endl;
		}
	}

}

void Game::NetworkBulletUpdate(float dt, BulletMessage data)
{

	numberOfBullets = data.noOfBullets;

	for (int i = 0; i < bullets.size(); i++)
	{
		bullets.at(i)->position = data.position;
		bullets.at(i)->Update(dt);
	}
}

void Game::Render(sf::RenderWindow& window)
{
	window.draw(*background);
	for (int i = 0; i < tanks.size(); i++) {
		tanks.at(i)->Render(window);
	}

}

// Add new tank to tank array
void Game::AddTank(std::string body_tex, std::string barrel_tex, sf::Vector2f position, int ID)
{ 
	std::unique_ptr<Tank> tank = std::make_unique<Tank>(Tank(textures[body_tex], textures[barrel_tex]));
	tank->position = position;
	tank->uniqueID = ID;
	tanks.push_back(std::move(tank));
}

void Game::RemoveTank(int id)
{
	//for (int i = 0; i < tanks.size(); i++)
	//{
	//	if (tanks.at(i)->uniqueID == id)
	//	{
	//		tanks.erase(tanks.at(i));
	//	}
	//}
	
}

// Add new bullet to bullet array
void Game::AddBullet(std::string tex, sf::Vector2f position)
{
	std::unique_ptr<Bullet> bullet = std::make_unique<Bullet>(Bullet(textures[tex], position));
	bullet->position = position;
	bullets.push_back(std::move(bullet));
}

// Returns this clients tank position
sf::Vector2f Game::GetThisTankPosition()
{
	return tanks.at(0)->position;
}

// Returns this clients tank rotation
float Game::GetThisTankRotation()
{
	return tanks.at(0)->bodyRotation.asDegrees();
}

BulletMessage Game::GetNetworkBulletUpdate()
{

	if (numberOfBullets > 0)
	{

		return { -1, 0, bullets.at(0)->position, int(bullets.size()) };
	}
}
