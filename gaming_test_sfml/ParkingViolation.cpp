#include "ParkingViolation.h"
#include <cmath>
#include <SFML/System/Time.hpp>
#include <vector>
#include <iostream>
#include "FightIncident.h"
ParkingViolation::ParkingViolation(const sf::Vector2f &pos, const sf::Texture& carTex, const sf::Texture& questionTex,
	unsigned frameCount,
	sf::Vector2u frameSize,
	float frameDuration):
	mCar(carTex), mQuestion(questionTex), mFrameCount(frameCount), mFrameDuration(frameDuration)
{
	mCar.setScale(1.5f, 1.5f);
	mCar.setPosition(pos);
	for (unsigned i = 0; i < mFrameCount; ++i) {
		mFrames.emplace_back(
			i * frameSize.x, 0,
			frameSize.x, frameSize.y
		);
	}
	// выставить первый кадр
	mQuestion.setTextureRect(mFrames[0]);

	// поставить центр вращени€ спрайта вопроса в его середину
	mQuestion.setOrigin(frameSize.x / 2.f, frameSize.y / 2.f);

	// поставим Ђ?ї чуть выше крыши
	mQuestion.setScale(1.5f, 1.5f);
	auto carBounds = mCar.getGlobalBounds();
	auto questionBounds = mQuestion.getLocalBounds();

	// выравниваем У?Ф по центру крыши машины
	float qx = carBounds.left + carBounds.width * 0.5f;
	float qy = carBounds.top - questionBounds.height*0.25f - 14.f;

	mQuestion.setPosition(qx, qy);

	if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf")) {
		std::cout << "cant load font!" << std::endl;
	}
	
}


void ParkingViolation::update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds)
{
	
	if (mResolved) return; // уже убрали

	mElapsedTime += dt.asSeconds();
	if (mElapsedTime >= mFrameDuration) {
		mElapsedTime -= mFrameDuration;
		mCurrentFrame = (mCurrentFrame + 1) % mFrameCount;
		mQuestion.setTextureRect(mFrames[mCurrentFrame]);
	}

	sf::FloatRect carRect = mCar.getGlobalBounds();
	float buffer = 8.f; // увеличь, если хочетс€ УширеФ зону
	carRect.left -= buffer;
	carRect.top -= buffer;
	carRect.width += 2 * buffer;
	carRect.height += 2 * buffer;

	mPlayerNear = carRect.intersects(playerBounds);
}


void ParkingViolation::render(sf::RenderTarget& target)
{
	if (mResolved) return;
	target.draw(mCar);
	target.draw(mQuestion);

	if (mPlayerNear && !mResolved) { // “олько если игрок р€дом и не решено!
		sf::Text pressE;
		pressE.setFont(mFont);
		pressE.setString("Press E for interacting");
		pressE.setCharacterSize(20);
		pressE.setFillColor(sf::Color::White);
		pressE.setOutlineColor(sf::Color::Black);
		pressE.setOutlineThickness(2.f);
		//  оординаты Ч над машиной или над знаком "?"
		float x = 1280 - 350;
		float y = 960 - 60;
		pressE.setPosition(x,y);
		target.draw(pressE);
	}
}
