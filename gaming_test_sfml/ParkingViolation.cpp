#include "ParkingViolation.h"

ParkingViolation::ParkingViolation(const sf::Vector2f &position):
	mElapsedaTime(sf::Time::Zero), mDuration(sf::seconds(5)) // инцидент длится 5 секунд
{
	mShape.setSize(sf::Vector2f(50.f, 50.f));
	mShape.setFillColor(sf::Color::Red);
	mShape.setPosition(position);
}

void ParkingViolation::update(sf::Time deltaTime)
{
	mElapsedaTime += deltaTime;
}

void ParkingViolation::render(sf::RenderTarget& target)
{
	target.draw(mShape);
}

bool ParkingViolation::isCompleted() const
{
	return mElapsedaTime >= mDuration;
}
