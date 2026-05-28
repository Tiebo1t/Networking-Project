#pragma once

struct Message
{
	int packetHeader = 2;
	float time = 0;
	int iterationCounter = 0; 
	int IDpos = 0;
	sf::Vector2f position = {0, 0};
	float rotation = 0;
	int numberOfTanks = 0;
};