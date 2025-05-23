#pragma once
#include <SFML/Graphics.hpp>

class Incident {
public:
	virtual ~Incident() = default;

	// ���������� ������ ���������
	virtual void update(const sf::Vector2f& playerPos, sf::Time dt) = 0;
	// ��������� ��������� �� ������
	virtual void render(sf::RenderTarget& target) = 0;
	// ����� ��� �����������, �������� �� ��������
	virtual bool isResolved() const = 0;
};