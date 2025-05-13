#pragma once

#include <SFML/Graphics.hpp>
using namespace sf;

enum class Direction{Left, Right};

class Player {
public:
	Player();
	void update(Time deltaTime);
	void render(RenderTarget& target) const;
	void handleInput(Keyboard::Key key, bool isPressed);
	void setPosition(const Vector2f& position);
	Vector2f getPosition() const;

	sf::Vector2f computeMovement(sf::Time deltaTime);
	void move(const sf::Vector2f& movement);
	sf::FloatRect getBounds() const;

private:
	Sprite mSprite;
	Texture mRightTexture;
	Texture mLeftTexture;
	IntRect mFrameRect;

	float mSpeed;
	bool mIsMovingUp;
	bool mIsMovingDown;
	bool mIsMovingLeft;
	bool mIsMovingRight;

	int mFramecount; // Кол-во кадров в анимации
	int mCurrentFrame; // Номер текущего кадра
	float mFrameWidth; // Ширина одного кадра
	float mFrameHeight; // Высота одного кадра
	Time mFrameDuration; // Время отображения одного кадра
	Time mElapsedTime; // Время, прошедшее с последней смены кадров

	void updateAnimation(sf::Time deltaTime);
	void updateDirection();

	Direction mCurrentDirection;
};
