#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class TileMap : public sf::Drawable, public sf::Transformable
{
public:
	bool load(const std::string& tileset,
		sf::Vector2u tileSize,
		const std::vector<int>& tiles,
		unsigned int width,
		unsigned int height,
		const std::vector<int>& collisionMask);
	
	bool isWalkable(int tileX, int tileY) const;

	sf::Vector2u getTileSize() const { return m_tileSize; }
private:
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	sf::VertexArray m_vertices;
	sf::Texture m_tileset;
	sf::Vector2u m_tileSize;
	unsigned int m_width{}, m_height{};
	std::vector<int> m_collision;
};