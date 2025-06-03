#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"

enum class GameState;
class Game; // Forward declaration


class MainMenuScreen {
public:
	explicit MainMenuScreen(GameState& stateRef, Game& gameRef);

	void loadAssets(const sf::RenderWindow& window);

	void update(const sf::RenderWindow& window);

	void draw(sf::RenderTarget& target);

private:
	GameState& mState;
	Game& mGame;
	sf::Texture mBgTex;	
	sf::Sprite mBgSprite;
	sf::Texture startN, startH, startP;
	sf::Texture scoreN, scoreH, scoreP;
	sf::Texture setN, setH, setP;
	std::vector<Button> mButtons;
};