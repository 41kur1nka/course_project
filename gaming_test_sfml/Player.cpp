#include "Player.h"
#include <iostream>
Player::Player() :mSpeed(140.f),
mIsMovingUp(false),
mIsMovingDown(false),
mIsMovingLeft(false),
mIsMovingRight(false),
mFramecount(12),
mCurrentFrame(0),
mFrameDuration(sf::seconds(0.15f)),
mElapsedTime(sf::Time::Zero)
{
	if (!mRightTexture.loadFromFile("assets/images/right_walk.png")) {
		std::cout << "Error to load spritesheet" << std::endl;
	}
	if (!mLeftTexture.loadFromFile("assets/images/left_walk.png")) {
		std::cout << "Error to load spritesheet" << std::endl;
	}
	if (!mRightTexture2.loadFromFile("assets/images/right_walk2.png")) {
		std::cout << "Error to load right_walk2" << std::endl;
	}
	if (!mLeftTexture2.loadFromFile("assets/images/left_walk2.png")) {
		std::cout << "Error to load left_walk2" << std::endl;
	}
	if (!mRightTexture3.loadFromFile("assets/images/right_walk3.png")) {
		std::cout << "Error to load right_walk3" << std::endl;
	}
	if (!mLeftTexture3.loadFromFile("assets/images/left_walk3.png")) {
		std::cout << "Error to load left_walk3" << std::endl;
	}

	mRightTextureToUse = &mRightTexture;
	mLeftTextureToUse = &mLeftTexture;

	mSprite.setTexture(mRightTexture);

	mFrameWidth = static_cast<float>(mRightTexture.getSize().x) / mFramecount;
	mFrameHeight = static_cast<float>(mRightTexture.getSize().y);

	mFrameRect = sf::IntRect(0, 0, static_cast<int>(mFrameWidth), static_cast<int>(mFrameHeight));
	mSprite.setTextureRect(mFrameRect);

	mSprite.setPosition(400.f, 300.f);
}

void Player::update(Time deltaTime)
{
	updateDirection();

	if (mIsMovingUp || mIsMovingDown || mIsMovingLeft || mIsMovingRight) {
		updateAnimation(deltaTime);
	}
}

void Player::render(RenderTarget& target) const
{
	target.draw(mSprite);
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

sf::Vector2f Player::computeMovement(sf::Time deltaTime)
{
	sf::Vector2f movement(0.f, 0.f);
	if (mIsMovingUp) movement.y -= mSpeed;
	if (mIsMovingDown) movement.y += mSpeed;
	if (mIsMovingLeft) movement.x -= mSpeed;
	if (mIsMovingRight) movement.x += mSpeed;
	return movement* deltaTime.asSeconds();
}

void Player::move(const sf::Vector2f& movement)
{
	mSprite.move(movement);
}

sf::FloatRect Player::getBounds() const
{
	return mSprite.getGlobalBounds();
}

void Player::setSkin(int skinIndex)
{
	if (skinIndex < 0 || skinIndex > 2) {
		std::cout << "!!! BAD SKIN INDEX: " << skinIndex << std::endl;
		skinIndex = 0;
	}
	
	mSkinIndex = skinIndex;
	switch (mSkinIndex) {
	case 0:
		mRightTextureToUse = &mRightTexture;
		mLeftTextureToUse = &mLeftTexture;
		break;
	case 1:
		mRightTextureToUse = &mRightTexture2;
		mLeftTextureToUse = &mLeftTexture2;
		break;
	case 2:
		mRightTextureToUse = &mRightTexture3;
		mLeftTextureToUse = &mLeftTexture3;
		break;
	default:
		mRightTextureToUse = &mRightTexture;
		mLeftTextureToUse = &mLeftTexture;
		break;
	}
	if (mCurrentDirection == Direction::Left)
		mSprite.setTexture(*mLeftTextureToUse);
	else
		mSprite.setTexture(*mRightTextureToUse);
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
	if (mSkinIndex < 0) mSkinIndex = 0;

	if (mIsMovingLeft && !mIsMovingRight) {
		if (mCurrentDirection != Direction::Left) {
			mCurrentDirection = Direction::Left;
			mSprite.setTexture(*mLeftTextureToUse);
		}
	}
	else if (mIsMovingRight && !mIsMovingLeft) {
		if (mCurrentDirection != Direction::Right) {
			mCurrentDirection = Direction::Right;
			mSprite.setTexture(*mRightTextureToUse);
		}
	}
}
