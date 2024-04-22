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

constexpr auto DIGITS_IN_SCORE = 5;

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
	SDL_Texture* bulletTexture = window.loadTexture("res/gfx/Laser.png");

	SDL_Texture* enemyTexture_1 = window.loadTexture("res/gfx/Invader_01-1.png");
	SDL_Texture* enemyTexture_2 = window.loadTexture("res/gfx/Invader_02-1.png");
	SDL_Texture* enemyTexture_3 = window.loadTexture("res/gfx/Invader_03-1.png");


	std::vector<Entity> bunkers = {
		Entity(Vector2f(92, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(270, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(448, 400), bunkerTexture, Vector2f(84, 64)),
		Entity(Vector2f(626, 400), bunkerTexture, Vector2f(84, 64)),
	};

	std::vector<Entity> playerLives = {
		Entity(Vector2f(10, 560), playerTexture, Vector2f(52, 32)),
		Entity(Vector2f(72, 560), playerTexture, Vector2f(52, 32)),
	};

	std::vector<Entity> enemyLayerTop, enemyLayerMid, enemyLayerBottom;
	for (int i = 0; i < 8; i++) {
		enemyLayerTop.push_back(Entity(Vector2f(114 + i * 76, 120), enemyTexture_1, Vector2f(32, 32)));
		enemyLayerMid.push_back(Entity(Vector2f(128 + i * 71, 180), enemyTexture_2, Vector2f(44, 32)));
		enemyLayerBottom.push_back(Entity(Vector2f(114 + i * 75, 240), enemyTexture_3, Vector2f(48, 32)));
	}

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
				case SDLK_a:
					player.moveX(-10);
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					player.moveX(10);
					break;
				case SDLK_SPACE:
					player.shoot();
				default:
					break;
				}
				break;
			}
		}

		// Clear the window
		window.clear();

		// Draw characters
		for (Entity& e : bunkers) {
			window.render(e);
		}

		for (Entity& e : playerLives) {
			window.render(e);
		}

		for (Entity& e : enemyLayerTop) {
			window.render(e);
		}
		for (Entity& e : enemyLayerMid) {
			window.render(e);
		}
		for (Entity& e : enemyLayerBottom) {
			window.render(e);
		}

		window.render(player);
		if (player.getBullet()) {
			player.displayBullet(window.getRenderer(), bulletTexture);
		}

		// Update
		player.update();

		// Check Collision
		/*if (player.getBullet()) {
			checkCollision();
		}*/

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