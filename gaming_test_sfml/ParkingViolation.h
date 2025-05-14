#pragma once
#include "Incident.h"

class ParkingViolation: public Incident {
public:
	ParkingViolation(const sf::Vector2f &position);
	void update(sf::Time deltaTime) override;
	void render(sf::RenderTarget& target) override;
	bool isCompleted() const override;

private:
	sf::RectangleShape mShape;
	sf::Time mElapsedaTime;
	sf::Time mDuration;
};