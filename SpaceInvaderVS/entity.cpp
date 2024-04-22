#include "Entity.hpp"
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim)
	: pos(p_pos), tex(p_tex)
{
	_rect.x = pos.x;
	_rect.y = pos.y;
	_rect.w = (int)p_dim.x;
	_rect.h = (int)p_dim.y;

	currentFrame.x = 0;
	currentFrame.y = 0;
	currentFrame.w = (int)p_dim.x;
	currentFrame.h = (int)p_dim.y;
}

Vector2f& Entity::getPos()
{
	return pos;
}

SDL_Texture* Entity::getTexture()
{
	return tex;
}

SDL_Rect Entity::getCurrentFrame()
{
	return currentFrame;
}

Player::Player(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim)
	: Entity(p_pos, p_tex, p_dim), bulletTexture(nullptr) {
	bullet = nullptr;
}

void Player::moveX(int x)
{
	pos.x += x;

	if (pos.x < 20) {
		pos.x -= x;
	}
	if (pos.x > 730) {
		pos.x -= x;
	}
	
}

void Player::shoot()
{
	if (bullet) return;

	bullet = new Bullet(Vector2f(pos.x + 24, pos.y), bulletTexture, Vector2f(40, 80), -7, this);
}

void Player::setBulletTexture(SDL_Texture* bulletTex)
{
	bulletTexture = bulletTex;
}

void Player::update()
{

	if (bullet) {
		bullet->update();
	}
}

void Player::displayBullet(SDL_Renderer* renderer, SDL_Texture* _texture)
{
	SDL_RenderCopy(renderer, _texture, nullptr, bullet->getRect());
}

Bullet* Player::getBullet()
{
	return bullet;
}

void Player::clearBullet()
{
	delete bullet;
	bullet = nullptr;
}

Bullet::Bullet(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, int speed, Player* player)
	: Entity(p_pos, p_tex, p_dim)
{
	this->speed = speed;
	rect = new SDL_Rect;
	rect->x = (int)p_pos.x;
	rect->y = (int)p_pos.y;
	rect->w = 4;
	rect->h = 8;
	_player = player;
}

SDL_Rect* Bullet::getRect()
{
	return rect;
}

void Bullet::update()
{
	pos.y += speed;
	rect->y = (int)pos.y;

	if (pos.y < 0) {
		_player->clearBullet();
	}
}
