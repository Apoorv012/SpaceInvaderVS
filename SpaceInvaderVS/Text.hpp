#pragma once
#include <SDL_ttf.h>
#include <SDL.h>
#include <string>

class Text {
public:
	Text(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, const std::string& message, const SDL_Color& color);
	void display(SDL_Renderer* renderer, int x, int y) const;
	static SDL_Texture* loadFont(SDL_Renderer* renderer, const std::string& fontPath, int fontSize, const std::string& message, const SDL_Color& color);

private:
	SDL_Texture* _textTexture = nullptr;
public:
	mutable SDL_Rect _textRect;

};