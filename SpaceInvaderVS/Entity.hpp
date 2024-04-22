#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>

#include "Math.hpp"
#include "Text.hpp"

class Entity
{
public:
	Entity(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);

	Vector2f& getPos();
	SDL_Texture* getTexture();
	SDL_Rect getCurrentFrame();
	SDL_Rect getRect();
protected:
	SDL_Rect _rect;
	Vector2f pos;
	SDL_Rect currentFrame;
	SDL_Texture* tex;
};

class Bullet : public Entity {
	int speed;
	class Player* _player;
public:
	Bullet(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, int speed, Player* player);
	SDL_Rect* getRectPtr();
	void update(std::vector<Bullet*>* enemyBullets);
	void display(SDL_Renderer* renderer, SDL_Texture* _texture);
};

class Player : public Entity
{
	SDL_Texture* bulletTexture;
	Bullet* bullet;
public:
	Player(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);
	void moveX(int x);
	void shoot(Mix_Chunk* shootingSound);
	void setBulletTexture(SDL_Texture* bulletTex);
	void update();
	void displayBullet(SDL_Renderer* renderer, SDL_Texture* _texture);
	Bullet* getBullet();
	void clearBullet();
	void newLife();
};

class Bunker : public Entity {
	int health;
	Text* healthText;
	SDL_Renderer* _renderer;
public:
	Bunker(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim, SDL_Renderer* renderer);
	void loseHealth();
	int getHealth();
	void showHealth();
};

class Enemy : public Entity {
public:
	Enemy(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);
	void shoot(std::vector<Bullet*>& bullets, SDL_Texture* bulletTexture);
};