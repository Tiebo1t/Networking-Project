#include "Bullet.h"

Bullet::Bullet(std::shared_ptr<sf::Texture> tex, sf::Vector2f tankDir)
{
	body = std::make_unique<sf::Sprite>(*tex);
	direction = tankDir;
}

void Bullet::Update(float dt)
{
	position += direction * dt * bulletSpeed;
}

const void Bullet::Render(sf::RenderWindow& window)
{
	window.draw(*body);
}