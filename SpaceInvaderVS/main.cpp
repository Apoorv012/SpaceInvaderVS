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
#include "Button.hpp"



bool init() {
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
	return true;
}

bool SDLinit = init();



// Sets the FPS and duration for a particular frame according to the FPS
const int FPS = 60;
const int frameDelay = 1000 / FPS;

// Shows the number of digits in the scoreboard, like 0020
const int DIGITS_IN_SCORE = 4;

// For matching the framerate
// We would first meaasure the time elapsed by rendering the frame, and would delay
// the remaining time, for constant frame rate
Uint32 frameStart;
int frameTime;

// MainWindow
RenderWindow window("Space Invader by Apoorv Mittal", 800, 600);

// BG
SDL_Texture* homeScreenTexture = window.loadTexture("res/gfx/HomeScreen BG.png");
SDL_Texture* endScreenWonTexture = window.loadTexture("res/gfx/EndScreenWon BG.png");
SDL_Texture* endScreenLostTexture = window.loadTexture("res/gfx/EndScreenLost BG.png");

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


// List of bunkers
std::vector<Bunker> bunkers;

// Total number of respawns for the player
// For easy mode, keep it to 4 i.e. player can die 4 times, and can still win
// If you want it difficult, change this to 2 lives.
const int numOfLives = 4;
std::vector<Entity> playerLives;

// Enemies based on their layers, there are 3 layers:
// Lower layer have 30 points
// Middle layer have 60 points
// Top layer have 100 points
std::vector<Enemy*> enemyLayerTop, enemyLayerMid, enemyLayerBottom;

// Enemy bullets Vector, which would be populated within the game period
std::vector<Bullet*> enemyBullets;



// Player object
Player player(Vector2f(20, 500), playerTexture, Vector2f(52, 32));



// Making 2 variables displayScore and currentScore
// to prevent constantly making textures... we would make a new texture only if the score has been updated
// i.e. currentScore != displayScore
int displayScore = -1, currentScore = 0;
Text* scoreText;



// Game variables
bool gameRunning = true;
SDL_Event event;
const int timeBetweenShooting = 60;
int timeUntilShooting = timeBetweenShooting;

// Game State
int state = 0; // 0: Home, 1: Game, 2: End
bool hasWon = false;

// 0: no, 1: right, -1: left
//int playerMoveState = 0;
bool playerMoveRight = false;
bool playerMoveLeft = false;
const int playerMoveSpeed = 3;

// Mouse Events
bool mouseClicked = false;
int mouseX = 0, mouseY = 0;

