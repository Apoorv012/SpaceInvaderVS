#include <SDL_ttf.h>
#include <SDL.h>
#include <string>
#include <iostream>

#include "Text.hpp"
#include "RenderWindow.hpp"

Text::Text(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, const std::string& message, const SDL_Color& color) {
	_textTexture = Text::loadFont(renderer, fontPath, fontSize, message, color);
	SDL_QueryTexture(_textTexture, nullptr, nullptr, &_textRect.w, &_textRect.h);
}


void Text::display(SDL_Renderer* renderer, int x, int y) const {
	_textRect.x = x;
	_textRect.y = y;

	SDL_RenderCopy(renderer, _textTexture, nullptr, &_textRect);

}

SDL_Texture* Text::loadFont(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, const std::string& message, const SDL_Color& color) {
	TTF_Font* font = TTF_OpenFont(fontPath.c_str(), fontSize);
	if (!font) {
		std::cerr << "ERROR: Failed to load font" << std::endl;
	}

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, message.c_str(), color);
	if (!textSurface) {
		std::cerr << "ERROR: Failed to create Text Surface" << std::endl;
	}

	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	if (!textTexture) {
		std::cerr << "ERROR: Failed to create Text Texture" << std::endl;
	}

	SDL_FreeSurface(textSurface);

	return textTexture;
}