#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

class Map : public sf::Drawable, public sf::Transformable {
public:
    // ��������� JSON � ������ ��� ���� (base + obstacles)
    bool loadFromJSON(const std::string& jsonPath, const std::string& tilesetPng);

    // ��������, ����� �� ������ �� ����� (tx,ty)
    bool isWalkable(int tx, int ty) const;

    sf::Vector2u getTileSize() const { return mTileSize; }
    sf::Vector2u getMapSize() const { return mMapSize; }

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Texture     mTileset;
    sf::VertexArray mBaseLayer;      // Quads
    sf::VertexArray mObstacleLayer;  // Quads
    std::vector<int> mCollisionMask; // 1=wall,0=free

    sf::Vector2u    mTileSize;       // �� JSON
    sf::Vector2u    mMapSize;        // �� JSON
};
