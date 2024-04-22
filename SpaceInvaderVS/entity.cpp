#include "Entity.hpp"
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"
#include <SDL_mixer.h>

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

SDL_Rect Entity::getRect()
{
	return _rect;
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

	_rect.x = pos.x;
}

void Player::shoot(Mix_Chunk* shootingSound)
{
	if (bullet) return;

	Mix_PlayChannel(1, shootingSound, 0);
	bullet = new Bullet(Vector2f(pos.x + 24, pos.y), bulletTexture, Vector2f(4, 8), -7, this);
}

void Player::setBulletTexture(SDL_Texture* bulletTex)
{
	bulletTexture = bulletTex;
}

void Player::update()
{
	if (bullet) {
		bullet->update(nullptr);
	}
}

void Player::displayBullet(SDL_Renderer* renderer, SDL_Texture* _texture)
{
	SDL_RenderCopy(renderer, _texture, nullptr, bullet->getRectPtr());
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

void Player::newLife()
{
	pos.x = 20;
	pos.y = 500;
}

Bullet::Bullet(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, int speed, Player* player)
	: Entity(p_pos, p_tex, p_dim)
{
	this->speed = speed;
	_player = player;
}

SDL_Rect* Bullet::getRectPtr()
{
	return &_rect;
}

void Bullet::update(std::vector<Bullet*>* enemyBullets)
{
	//std::cout << "Bullet at x: " << pos.x << ", y: " << pos.y << std::endl;

	pos.y += speed;
	_rect.y = (int)pos.y;

	if (pos.y < 0) {
		_player->clearBullet();
	}

	if (enemyBullets) {
		if (pos.y > 580) {
			for (size_t i = 0; i < enemyBullets->size(); i++) {
				if ((*enemyBullets)[i] == this) {
					enemyBullets->erase(enemyBullets->begin() + i);
					break;
				}
			}
			delete this;
		}
	}
}

void Bullet::display(SDL_Renderer* renderer, SDL_Texture* _texture)
{
	SDL_RenderCopy(renderer, _texture, nullptr, getRectPtr());
}

Bunker::Bunker(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, SDL_Renderer* renderer)
	: Entity(p_pos, p_tex, p_dim), health(5), _renderer(renderer) {
	healthText = new Text(_renderer, "res/fonts/space_invaders.ttf", 20, std::to_string(health), { 0, 0, 0, 255 });
}

void Bunker::loseHealth()
{
	health--;
	delete healthText;
	healthText = new Text(_renderer, "res/fonts/space_invaders.ttf", 20, std::to_string(health), { 0, 0, 0, 255 });
}

int Bunker::getHealth()
{
	return health;
}

void Bunker::showHealth()
{
	healthText->display(_renderer, pos.x + 36, pos.y + 10);
}

Enemy::Enemy(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim)
	: Entity(p_pos, p_tex, p_dim) {}

void Enemy::shoot(std::vector<Bullet*>& bullets, SDL_Texture* bulletTexture) {
	Bullet* bullet = new Bullet(Vector2f(pos.x + (_rect.w / 2), pos.y + _rect.h), bulletTexture, Vector2f(4, 8), 7, nullptr);
	bullets.push_back(bullet);
}
