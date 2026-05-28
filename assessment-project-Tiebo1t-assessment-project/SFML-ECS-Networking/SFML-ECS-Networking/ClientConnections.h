#pragma once
#include <SFML\Network.hpp>
#include <deque>
#include "Message.h"

struct ClientConnections
{
	int uniqueID = 0;
	sf::Vector2f position;
	float rotation = 0;
	std::deque<Message> message_history;

	float inter_t = 0;

	sf::Vector2f Interpolate(float dt) {

		float step = 0.f;

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
		if (inter_t > 1.f)
			inter_t = 1.f;

		// Returns new position calculated from linear interpolation every frame
		if (message_history.size() > 1)
		{
			return (((1 - inter_t) * message_history[1].position) + (inter_t * message_history[0].position));
		}
		else
		{
			return message_history[0].position;
		}

	}
};