// For checking the collision between the player's bullet with bunker and enemies
bool checkCollision() {
	
	Bullet* bullet = player.getBullet();
	
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
	for (size_t i = 0; i < enemyLayerBottom.size(); ++i) {
		if (enemyLayerBottom[i] != nullptr && intersect(bullet->getRect(), enemyLayerBottom[i]->getRect())) {
			std::cout << "Lower level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//enemyLayerBottom.erase(enemyLayerBottom.begin() + i);
			delete enemyLayerBottom[i];
			enemyLayerBottom[i] = nullptr;

			if (enemyLayerBottom[i] == nullptr && enemyLayerMid[i] == nullptr && enemyLayerTop[i] == nullptr)
			{
				// remove the whole column
				enemyLayerBottom.erase(enemyLayerBottom.begin() + i);
				enemyLayerMid.erase(enemyLayerMid.begin() + i);
				enemyLayerTop.erase(enemyLayerTop.begin() + i);
			}

			currentScore += 30;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// Middle layer of enemies
	for (size_t i = 0; i < enemyLayerMid.size(); ++i) {
		if (enemyLayerMid[i] != nullptr && intersect(bullet->getRect(), enemyLayerMid[i]->getRect())) {
			std::cout << "Mid level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//enemyLayerMid.erase(enemyLayerMid.begin() + i);
			delete enemyLayerMid[i];
			enemyLayerMid[i] = nullptr;

			if (enemyLayerBottom[i] == nullptr && enemyLayerMid[i] == nullptr && enemyLayerTop[i] == nullptr)
			{
				// remove the whole column
				enemyLayerBottom.erase(enemyLayerBottom.begin() + i);
				enemyLayerMid.erase(enemyLayerMid.begin() + i);
				enemyLayerTop.erase(enemyLayerTop.begin() + i);
			}

			currentScore += 60;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// Topmost layer of enemies
	for (size_t i = 0; i < enemyLayerTop.size(); ++i) {
		if (enemyLayerTop[i] != nullptr && intersect(bullet->getRect(), enemyLayerTop[i]->getRect())) {
			std::cout << "Top level enemy " << i << " hit" << std::endl;
			player.clearBullet();
			//enemyLayerTop.erase(enemyLayerTop.begin() + i);
			delete enemyLayerTop[i];
			enemyLayerTop[i] = nullptr;

			if (enemyLayerBottom[i] == nullptr && enemyLayerMid[i] == nullptr && enemyLayerTop[i] == nullptr)
			{
				// remove the whole column
				enemyLayerBottom.erase(enemyLayerBottom.begin() + i);
				enemyLayerMid.erase(enemyLayerMid.begin() + i);
				enemyLayerTop.erase(enemyLayerTop.begin() + i);
			}

			currentScore += 100;
			Mix_PlayChannel(3, hurtSound, 0);
			return true;
		}
	}

	// This function returns false in case the bullet does not hit anything
	return false;
}


// For checking the collision between the enemy's bullet with bunker and the player
bool checkEnemyBulletCollision(int index) {

	Bullet* bullet = enemyBullets[index];

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
			state = 2;
			hasWon = false;
			return true;
		}

		player.newLife();
		playerLives.pop_back();
		return true;
	}

	return false;
}


bool checkAllEnemyBulletCollision() {
	bool hasHit = false;
	
	for (int i = enemyBullets.size() - 1; i >= 0; i--) {
		hasHit |= checkEnemyBulletCollision(i);
	}

	return hasHit;
}



// Home Screen Consts
SDL_Rect homeScreenRect = { 0, 0, 800, 600 };
Text* gameTitle, *gameTitleShadow, *madeByText;
Button exitButton({ 308, 340, 184, 65 }, { 352, 352 }, "EXIT", "res/fonts/Trispace-SemiBold.ttf", 36, { 0, 195, 8, 255 }, { 255, 255, 255, 255 });
Button startButton({ 308, 248, 184, 65 }, { 342, 260 }, "START", "res/fonts/Trispace-SemiBold.ttf", 36, { 0, 195, 8, 255 }, { 255, 255, 255, 255 });

void HomeScreen() {
	// Clear the window
	window.clear();

	// Update
	if (mouseClicked) {
		mouseClicked = false;

		if (exitButton.getIsActive()) {
			gameRunning = false;
			return;
		}

		if (startButton.getIsActive()) {
			state = 1;
			return;
		}
	}

	startButton.update(window.getRenderer(), mouseX, mouseY);
	exitButton.update(window.getRenderer(), mouseX, mouseY);

	// Render
	window.renderImage(homeScreenTexture, &homeScreenRect);

	// UI
	gameTitleShadow->display(window.getRenderer(), 85, 55);
	gameTitle->display(window.getRenderer(), 80, 50);
	madeByText->display(window.getRenderer(), 188, 520);

	exitButton.display(window.getRenderer());
	startButton.display(window.getRenderer());
	
	SDL_SetRenderDrawColor(window.getRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawLine(window.getRenderer(), 180, 560, 620, 560);

}



// Reset variables
void reset() {
	// Clear the vectors
	bunkers.clear();
	playerLives.clear();
	enemyLayerTop.clear();
	enemyLayerMid.clear();
	enemyLayerBottom.clear();

	// Populate the vectors
	for (int i = 0; i < 4; i++) {
		bunkers.push_back(Bunker(Vector2f(92 + 178 * i, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()));
	};

	for (int i = 0; i < numOfLives; ++i) {
		playerLives.push_back(Entity(Vector2f(10 + 62 * i, 560), playerTexture, Vector2f(52, 32)));
	}

	for (int i = 0; i < 8; i++) {
		enemyLayerTop.push_back(new Enemy(Vector2f(136 + i * 71, 120), enemyTexture_1, Vector2f(32, 32)));
		enemyLayerMid.push_back(new Enemy(Vector2f(128 + i * 71, 180), enemyTexture_2, Vector2f(44, 32)));
		enemyLayerBottom.push_back(new Enemy(Vector2f(114 + i * 75, 240), enemyTexture_3, Vector2f(48, 32)));
	}


	currentScore = 0;

	player.newLife();
	player.clearBullet();


	window.clear();
}



// End Screen Consts
SDL_Rect endScreenRect = { 0, 0, 800, 600 };
Text* gameWonTitle, * gameWonTitleShadow, * gameLostTitle, * gameLostTitleShadow;
Button endExitButton({ 308, 380, 184, 65 }, { 352, 392 }, "EXIT", "res/fonts/Trispace-SemiBold.ttf", 36, { 0, 195, 8, 255 }, { 255, 255, 255, 255 });
Button replayButton({ 308, 290, 184, 65 }, { 332, 300 }, "REPLAY", "res/fonts/Trispace-SemiBold.ttf", 36, { 0, 195, 8, 255 }, { 255, 255, 255, 255 });


void EndScreen() {
	// Clear the window
	window.clear();

	// Update
	if (mouseClicked) {
		mouseClicked = false;

		if (endExitButton.getIsActive()) {
			gameRunning = false;
			return;
		}

		if (replayButton.getIsActive()) {
			state = 1;
			reset();
			return;
		}
	}

	replayButton.update(window.getRenderer(), mouseX, mouseY);
	endExitButton.update(window.getRenderer(), mouseX, mouseY);

	if (hasWon) {
		window.renderImage(endScreenWonTexture, &endScreenRect);

		gameWonTitleShadow->display(window.getRenderer(), 241, 56);
		gameWonTitle->display(window.getRenderer(), 236, 51);
	}
	else {
		window.renderImage(endScreenLostTexture, &endScreenRect);

		gameLostTitle->display(window.getRenderer(), 222, 56);
		gameLostTitleShadow->display(window.getRenderer(), 217, 51);
	}

	scoreText->display(window.getRenderer(), 288, 172);

	madeByText->display(window.getRenderer(), 188, 520);

	replayButton.display(window.getRenderer());
	endExitButton.display(window.getRenderer());

	SDL_SetRenderDrawColor(window.getRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawLine(window.getRenderer(), 180, 560, 620, 560);

}



void GameScreen() {
	// Clear the window
	window.clear();

	// Move player
	if (playerMoveRight && !playerMoveLeft) {
		player.moveX(playerMoveSpeed);
	}
	else if (!playerMoveRight && playerMoveLeft) {
		player.moveX(-playerMoveSpeed);
	}

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
			state = 2; // End Screen
			hasWon = true;
		}
		else {
			int index = random(0, enemyLayerTop.size() - 1);
			if (enemyLayerBottom[index] != nullptr) {
				enemyLayerBottom[index]->shoot(enemyBullets, bulletTexture);
			}
			else if (enemyLayerMid[index] != nullptr) {
				enemyLayerMid[index]->shoot(enemyBullets, bulletTexture);
			}
			else {
				enemyLayerTop[index]->shoot(enemyBullets, bulletTexture);
			}
			Mix_PlayChannel(2, gunSound, 0);
		}

		timeUntilShooting += timeBetweenShooting;
	}

	// Check Collision
	if (player.getBullet()) {
		checkCollision();
	}
	checkAllEnemyBulletCollision();

	// UI
	if (displayScore != currentScore) {
		delete scoreText;
		scoreText = new Text(window.getRenderer(), "res/fonts/space_invaders.ttf", 30, getScoreString(currentScore, DIGITS_IN_SCORE), { 0, 0, 255, 255 });
		displayScore = currentScore;
	}
	scoreText->display(window.getRenderer(), 10, 10);

	window.renderUI();
}



// Main Function
int main(int argc, char* argv[])
{
	// Setting the random seed, according to the time
	std::srand(static_cast<unsigned int>(std::time(nullptr)));


	// Play background music continously
	Mix_PlayMusic(backgroundSound, 0);


	// Populate the vectors
	for (int i = 0; i < 4; i++) {
		bunkers.push_back(Bunker(Vector2f(92 + 178 * i, 400), bunkerTexture, Vector2f(84, 64), window.getRenderer()));
	};
	
	for (int i = 0; i < numOfLives; ++i) {
		playerLives.push_back(Entity(Vector2f(10 + 62 * i, 560), playerTexture, Vector2f(52, 32)));
	}

	for (int i = 0; i < 8; i++) {
		enemyLayerTop.push_back(new Enemy(Vector2f(136 + i * 71, 120), enemyTexture_1, Vector2f(32, 32)));
		enemyLayerMid.push_back(new Enemy(Vector2f(128 + i * 71, 180), enemyTexture_2, Vector2f(44, 32)));
		enemyLayerBottom.push_back(new Enemy(Vector2f(114 + i * 75, 240), enemyTexture_3, Vector2f(48, 32)));
	}

	
	// text for the score of player
	scoreText = new Text(window.getRenderer(), "res/fonts/space_invaders.ttf", 30, getScoreString(currentScore, DIGITS_IN_SCORE), {0, 0, 255, 255});
	gameTitle = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 77, "Space Invader", { 225, 207, 51, 255 });
	gameTitleShadow = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 77, "Space Invader", { 101, 0, 110, 255 });
	gameWonTitle = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 76, "You Win", { 225, 207, 51, 255 });
	gameWonTitleShadow = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 76, "You Win", { 101, 0, 110, 255 });
	gameLostTitle = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 76, "You Lost", { 225, 207, 51, 255 });
	gameLostTitleShadow = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 76, "You Lost", { 101, 0, 110, 255 });
	madeByText = new Text(window.getRenderer(), "res/fonts/Trispace-SemiBold.ttf", 32, "Made By Apoorv Mittal", { 76, 191, 255, 255 });

	// Clear the window
	window.clear();

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
					playerMoveLeft = true;
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					playerMoveRight = true;
					break;
				case SDLK_SPACE:
					if (state == 1) {
						player.shoot(gunSound);
					}
					break;
				case SDLK_m:
					// Just for debugging
					
					//state = (state + 1) % 3;
					//Mix_PlayChannel(1, shootingSound, 0);
					break;
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

			case SDL_KEYUP:
				switch (event.key.keysym.sym) {
				case SDLK_LEFT:
				case SDLK_a:
					playerMoveLeft = false;
					break;
				case SDLK_d:
				case SDLK_RIGHT:
					playerMoveRight = false;
					break;
				default:
					break;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button) {
				case SDL_BUTTON_LEFT:
					mouseClicked = true;
					break;
				}
				break;

			case SDL_MOUSEMOTION:
				mouseX = event.motion.x;
				mouseY = event.motion.y;
			}
		}

		// Clear the window
		window.clear();

		switch (state)
		{
		case 0: // Home Screen
			HomeScreen();
			break;


		case 1: // Game
			GameScreen();
			break;
		
		
		case 2: // End Screen
			EndScreen();
			break;
		}

		

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
