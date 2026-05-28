#pragma once
#include "Tank.h"
#include <SFML\Graphics.hpp>
#include <vector>
#include "Message.h"
#include "Bullet.h"
#include "BulletMessage.h"
#include "CreateMessage.h"

class Game
{
public:
	Game();

	void LoadTextures();
	void HandleEvents(const std::optional<sf::Event> event);
	void Update(float dt);
	// Network update now gets a collection of tank messages for multiple tanks.
	void NetworkUpdate(float dt, Message data);
	void NetworkBulletUpdate(float dt, BulletMessage data);
	void Render(sf::RenderWindow& window);
	void AddTank(std::string body_tex, std::string barrel_tex, sf::Vector2f position, int uniqueID);
	void AddBullet(std::string tex, sf::Vector2f position);
	void RemoveTank(int id);
	BulletMessage GetNetworkBulletUpdate();
	sf::Vector2f GetThisTankPosition();
	float GetThisTankRotation();
	void Interpolate(sf::Vector2f pos, int id, float rot);

private:
	// We can now have more thank 1 tank in the game.
	std::vector<std::unique_ptr<Tank>> tanks;
	std::vector<std::unique_ptr<Bullet>> bullets;
	std::unique_ptr<sf::Sprite> background;
	// We'll hold all textures here so we only need to load them once at game creation.
	std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;
	int numberOfTanks = 0;
	int numberOfBullets = 0;
	int initialID;
};

