#pragma once
#include "Incident.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Time.hpp>    // ← для sf::Time
#include <vector> 

class ParkingViolation: public Incident {
public:
	ParkingViolation(const sf::Vector2f &position, const sf::Texture& carTex, const sf::Texture& questionTex,
		unsigned frameCount,           // число кадров в спрайт-шите
		sf::Vector2u frameSize,        // размер одного кадра
		float frameDuration = 0.1f
	);
	void update(const sf::Vector2f& playerPos, sf::Time dt) override;
	void render(sf::RenderTarget& target) override;
	bool isPlayerInRange() const { return mPlayerNear; }
	bool isResolved() const override { return mResolved; }

	// вызываем из GameLogic при «решении» мини-игры
	void resolve() { mResolved = true; }
	sf::FloatRect getCarBounds() const {
		return mCar.getGlobalBounds();
	}

private:
	sf::Sprite mCar;
	sf::Sprite mQuestion;    // знак «?»
	std::vector<sf::IntRect> mFrames;
	unsigned mFrameCount;
	unsigned mCurrentFrame = 0;
	float mFrameDuration; // сколько секунд каждый кадр
	float mElapsedTime = 0.f;

	float      mTriggerDist = 50.f;
	bool       mPlayerNear = false;
	bool       mResolved = false;
};