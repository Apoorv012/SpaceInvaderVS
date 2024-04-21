#pragma once
#include <SDL_ttf.h>
#include <SDL.h>
#include <string>
#include <vector>

#include "Text.hpp"

class Scoreboard {
private:
	std::vector<SDL_Texture*> _score;

	void clearBoard();

public:
	Scoreboard();
	~Scoreboard();

	void setScore(int score);
	void render();
};