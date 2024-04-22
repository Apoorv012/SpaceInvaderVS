#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <SDL_ttf.h>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include <SDL_mixer.h>

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
	: window(NULL), renderer(NULL)
{
	window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);

	if (window == NULL) {
		std::cout << "Window failed to init. Error: " << SDL_GetError() << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}


SDL_Texture* RenderWindow::loadTexture(const char* p_filePath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, p_filePath);

	if (texture == NULL) {
		std::cout << "Failed to load texture. Error: " << SDL_GetError() << std::endl;
	}

	return texture;
}

SDL_Renderer* RenderWindow::getRenderer()
{
	return renderer;
}

void RenderWindow::clear() {
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}


void RenderWindow::render(Entity& p_entity) {
	SDL_Rect src;
	src.x = p_entity.getCurrentFrame().x;
	src.y = p_entity.getCurrentFrame().y;
	src.w = p_entity.getCurrentFrame().w;
	src.h = p_entity.getCurrentFrame().h;

	SDL_Rect dst;
	dst.x = (int)p_entity.getPos().x;
	dst.y = (int)p_entity.getPos().y;
	dst.w = (int)p_entity.getCurrentFrame().w;
	dst.h = (int)p_entity.getCurrentFrame().h;

	SDL_RenderCopy(renderer, p_entity.getTexture(), &src, &dst);
}

void RenderWindow::render(Entity* p_entity)
{
	if (p_entity) {
		SDL_Rect src;
		src.x = p_entity->getCurrentFrame().x;
		src.y = p_entity->getCurrentFrame().y;
		src.w = p_entity->getCurrentFrame().w;
		src.h = p_entity->getCurrentFrame().h;

		SDL_Rect dst;
		std::cout << "x: " << p_entity->getPos().x << ", y: " << p_entity->getPos().y << std::endl;
		dst.x = (int)p_entity->getPos().x;
		dst.y = (int)p_entity->getPos().y;
		dst.w = (int)p_entity->getCurrentFrame().w;
		dst.h = (int)p_entity->getCurrentFrame().h;

		SDL_RenderCopy(renderer, p_entity->getTexture(), &src, &dst);
	}
}

void RenderWindow::renderUI() {
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderDrawLine(renderer, 0, 550, 800, 550);
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
}

