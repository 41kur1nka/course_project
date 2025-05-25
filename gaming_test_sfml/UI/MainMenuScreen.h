#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "Button.h"

enum class GameState;

class MainMenuScreen {
public:
	// stateRef � ������ �� ���������� � Game, ����� ������ � ��� ������
	explicit MainMenuScreen(GameState& stateRef);

	// ��������� ��� ������ (�������� ������ � ���)
	void loadAssets(const sf::RenderWindow& window);

	// �������� (��������� ������)
	void update(const sf::RenderWindow& window);

	// ����������
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