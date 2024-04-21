#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <string>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "Text.hpp"

#define DIGITS_IN_SCORE 5

int main(int argc, char* argv[])
{
	const int FPS = 120;
	const int frameDelay = 1000 / FPS;

	Uint32 frameStart;
	int frameTime;

	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "IMG_Init HAS FAILED. ERROR: " << SDL_GetError() << std::endl;
	}

	if (TTF_Init() == -1) {
		std::cout << "TTF_Init HAS FAILED. ERROR: " << SDL_GetError() << std::endl;
	}

	RenderWindow window("GAME v1.0", 800, 600);

	SDL_Texture* bunkerTexture = window.loadTexture("res/gfx/Bunker.png");
	SDL_Texture* playerTexture = window.loadTexture("res/gfx/Player.png");

	std::vector<Entity> entities = {
		Entity(Vector2f(92, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(270, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(448, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(626, 400), bunkerTexture, Vector2f(84, 64)),
	};

	std::vector<Entity> playerLives = {
		Entity(Vector2f(10, 560), playerTexture, Vector2f(52, 32)),
		Entity(Vector2f(72, 560), playerTexture, Vector2f(52, 32)),
	};

	Player player(Vector2f(20, 500), playerTexture, Vector2f(52, 32));

	int displayScore = -1, currentScore = 0;
	Text *scoreText = new Text(window.getRenderer(), "res/fonts/space_invaders.ttf", 30, getScoreString(currentScore, DIGITS_IN_SCORE), {0, 0, 255, 255});

	bool gameRunning = true;
	SDL_Event event;

	while (gameRunning) {
		frameStart = SDL_GetTicks();

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				gameRunning = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
					player.moveX(-10);
					break;
				case SDLK_RIGHT:
					player.moveX(10);
					break;
				default:
					break;
				}
				break;
			}
		}

		// Clear the window
		window.clear();

		// Draw characters
		for (Entity& e : entities) {
			window.render(e);
		}

		for (Entity& e : playerLives) {
			window.render(e);
		}

		window.render(player);

		// UI
		if (displayScore != currentScore) {
			delete scoreText;
			scoreText = new Text(window.getRenderer(), "res/fonts/space_invaders.ttf", 30, getScoreString(currentScore, DIGITS_IN_SCORE), { 0, 0, 255, 255 });
			displayScore = currentScore;
		}
		scoreText->display(window.getRenderer(), 10, 10);
		window.renderUI();

		// Display
		window.display();

		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}

	window.cleanUp();

	return 0;
}