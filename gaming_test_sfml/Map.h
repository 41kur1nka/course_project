#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class Map : public sf::Drawable, public sf::Transformable {
public:
    // ��������� JSON � ������ ��� ���� (base + obstacles)
    bool loadFromJSON(const std::string& jsonPath);

    // ��������, ����� �� ������ �� ����� (tx,ty)
    bool isWalkable(int tx, int ty) const;

    sf::Vector2u getTileSize() const { return mTileSize; }
    sf::Vector2u getMapSize() const { return mMapSize; }


private:
    struct Tileset {
        int               firstGid;
        sf::Texture       texture;
        int               tileWidth;
        int               tileHeight;
        int               columns;     // ������� ������ ���������� �� X
    };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2u                 mTileSize;
    sf::Vector2u                 mMapSize;
    std::vector<Tileset>         mTilesets;
    std::vector<sf::VertexArray> mBaseLayers;      // �� ������ VA �� ������ tileset
    std::vector<sf::VertexArray> mObstacleLayers;  // �� ������ VA �� ������ tileset
    std::vector<int>             mCollisionMask;   // 1=�����, 0=��������

    struct SpawnZone {
        sf::FloatRect rect;       // { x, y, width, height }
        std::string   orientation; // "side" ��� "front"
        std::string   color;       // "red", "blue" � �.�.
        std::string incidentType = "car";
    };
    std::vector<SpawnZone> mSpawnZones;

public:
    const std::vector<SpawnZone>& getSpawnZones() const { return mSpawnZones; };

};