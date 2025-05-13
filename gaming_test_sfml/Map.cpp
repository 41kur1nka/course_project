#include "Map.h"
#include <fstream>
#include <iostream>

static constexpr unsigned FLIPPED_H = 0x80000000;
static constexpr unsigned FLIPPED_V = 0x40000000;
static constexpr unsigned FLIPPED_D = 0x20000000;
static constexpr unsigned FLIPPED_MASK = FLIPPED_H | FLIPPED_V | FLIPPED_D;

bool Map::loadFromJSON(const std::string& jsonPath, const std::string& tilesetPng)
{
    std::ifstream in(jsonPath);
    if (!in.is_open())
        throw std::runtime_error("Failed to open " + jsonPath);

    nlohmann::json j;
    in >> j;

    // размеры карты и тайла
    unsigned W = j.at("width").get<unsigned>();
    unsigned H = j.at("height").get<unsigned>();
    unsigned tileW = j.at("tilewidth").get<unsigned>();
    unsigned tileH = j.at("tileheight").get<unsigned>();
    mMapSize = { W, H };
    mTileSize = { tileW, tileH };

    

    // найти нужные слои
    std::vector<int> rawBase, rawObs;
    int found = 0;
    for (auto& layer : j.at("layers")) {
        if (layer.at("type").get<std::string>() != "tilelayer")
            continue;

        auto data = layer.at("data").get<std::vector<int>>();
        if (found == 0) {
            rawBase = std::move(data);
        }
        else if (found == 1) {
            rawObs = std::move(data);
        }
        ++found;
        if (found >= 2) break;
    }
    if (rawBase.size() != W * H || rawObs.size() != W * H)
        throw std::runtime_error("Layer size mismatch");

   

    // --- 2) Загрузка текстуры тайлсета ---
    if (!mTileset.loadFromFile(tilesetPng))
        throw std::runtime_error("Failed to load " + tilesetPng);

    unsigned tilesPerRow = mTileset.getSize().x / tileW;
    unsigned tilesPerCol = mTileset.getSize().y / tileH;
    unsigned maxLocalId = tilesPerRow * tilesPerCol;

    // --- 3) Строим VertexArray для базового слоя ---
    mBaseLayer.setPrimitiveType(sf::Quads);
    mBaseLayer.resize(W * H * 4);

    // --- 4) Строим VertexArray для препятствий и маску коллизий ---
    mObstacleLayer.setPrimitiveType(sf::Quads);
    mObstacleLayer.resize(W * H * 4);
    mCollisionMask.assign(W * H, 0);

    for (unsigned y = 0; y < H; ++y) {
        for (unsigned x = 0; x < W; ++x) {
            unsigned idx = x + y * W;
            // базовый слой
            unsigned gidBase = rawBase[idx];
            // слой препятствий
            unsigned gidObs = rawObs[idx];

            auto buildQuad = [&](unsigned gid, sf::Vertex* quad) {
                bool flipH = gid & FLIPPED_H;
                bool flipV = gid & FLIPPED_V;
                bool flipD = gid & FLIPPED_D;
                unsigned plain = gid & ~FLIPPED_MASK;
                if (plain == 0) return false;

                unsigned local = plain - 1;
                if (local >= maxLocalId) return false;

                unsigned tu = local % tilesPerRow;
                unsigned tv = local / tilesPerRow;
                float tx = float(tu * tileW);
                float ty = float(tv * tileH);

                // позиции вершин в мировых координатах
                float px = float(x * tileW);
                float py = float(y * tileH);
                quad[0].position = { px,           py };
                quad[1].position = { px + tileW,   py };
                quad[2].position = { px + tileW,   py + tileH };
                quad[3].position = { px,           py + tileH };

                // начальные UV
                sf::Vector2f uv[4] = {
                    {tx,         ty},
                    {tx + tileW, ty},
                    {tx + tileW, ty + tileH},
                    {tx,         ty + tileH}
                };

                // диагональный flip
                if (flipD) std::swap(uv[1], uv[3]);
                // горизонтальный
                if (flipH) {
                    std::swap(uv[0], uv[1]);
                    std::swap(uv[3], uv[2]);
                }
                // вертикальный
                if (flipV) {
                    std::swap(uv[0], uv[3]);
                    std::swap(uv[1], uv[2]);
                }

                quad[0].texCoords = uv[0];
                quad[1].texCoords = uv[1];
                quad[2].texCoords = uv[2];
                quad[3].texCoords = uv[3];
                return true;
                };

            // заполняем базовый слой
            sf::Vertex* baseQ = &mBaseLayer[idx * 4];
            buildQuad(gidBase, baseQ);

            // заполняем obstacle-слой + маску коллизий
            sf::Vertex* obsQ = &mObstacleLayer[idx * 4];
            if (buildQuad(gidObs, obsQ)) {
                mCollisionMask[idx] = 1;
            }
        }
    }
    

    return true;

}

bool Map::isWalkable(int tx, int ty) const
{
    if (tx < 0 || ty < 0 || tx >= int(mMapSize.x) || ty >= int(mMapSize.y))
        return false;
    return mCollisionMask[tx + ty * mMapSize.x] == 0;
}

void Map::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.texture = &mTileset;
    target.draw(mBaseLayer, states);
    target.draw(mObstacleLayer, states);
}
