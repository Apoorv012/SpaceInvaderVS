#include "Scoreboard.hpp"

Scoreboard::Scoreboard() {
	setScore(0);
}

Scoreboard::~Scoreboard() {
	Scoreboard::clearBoard();
}

void Scoreboard::clearBoard() {
	for (auto el : _score) {
		delete el;
	}

	_score.clear();
}

void Scoreboard::setScore(int score) {
	clearBoard();



}