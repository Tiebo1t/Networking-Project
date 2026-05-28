#pragma once
#include <SFML\Graphics.hpp>

struct BulletMessage
{
	float time;
	int id;
	sf::Vector2f position = { 0, 0 };
	int noOfBullets;
};