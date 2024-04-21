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
	Vector2f pos;
	SDL_Rect currentFrame;
	SDL_Texture* tex;
};

class Player : public Entity
{
public:
	Player(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim);
	void moveX(int x);
};