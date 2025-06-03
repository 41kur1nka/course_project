#pragma once
#include <SFML/Graphics.hpp>

class Incident {
public:
	virtual ~Incident() = default;

	// Updating incident logic
	virtual void update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds) = 0;
	// Drawing incident on screen
	virtual void render(sf::RenderTarget& target) = 0;
	// Incident is complete function
	virtual bool isResolved() const = 0;

	virtual void resolve() = 0;
};