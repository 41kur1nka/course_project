#pragma once
#include <SFML/Graphics.hpp>

class Incident {
public:
	virtual ~Incident() = default;

	// Обновление логики инцидента
	virtual void update(const sf::Vector2f& playerPos, sf::Time dt, const sf::FloatRect& playerBounds) = 0;
	// Отрисовка инцидента на экране
	virtual void render(sf::RenderTarget& target) = 0;
	// Метод для опредедения, завершён ли инцедент
	virtual bool isResolved() const = 0;

	virtual void resolve() = 0;
};