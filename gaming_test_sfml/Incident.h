#pragma once
#include <SFML/Graphics.hpp>

class Incident {
public:
	virtual ~Incident() = default;

	// Обновление логики инцидента
	virtual void update(sf::Time deltaTime) = 0;
	// Отрисовка инцидента на экране
	virtual void render(sf::RenderWindow& window) = 0;
	// Метод для опредедения, завершён ли инцедент
	virtual bool isCompleted() const = 0;
};