#pragma once
#include <SDL.h>
#include <string>
#include "Text.hpp"
#include "Math.hpp"

class Button
{
public:
	Button(SDL_Rect m_buttonRect, Vector2i m_textPos, std::string m_text, std::string _fontPath, int _fontSize, SDL_Color m_activeColor, SDL_Color m_inactiveColor);
	void display(SDL_Renderer* renderer);
	void update(SDL_Renderer* renderer, int mouseX, int mouseY);
	bool getIsActive();

private:
	SDL_Rect _buttonRect;
	Text* _textObj;
	Vector2i _textPos;
	std::string _text;
	std::string _fontPath;
	int _fontSize;
	SDL_Color _activeColor;
	SDL_Color _inactiveColor;
	bool isActive = false;

	void generateTextObj(SDL_Renderer* renderer);
};