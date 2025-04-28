#pragma once
#include <SFML/Graphics.hpp>
#include "IncidentManager.h"
#include "Player.h"
#include <nlohmann/json.hpp>
#include <vector>
#include <string>
using json = nlohmann::json;
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
	
	// JSON-карта
	/*bool loadMapJSON(const std::string & path);*/

	// тайловый атлас и фон
	sf::Texture     mTilesetTexture;
	sf::VertexArray mBaseVertices;   // VertexArray для слоя "Base"
	sf::VertexArray mObstacleVertices;

	// размеры
	sf::Vector2u    mTileSize;       // (tilewidth, tileheight)
	sf::Vector2u    mMapSize;        // (width, height) в тайлах

	// коллизии: 1 = стена, 0 = проход
	std::vector<int> mCollisionMask;

	// необработанные JSON-слои объектов (objectgroup)
	std::vector<json> mObjectLayers;

};