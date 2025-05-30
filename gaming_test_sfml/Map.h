#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>

class Map : public sf::Drawable, public sf::Transformable {
public:
    // загружает JSON и строит два слоя (base + obstacles)
    bool loadFromJSON(const std::string& jsonPath);

    // проверка, можно ли ходить по тайлу (tx,ty)
    bool isWalkable(int tx, int ty) const;

    sf::Vector2u getTileSize() const { return mTileSize; }
    sf::Vector2u getMapSize() const { return mMapSize; }


private:
    struct Tileset {
        int               firstGid;
        sf::Texture       texture;
        int               tileWidth;
        int               tileHeight;
        int               columns;     // сколько тайлов помещается по X
    };

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

    sf::Vector2u                 mTileSize;
    sf::Vector2u                 mMapSize;
    std::vector<Tileset>         mTilesets;
    std::vector<sf::VertexArray> mBaseLayers;      // по одному VA на каждый tileset
    std::vector<sf::VertexArray> mObstacleLayers;  // по одному VA на каждый tileset
    std::vector<int>             mCollisionMask;   // 1=стена, 0=свободно

    struct SpawnZone {
        sf::FloatRect rect;       // { x, y, width, height }
        std::string   orientation; // "side" или "front"
        std::string   color;       // "red", "blue" и т.п.
        std::string incidentType = "car";
    };
    std::vector<SpawnZone> mSpawnZones;

public:
    const std::vector<SpawnZone>& getSpawnZones() const { return mSpawnZones; };

};