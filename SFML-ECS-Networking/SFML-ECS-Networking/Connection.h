#pragma once
#include <SFML\Network.hpp>
#include <deque>
#include "Message.h"

struct Connection
{
	//std::unordered_map < unsigned short, sf::IpAddress > connectionSocket;
	int uniqueID;
	std::deque<Message> message_history;

	float rotation = 0;

	float inter_t = 0.f; // This value goes from 0.f to 1.f during interpolation and will be accumulated based on time delta.

	// Interpolate position based on selected method and value of t. Return TankMessage with updated position.
	sf::Vector2f Interpolate(float dt) {
		// Calculate interpolation step
		float step = 0.f;
		sf::Vector2f velocity = sf::Vector2f(0, 0);

		if (message_history.size() > 1)
		{
			// Make sure we have at least 2 messages for Linear interpolation.
			sf::Vector2f distance = message_history[0].position - message_history[1].position;

			velocity = distance;// (message_history[0].time - message_history[1].time);

			step = 1.f / ((message_history[0].time - message_history[1].time) / dt);
		}
		else
			step = 0.f;

		// Add step to inter_t
		inter_t += step;

		// Clamp inter_t to max of 1.f
		if (inter_t > 1.f)
			inter_t = 1.f;

		if (message_history.size() > 1) { // Make sure we have at least 2 messages for Linear interpolation.
			// Calculate new position by interpolating between last two known positons.
			//return ((1 - inter_t) * message_history[1].position + inter_t * message_history[0].position);
			return message_history[0].position + velocity;
		}
		else
			return message_history[0].position; // Just return the last known position.
	}
};
