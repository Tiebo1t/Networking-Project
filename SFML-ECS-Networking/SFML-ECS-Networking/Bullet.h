#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include <memory>
#include "Message.h"

class Bullet
{
public:
	Bullet(std::shared_ptr<sf::Texture> texture, sf::Vector2f tankDirection);

	void Update(float dt);
	const void Render(sf::RenderWindow& window);

	sf::Vector2f position;

private:

	std::unique_ptr<sf::Sprite> body;
	float bulletSpeed = 15;
	sf::Vector2f direction;
};

