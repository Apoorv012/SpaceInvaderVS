#include "Entity.hpp"
#include <SDL.h>
#include <SDL_image.h>

#include "Math.hpp"

Entity::Entity(Vector2f p_pos, SDL_Texture* p_tex, Vector2f p_dim)
	: pos(p_pos), tex(p_tex)
{
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
	: Entity(p_pos, p_tex, p_dim) {}

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
