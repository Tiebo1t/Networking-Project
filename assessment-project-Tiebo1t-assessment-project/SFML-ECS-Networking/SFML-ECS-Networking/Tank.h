#pragma once
#include <SFML\Graphics.hpp>
#include <vector>
#include "Message.h"

class Tank
{
public:
	// Tank constructor now expect a pointer to the previously loaded texture.
	Tank(std::shared_ptr<sf::Texture> bodyTexture, std::shared_ptr<sf::Texture> barrelTexture);

	void Update(float dt);
	const void Render(sf::RenderWindow& window);

	int uniqueID = 0;

	sf::Vector2f position = { 0.f, 0.f };
	sf::Angle barrelRotation = sf::degrees(0);
	sf::Angle bodyRotation = sf::degrees(0);;

	struct {
		bool forward = false;
		bool backward = false;
		bool left = false;
		bool right = false;
	} isMoving;

	bool isShooting = false;

private:
	std::unique_ptr<sf::Sprite> body;
	std::unique_ptr<sf::Sprite> barrel;

	float movementSpeed = 150.f;
	float rotationSpeed = 200.f;
};
