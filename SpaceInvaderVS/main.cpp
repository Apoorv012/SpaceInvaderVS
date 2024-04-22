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

const int DIGITS_IN_SCORE = 5;

bool checkCollision(Bullet* bullet, Player& player, int& score, std::vector<Bunker>& bunkers, std::vector<Enemy>& lowerLevel, std::vector<Enemy>& midLevel, std::vector<Enemy>& upperLevel) {
	for (size_t i = 0; i < bunkers.size(); ++i) {
		if (intersect(bullet->getRect(), bunkers[i].getRect())) {
			std::cout << "Bunker " << i << " hit" << std::endl;
			bunkers[i].loseHealth();

			if (bunkers[i].getHealth() <= 0) {
				bunkers.erase(bunkers.begin() + i);
			}
			player.clearBullet();
			return true;
		}
	}

	for (size_t i = 0; i < lowerLevel.size(); ++i) {
		if (intersect(bullet->getRect(), lowerLevel[i].getRect())) {
			std::cout << "Lower level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			lowerLevel.erase(lowerLevel.begin() + i);
			score += 30;
			return true;
		}
	}

	for (size_t i = 0; i < midLevel.size(); ++i) {
		if (intersect(bullet->getRect(), midLevel[i].getRect())) {
			std::cout << "Mid level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			midLevel.erase(midLevel.begin() + i);
			score += 60;
			return true;
		}
	}

	for (size_t i = 0; i < upperLevel.size(); ++i) {
		if (intersect(bullet->getRect(), upperLevel[i].getRect())) {
			std::cout << "Top level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			upperLevel.erase(upperLevel.begin() + i);
			score += 100;
			return true;
		}
	}

	return false;
}

bool checkEnemyBulletCollision(Bullet* bullet, std::vector<Bunker>& bunkers, Player& player, std::vector<Bullet*>& enemyBullets, int index, std::vector<Entity>& playerLives) {
	for (size_t i = 0; i < bunkers.size(); i++) {
		if (intersect(bullet->getRect(), bunkers[i].getRect())) {
			std::cout << "Bunker " << i << " hit" << std::endl;
			bunkers[i].loseHealth();

			if (bunkers[i].getHealth() <= 0) {
				bunkers.erase(bunkers.begin() + i);
			}

			enemyBullets.erase(enemyBullets.begin() + index);
			delete bullet;
			return true;
		}
	}

	if (intersect(bullet->getRect(), player.getRect())) {
		std::cout << "Player hit" << std::endl;
		enemyBullets.erase(enemyBullets.begin() + index);
		delete bullet;

		if (playerLives.size() <= 0) {
			std::cout << "GAME OVER!";
			return 0;
		}

		player.newLife();
		playerLives.pop_back();
		return true;
	}

	return false;
}

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

	std::vector<Bunker> bunkers = {
		Bunker(Vector2f(92, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(270, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(448, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(626, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
	};

	std::vector<Entity> playerLives = {
		Entity(Vector2f(10, 560), playerTexture, Vector2f(52, 32)),
		Entity(Vector2f(72, 560), playerTexture, Vector2f(52, 32)),
	};

	std::vector<Enemy> enemyLayerTop, enemyLayerMid, enemyLayerBottom;
	for (int i = 0; i < 8; i++) {
		enemyLayerTop.push_back(Enemy(Vector2f(114 + i * 76, 120), enemyTexture_1, Vector2f(32, 32)));
		enemyLayerMid.push_back(Enemy(Vector2f(128 + i * 71, 180), enemyTexture_2, Vector2f(44, 32)));
		enemyLayerBottom.push_back(Enemy(Vector2f(114 + i * 75, 240), enemyTexture_3, Vector2f(48, 32)));
	}

	std::vector<Bullet*> enemyBullets;

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
					break;
				case SDLK_x:
					enemyLayerBottom[0].shoot(enemyBullets, bulletTexture);
				default:
					break;
				}
				break;
			}
		}

		// Clear the window
		window.clear();

		// Draw characters
		for (Bunker& e : bunkers) {
			window.render(e);
			e.showHealth();
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
		for (auto el : enemyBullets) {
			el->display(window.getRenderer(), bulletTexture);
		}

		// Update
		player.update();
		for (auto el : enemyBullets) {
			el->update(&enemyBullets);
		}

		// Check Collision
		if (player.getBullet()) {
			checkCollision(player.getBullet(), player, currentScore, bunkers, enemyLayerBottom, enemyLayerMid, enemyLayerTop);
		}
		for (int i = enemyBullets.size() - 1; i >= 0; i--) {
			checkEnemyBulletCollision(enemyBullets[i], bunkers, player, enemyBullets, i, playerLives);
		}

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
