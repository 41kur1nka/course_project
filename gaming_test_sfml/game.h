#pragma once
#include <SFML/Graphics.hpp>
#include "IncidentManager.h"
#include "Player.h"
#include "TileMap.h"
class Game {
public:
	Game();
	void run();
private:
	void processEvents();
	void handlePlayerInput(sf::Keyboard::Key key, bool isPressed);
	void update(sf::Time deltaTime);
	void render();
	void spawnIncident();
	sf::RenderWindow mWindow;
	bool mIsMovingUp, mIsMovingDown, mIsMovingLeft, mIsMovingRight;
	Player mPlayer;
	IncidentManager mIncidentManager;

	sf::Texture mBackgroundTexture;
	sf::Sprite mBackgroundSprite;
	TileMap mMap;
};