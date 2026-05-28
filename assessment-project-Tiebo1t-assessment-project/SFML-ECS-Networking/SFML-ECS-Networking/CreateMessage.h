#pragma once

struct CreateMessage
{
	int packetHeader = 1;
	int id = 0;
	sf::Vector2<float> initialPos = { 100, 100 };
};
