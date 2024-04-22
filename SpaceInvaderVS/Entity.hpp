#pragma once
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"

class Entity
{
public:
	Entity(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);

	Vector2f& getPos();
	SDL_Texture* getTexture();
	SDL_Rect getCurrentFrame();
protected:
	SDL_Rect _rect;
	Vector2f pos;
	SDL_Rect currentFrame;
	SDL_Texture* tex;
};

class Bullet : public Entity {
	int speed;
	class Player* _player;
	SDL_Rect* rect;
public:
	Bullet(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, int speed, Player* player);
	SDL_Rect* getRect();
	void update();
};

class Player : public Entity
{
	SDL_Texture* bulletTexture;
	Bullet* bullet;
public:
	Player(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);
	void moveX(int x);
	void shoot();
	void setBulletTexture(SDL_Texture* bulletTex);
	void update();
	void displayBullet(SDL_Renderer* renderer, SDL_Texture* _texture);
	Bullet* getBullet();
	void clearBullet();
};

