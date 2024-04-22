#pragma once

#include <iostream>
#include <string>

std::string getScoreString(int score, int numOfDigits) {
	std::string scorePrefix("SCORE: ");
	std::string scoreText = std::to_string(score);
	std::string final = scorePrefix + std::string(numOfDigits - scoreText.length(), '0') + scoreText;

	return final;
}

bool intersect(SDL_Rect rect1, SDL_Rect rect2) {
	return (rect1.x < rect2.x + rect2.w &&
		rect1.x + rect1.w > rect2.x &&
		rect1.y < rect2.y + rect2.h &&
		rect1.y + rect1.h > rect2.y);
}

int random(int min, int max) {
	return min + std::rand() % (max - min + 1);
}

Mix_Chunk* loadShootingSound(const char* filename) {
	Mix_Chunk* shootingSound = Mix_LoadWAV(filename);
	if (!shootingSound) {
		std::cerr << "Failed to load shooting sound: " << Mix_GetError() << std::endl;
	}
	return shootingSound;
}