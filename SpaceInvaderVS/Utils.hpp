#pragma once

#include <iostream>
#include <string>

std::string getScoreString(int score, int numOfDigits) {
	std::string scorePrefix("SCORE: ");
	std::string scoreText = std::to_string(score);
	std::string final = scorePrefix + std::string(numOfDigits - scoreText.length(), '0') + scoreText;

	return final;
}

