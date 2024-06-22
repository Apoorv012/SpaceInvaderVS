#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>

#include "RenderWindow.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "Text.hpp"
#include "main.h"

// Shows the number of digits in the scoreboard, like 0020
const int DIGITS_IN_SCORE = 4;

// For checking the collision between the player's bullet with bunker and enemies
bool checkCollision(Bullet* bullet, Player& player, int& score, Mix_Chunk* hurtSound, std::vector<Bunker>& bunkers, std::vector<Enemy*>& lowerLevel, std::vector<Enemy*>& midLevel, std::vector<Enemy*>& upperLevel) {
	// Bunkers
	for (size_t i = 0; i < bunkers.size(); ++i) {
		if (intersect(bullet->getRect(), bunkers[i].getRect())) {
			std::cout << "Bunker " << i << " hit" << std::endl;
			bunkers[i].loseHealth();

			if (bunkers[i].getHealth() <= 0) {
				bunkers.erase(bunkers.begin() + i);
			}

			Mix_PlayChannel(3, hurtSound, 0);
			player.clearBullet();
			return true;
		}
	}

	// Lower layer of enemies
	for (size_t i = 0; i < lowerLevel.size(); ++i) {
		if (lowerLevel[i] != nullptr && intersect(bullet->getRect(), lowerLevel[i]->getRect())) {
			std::cout << "Lower level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//lowerLevel.erase(lowerLevel.begin() + i);
			delete lowerLevel[i];
			lowerLevel[i] = nullptr;
			score += 30;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// Middle layer of enemies
	for (size_t i = 0; i < midLevel.size(); ++i) {
		if (midLevel[i] != nullptr && intersect(bullet->getRect(), midLevel[i]->getRect())) {
			std::cout << "Mid level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//midLevel.erase(midLevel.begin() + i);
			delete midLevel[i];
			midLevel[i] = nullptr;
			score += 60;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// Topmost layer of enemies
	for (size_t i = 0; i < upperLevel.size(); ++i) {
		if (upperLevel[i] != nullptr && intersect(bullet->getRect(), upperLevel[i]->getRect())) {
			std::cout << "Top level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//upperLevel.erase(upperLevel.begin() + i);
			delete upperLevel[i];
			upperLevel[i] = nullptr;

			// remove the whole column
			lowerLevel.erase(lowerLevel.begin() + i);
			midLevel.erase(midLevel.begin() + i);
			upperLevel.erase(upperLevel.begin() + i);

			score += 100;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// This function returns false in case the bullet does not hit anything
	return false;
}

// For checking the collision between the enemy's bullet with bunker and the player
bool checkEnemyBulletCollision(Bullet* bullet, std::vector<Bunker>& bunkers, Player& player, Mix_Chunk* hurtSound, std::vector<Bullet*>& enemyBullets, int index, std::vector<Entity>& playerLives, bool& gameRunning) {
	
	// Bunkers
	for (size_t i = 0; i < bunkers.size(); i++) {
		if (intersect(bullet->getRect(), bunkers[i].getRect())) {
			std::cout << "Bunker " << i << " hit" << std::endl;
			bunkers[i].loseHealth();

			if (bunkers[i].getHealth() <= 0) {
				bunkers.erase(bunkers.begin() + i);
			}

			enemyBullets.erase(enemyBullets.begin() + index);
			delete bullet;

			Mix_PlayChannel(4, hurtSound, 0);
			return true;
		}
	}

	// Collision Check with player
	if (intersect(bullet->getRect(), player.getRect())) {
		std::cout << "Player hit" << std::endl;
		enemyBullets.erase(enemyBullets.begin() + index);
		delete bullet;
		Mix_PlayChannel(4, hurtSound, 0);

		if (playerLives.size() <= 0) {
			std::cout << "GAME OVER!" << std::endl;
			gameRunning = false;
			return true;
		}

		player.newLife();
		playerLives.pop_back();
		return true;
	}

	return false;
}

// Main Function
int main(int argc, char* argv[])
{
	// Sets the FPS and duration for a particular frame according to the FPS
	const int FPS = 120;
	const int frameDelay = 1000 / FPS;

	// Setting the random seed, according to the time
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	// For matching the framerate
	// We would first meaasure the time elapsed by rendering the frame, and would delay
	// the remaining time, for constant frame rate
	Uint32 frameStart;
	int frameTime;

#pragma region Initialisation

	// Initialising SDL, IMG, TTF, and Mixer
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		std::cout << "SDL_Init HAS FAILED. SDL_ERROR: " << SDL_GetError() << std::endl;
	}

	if (!IMG_Init(IMG_INIT_PNG)) {
		std::cout << "IMG_Init HAS FAILED. ERROR: " << SDL_GetError() << std::endl;
	}

	if (TTF_Init() == -1) {
		std::cout << "TTF_Init HAS FAILED. ERROR: " << SDL_GetError() << std::endl;
	}

	if (!Mix_Init(MIX_INIT_MP3)) {
		std::cout << "Mix_Init HAS FAILED. ERROR: " << SDL_GetError() << std::endl;
	}
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 6, 4096) < 0) {
		std::cout << "Mix_OpenAudio HAS FAILED. ERROR: " << Mix_GetError() << std::endl;
	}

#pragma endregion

	// MainWindow
	RenderWindow window("GAME v1.0", 800, 600);

#pragma region Load Textures and Music

	// Textures
	SDL_Texture* bunkerTexture = window.loadTexture("res/gfx/Bunker.png");
	SDL_Texture* playerTexture = window.loadTexture("res/gfx/Player.png");
	SDL_Texture* bulletTexture = window.loadTexture("res/gfx/Laser.png");

	SDL_Texture* enemyTexture_1 = window.loadTexture("res/gfx/Invader_01-1.png");
	SDL_Texture* enemyTexture_2 = window.loadTexture("res/gfx/Invader_02-1.png");
	SDL_Texture* enemyTexture_3 = window.loadTexture("res/gfx/Invader_03-1.png");

	// Sound
	Mix_Music* backgroundSound = Mix_LoadMUS("res/sfx/background.mp3");

	// We have two different sounds of shooting, both sound fine, so have not removed
	//Mix_Chunk* laserSound = loadShootingSound("res/sfx/laser_shoot.mp3");
	Mix_Chunk* gunSound = loadShootingSound("res/sfx/gun_shoot.mp3");
	Mix_Chunk* hurtSound = loadShootingSound("res/sfx/hurt.mp3");

#pragma endregion

	// Play background music continously
	Mix_PlayMusic(backgroundSound, 0);

	// List of bunkers
	std::vector<Bunker> bunkers = {
		Bunker(Vector2f(92, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(270, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(448, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
		Bunker(Vector2f(626, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()),
	};

	// Total number of respawns for the player
	// For easy mode, keep it to 4 i.e. player can die 4 times, and can still win
	// If you want it difficult, change this to 2 lives.
	const int numOfLives = 4;
	std::vector<Entity> playerLives;
	for (int i = 0; i < numOfLives; ++i) {
		playerLives.push_back(Entity(Vector2f(10 + 62 * i, 560), playerTexture, Vector2f(52, 32)));
	}

	// Enemies based on their layers, there are 3 layers:
	// Lower layer have 30 points
	// Middle layer have 60 points
	// Top layer have 100 points
	std::vector<Enemy*> enemyLayerTop, enemyLayerMid, enemyLayerBottom;
	for (int i = 0; i < 8; i++) {
		enemyLayerTop.push_back(new Enemy(Vector2f(136 + i * 71, 120), enemyTexture_1, Vector2f(32, 32)));
		enemyLayerMid.push_back(new Enemy(Vector2f(128 + i * 71, 180), enemyTexture_2, Vector2f(44, 32)));
		enemyLayerBottom.push_back(new Enemy(Vector2f(114 + i * 75, 240), enemyTexture_3, Vector2f(48, 32)));
	}

	// Enemy bullets Vector, which would be populated within the game period
	std::vector<Bullet*> enemyBullets;

	// Player object
	Player player(Vector2f(20, 500), playerTexture, Vector2f(52, 32));

	// Making 2 variables displayScore and currentScore
	// to prevent constantly making textures... we would make a new texture only if the score has been updated
	// i.e. currentScore != displayScore
	int displayScore = -1, currentScore = 0;
	Text *scoreText = new Text(window.getRenderer(), "res/fonts/space_invaders.ttf", 30, getScoreString(currentScore, DIGITS_IN_SCORE), {0, 0, 255, 255});

	// Game variables
	bool gameRunning = true;
	SDL_Event event;
	int timeBetweenShooting = 100;
	int timeUntilShooting = timeBetweenShooting;

	// Main game loop
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
					player.shoot(gunSound);
					break;
				//case SDLK_m:
				//	Mix_PlayChannel(1, shootingSound, 0);
				//	break;
				case SDLK_p:
					if (Mix_PausedMusic()) {
						Mix_ResumeMusic();
					}
					else {
						Mix_PauseMusic();
					}
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
		for (Bunker& e : bunkers) {
			window.render(e);
			e.showHealth();
		}

		for (Entity& e : playerLives) {
			window.render(e);
		}

		for (Entity* e : enemyLayerTop) {
			if (e == nullptr) {
				continue;
			}
			window.render(*e);
		}
		for (Entity* e : enemyLayerMid) {
			if (e == nullptr) {
				continue;
			}
			window.render(*e);
		}
		for (Entity* e : enemyLayerBottom) {
			if (e == nullptr) {
				continue;
			}
			window.render(*e);
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
		if (timeUntilShooting-- < 0) {
			if (enemyLayerTop.size() <= 0) {
				std::cout << "YOU WIN!" << std::endl;
				gameRunning = false;
			}
			else {
				int index = random(0, enemyLayerTop.size() - 1);
				if (enemyLayerMid[index] != nullptr) {
					// middle row enemy exists
					if (enemyLayerBottom[index] != nullptr) {
						enemyLayerBottom[index]->shoot(enemyBullets, bulletTexture);
					}
					// either middle row would shoot, or bottom row
					else {
						enemyLayerMid[index]->shoot(enemyBullets, bulletTexture);
					}
				}
				else {
					// top row shoots
					enemyLayerTop[index]->shoot(enemyBullets, bulletTexture);
				}
				Mix_PlayChannel(2, gunSound, 0);
			}
			/*
			if (enemyLayerBottom.size() > 0) {
				int index = random(0, enemyLayerBottom.size() - 1);
				if (enemyLayerBottom[index] != nullptr) {
					enemyLayerBottom[index]->shoot(enemyBullets, bulletTexture);
				}
				Mix_PlayChannel(2, gunSound, 0);
			}
			else if (enemyLayerMid.size() > 0) {
				int index = random(0, enemyLayerMid.size() - 1);
				if (enemyLayerMid[index] != nullptr) {
					enemyLayerMid[index]->shoot(enemyBullets, bulletTexture);
				}
				Mix_PlayChannel(2, gunSound, 0);
			}
			else if (enemyLayerTop.size() > 0) {
				int index = random(0, enemyLayerTop.size() - 1);
				if (enemyLayerTop[index] != nullptr) {
					enemyLayerTop[index]->shoot(enemyBullets, bulletTexture);
				}
				Mix_PlayChannel(2, gunSound, 0);
			}
			else {
				std::cout << "YOU WIN!" << std::endl;
				gameRunning = false;
			}
			*/
			
			timeUntilShooting += timeBetweenShooting;
		}

		// Check Collision
		if (player.getBullet()) {
			checkCollision(player.getBullet(), player, currentScore, hurtSound, bunkers, enemyLayerBottom, enemyLayerMid, enemyLayerTop);
		}
		for (int i = enemyBullets.size() - 1; i >= 0; i--) {
			checkEnemyBulletCollision(enemyBullets[i], bunkers, player, hurtSound, enemyBullets, i, playerLives, gameRunning);
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

	// Clean UP
	Mix_HaltMusic();
	window.cleanUp();

	// Don't end abruptly
	std::getchar();

	// If all went well, it would return 0
	return 0;
}
