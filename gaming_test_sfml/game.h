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
	
	// JSON-�����
	/*bool loadMapJSON(const std::string & path);*/

	// �������� ����� � ���
	sf::Texture     mTilesetTexture;
	sf::VertexArray mBaseVertices;   // VertexArray ��� ���� "Base"
	sf::VertexArray mObstacleVertices;

	// �������
	sf::Vector2u    mTileSize;       // (tilewidth, tileheight)
	sf::Vector2u    mMapSize;        // (width, height) � ������

	// ��������: 1 = �����, 0 = ������
	std::vector<int> mCollisionMask;

	// �������������� JSON-���� �������� (objectgroup)
	std::vector<json> mObjectLayers;

};