#include "Player.h"
#include <iostream>
Player::Player() :mSpeed(300.f),
mIsMovingUp(false),
mIsMovingDown(false),
mIsMovingLeft(false),
mIsMovingRight(false),
mFramecount(4),
mCurrentFrame(0),
mFrameDuration(sf::seconds(0.15f)),
mElapsedTime(sf::Time::Zero)
{
	if (!mRightTexture.loadFromFile("assets/images/player_spritesheet_right.png")) {
		std::cout << "Error to load spritesheet" << std::endl;
	}
	if (!mLeftTexture.loadFromFile("assets/images/player_spritesheet_left.png")) {
		std::cout << "Error to load spritesheet" << std::endl;
	}
	mSprite.setTexture(mRightTexture);

	//Спрайтлист состоит из горизональных  кадров
	mFrameWidth = static_cast<float>(mRightTexture.getSize().x) / mFramecount;
	mFrameHeight = static_cast<float>(mRightTexture.getSize().y);

	//Первый кадр
	mFrameRect = sf::IntRect(0, 0, static_cast<int>(mFrameWidth), static_cast<int>(mFrameHeight));
	mSprite.setTextureRect(mFrameRect);

	//Стартовая позиция
	mSprite.setPosition(400.f, 300.f);
}

void Player::update(Time deltaTime)
{
	updateDirection();

	Vector2f movement(0.f, 0.f);
	if (mIsMovingUp)
		movement.y -= mSpeed;
	if (mIsMovingDown)
		movement.y += mSpeed;
	if (mIsMovingLeft)
		movement.x -= mSpeed;
	if (mIsMovingRight)
		movement.x += mSpeed;

	mSprite.move(movement * deltaTime.asSeconds());

	if (mIsMovingUp || mIsMovingDown || mIsMovingLeft || mIsMovingRight) {
		updateAnimation(deltaTime);
	}
}

void Player::render(RenderWindow& window)
{
	window.draw(mSprite);
}

void Player::handleInput(Keyboard::Key key, bool isPressed)
{
	if (key == Keyboard::W || key == Keyboard::Up)
		mIsMovingUp = isPressed;
	else if (key == Keyboard::S || key == Keyboard::Down)
		mIsMovingDown = isPressed;
	else if (key == Keyboard::A || key == Keyboard::Left)
		mIsMovingLeft = isPressed;
	else if (key == Keyboard::D || key == Keyboard::Right)
		mIsMovingRight = isPressed;
}

void Player::setPosition(const Vector2f& position)
{
	mSprite.setPosition(position);
}

Vector2f Player::getPosition() const
{
	return mSprite.getPosition();
}

void Player::updateAnimation(sf::Time deltaTime)
{
	mElapsedTime += deltaTime;
	if (mElapsedTime >= mFrameDuration) {
		mCurrentFrame = (mCurrentFrame + 1) % mFramecount;
		mFrameRect.left = static_cast<int>(mCurrentFrame * mFrameWidth);
		mSprite.setTextureRect(mFrameRect);

		mElapsedTime = sf::microseconds(mElapsedTime.asMicroseconds() % mFrameDuration.asMicroseconds());
	}
}

void Player::updateDirection()
{
	if (mIsMovingLeft && !mIsMovingRight) {
		if (mCurrentDirection != Direction::Left) {
			mCurrentDirection = Direction::Left;
			mSprite.setTexture(mLeftTexture);
		}
	}
	else if (mIsMovingRight && !mIsMovingLeft) {
		if (mCurrentDirection != Direction::Right) {
			mCurrentDirection = Direction::Right;
			mSprite.setTexture(mRightTexture);
		}
	}
}
