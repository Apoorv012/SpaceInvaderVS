#include "Button.hpp"

Button::Button(SDL_Rect m_buttonRect, Vector2i m_textPos, std::string m_text, std::string m_fontPath, int m_fontSize, SDL_Color m_activeColor, SDL_Color m_inactiveColor)
	: _buttonRect(m_buttonRect), _textPos(m_textPos), _text(m_text), _fontPath(m_fontPath), _fontSize(m_fontSize), _activeColor(m_activeColor), _inactiveColor(m_inactiveColor)
	{}

void Button::display(SDL_Renderer * renderer)
{
	if (_textObj == nullptr) {
		_textObj = new Text(renderer, _fontPath, _fontSize, _text, _inactiveColor);
	}

	_textObj->display(renderer, _textPos.x, _textPos.y);
}

void Button::update(SDL_Renderer* renderer, int mouseX, int mouseY)
{
	if (mouseX > _buttonRect.x && mouseX < _buttonRect.x + _buttonRect.w && mouseY > _buttonRect.y && mouseY < _buttonRect.y + _buttonRect.h) {
		if (!isActive) {
			isActive = true;
			generateTextObj(renderer);
		}
	}
	else {
		if (isActive) {
			isActive = false;
			generateTextObj(renderer);
		}
	}
}

bool Button::getIsActive()
{
	return isActive;
}

void Button::generateTextObj(SDL_Renderer* renderer)
{
	_textObj = new Text(renderer, _fontPath, _fontSize, _text, isActive ? _activeColor : _inactiveColor);
}

