#include "game.h"
#include "ParkingViolation.h"
#include <cstdlib>
#include <iostream>

Game::Game() :
	mWindow(sf::VideoMode(1920, 1080), "Beat Cop prototype")
{
	mPlayer.setPosition(sf::Vector2f(400.f, 300.f));

	if (!mBackgroundTexture.loadFromFile("assets/images/background.png")) {
		std::cerr << "Import error background!" << std::endl;
	}
	mBackgroundSprite.setTexture(mBackgroundTexture);
	
	//Масштабируем фон под размеры окнаж
	sf::Vector2u textureSize = mBackgroundTexture.getSize();
	sf::Vector2u windowSize = mWindow.getSize();
	mBackgroundSprite.setScale(
		static_cast<float>(windowSize.x) / textureSize.x,
		static_cast<float>(windowSize.y) / textureSize.y);
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

void Game::update(sf::Time deltaTime)
{
	mPlayer.update(deltaTime);
}

void Game::render()
{
	// очищаем окно черным цветом, отрисовываем игрока и отображаем результат
	mWindow.clear(sf::Color::Black);
	mWindow.draw(mBackgroundSprite);
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
