#include "game.h"
#include "ParkingViolation.h"
#include <cstdlib>
#include <iostream>

Game::Game() :
	mWindow(sf::VideoMode(1920, 1080), "Beat Cop prototype")
{
	mPlayer.setPosition(sf::Vector2f(400.f, 300.f));

	const sf::Vector2u tileSize(80, 60);
	if (!mMap.load("assets/images/location.png", tileSize, level, W, H, collisionMask))
		throw std::runtime_error("cant load map");
	//if (!mBackgroundTexture.loadFromFile("assets/images/background.png")) {
	//	std::cerr << "Import error background!" << std::endl;
	//}
	//mBackgroundSprite.setTexture(mBackgroundTexture);
	//
	////Масштабируем фон под размеры окнаж
	//sf::Vector2u textureSize = mBackgroundTexture.getSize();
	//sf::Vector2u windowSize = mWindow.getSize();
	//mBackgroundSprite.setScale(
	//	static_cast<float>(windowSize.x) / textureSize.x,
	//	static_cast<float>(windowSize.y) / textureSize.y);
}

void Game::run()
{
	sf::Clock clock;
	while (mWindow.isOpen()) {
		sf::Time deltaTime = clock.restart(); // вычисляем время между кадрами
		processEvents();
		update(deltaTime);
		render();
	}
}

void Game::processEvents()
{
	sf::Event event;
	while (mWindow.pollEvent(event)) {
		switch (event.type) {
		case sf::Event::KeyPressed:
			handlePlayerInput(event.key.code, true);
			// Создаем инцидент при нажатии клавиши I
			if (event.key.code == sf::Keyboard::I) {
				spawnIncident();
			}
			break;
		case sf::Event::KeyReleased:
			handlePlayerInput(event.key.code, false);
			break;
		case sf::Event::Closed:
			mWindow.close();
			break;
		default:
			break;
		}
	}
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
	mPlayer.handleInput(key, isPressed);
}

void Game::update(sf::Time dt)
{
	// 1. Расчёт желаемого движения
	sf::Vector2f desired = mPlayer.computeMovement(dt);
	// 2. Проверка проходимости
	sf::FloatRect nextBox = mPlayer.getBounds();
	nextBox.left += desired.x;
	nextBox.top += desired.y;

	auto ts = mMap.getTileSize();
	auto check = [&](float x, float y) {
		int tx = int(x) / ts.x;
		int ty = int(y) / ts.y;
		return mMap.isWalkable(tx, ty);
	};

	bool pass = check(nextBox.left, nextBox.top)
		&& check(nextBox.left + nextBox.width - 1, nextBox.top)
		&& check(nextBox.left, nextBox.top + nextBox.height - 1)
		&& check(nextBox.left + nextBox.width - 1, nextBox.top + nextBox.height - 1);

	if (pass)
		mPlayer.move(desired);
}

void Game::render()
{
	// очищаем окно черным цветом, отрисовываем игрока и отображаем результат
	mWindow.clear(sf::Color::Black);
	mWindow.draw(mMap);
	mPlayer.render(mWindow);
	mIncidentManager.render(mWindow);
	mWindow.display();
}

void Game::spawnIncident()
{
	float x = static_cast<float>(std::rand() % (mWindow.getSize().x - 50));
	float y = static_cast<float>(std::rand() % (mWindow.getSize().y - 50));
	mIncidentManager.addIncident(std::make_unique<ParkingViolation>(sf::Vector2f(x, y)));
}
