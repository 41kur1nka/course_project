#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"

enum class GameState;

class MainMenuScreen {
public:
	// stateRef — ссылка на переменную в Game, чтобы менять её при кликах
	explicit MainMenuScreen(GameState& stateRef);

	// Загрузить все ассеты (текстуры кнопок и фон)
	void loadAssets(const sf::RenderWindow& window);

	// Обновить (проверить кнопки)
	void update(const sf::RenderWindow& window);

	// Отрисовать
	void draw(sf::RenderTarget& target);

private:
	GameState& mState;
	sf::Texture mBgTex;	
	sf::Sprite mBgSprite;
	sf::Texture startN, startH, startP;
	sf::Texture scoreN, scoreH, scoreP;
	sf::Texture setN, setH, setP;
	std::vector<Button> mButtons;
};