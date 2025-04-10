#pragma once
#include <SFML/Graphics.hpp>

class Incident {
public:
	virtual ~Incident() = default;

	// ���������� ������ ���������
	virtual void update(sf::Time deltaTime) = 0;
	// ��������� ��������� �� ������
	virtual void render(sf::RenderWindow& window) = 0;
	// ����� ��� �����������, �������� �� ��������
	virtual bool isCompleted() const = 0;
};