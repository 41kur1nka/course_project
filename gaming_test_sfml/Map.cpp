#include "Map.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <SFML/Graphics.hpp>

static constexpr unsigned FLIPPED_H = 0x80000000;
static constexpr unsigned FLIPPED_V = 0x40000000;
static constexpr unsigned FLIPPED_D = 0x20000000;
static constexpr unsigned FLIPPED_MASK = FLIPPED_H | FLIPPED_V | FLIPPED_D;

bool Map::loadFromJSON(const std::string& jsonPath)
{
    std::filesystem::path baseDir = std::filesystem::path(jsonPath).parent_path();

    // 1) Загрузка JSON
    std::ifstream in(jsonPath);
    if (!in.is_open())
        throw std::runtime_error("Failed to open " + jsonPath);
    nlohmann::json j;
    in >> j;

    // 2) Читаем размеры
    unsigned W = j.at("width").get<unsigned>();
    unsigned H = j.at("height").get<unsigned>();
    unsigned tileW = j.at("tilewidth").get<unsigned>();
    unsigned tileH = j.at("tileheight").get<unsigned>();
    mMapSize = { W, H };
    mTileSize = { tileW, tileH };

    // 3) Загружаем ВСЕ встроенные tilesets
    for (auto& ts : j.at("tilesets")) {
        if (!ts.contains("image"))
            throw std::runtime_error("External .tsx not supported: embed tilesets in JSON");
        Tileset t;
        t.firstGid = ts.at("firstgid").get<int>();
        t.tileWidth = ts.at("tilewidth").get<int>();
        t.tileHeight = ts.at("tileheight").get<int>();
        int imageW = ts.at("imagewidth").get<int>();
        t.columns = imageW / t.tileWidth;

        std::filesystem::path imgPath = baseDir / ts.at("image").get<std::string>();
        if (!t.texture.loadFromFile(imgPath.string()))
            throw std::runtime_error("Failed to load tileset image: " + imgPath.string());
        mTilesets.push_back(std::move(t));
    }

    // 4) Подготовка vertex arrays и mask
    size_t nTs = mTilesets.size();
    mBaseLayers.resize(nTs);
    mObstacleLayers.resize(nTs);
    for (size_t i = 0; i < nTs; ++i) {
        mBaseLayers[i].setPrimitiveType(sf::Quads);
        mObstacleLayers[i].setPrimitiveType(sf::Quads);
    }
    mCollisionMask.assign(W * H, 0);

    // 5) Читаем два слоя из JSON (base + obstacles)
    std::vector<int> rawBase, rawObs;
    int layerCount = 0;
    for (auto& layer : j.at("layers")) {
        if (layer.at("type").get<std::string>() != "tilelayer") continue;
        auto data = layer.at("data").get<std::vector<int>>();
        if (layerCount == 0) rawBase = std::move(data);
        else if (layerCount == 1) rawObs = std::move(data);
        if (++layerCount >= 2) break;
    }
    if (rawBase.size() != W * H || rawObs.size() != W * H)
        throw std::runtime_error("Layer size mismatch");

    // 6) Построение вершин
    for (unsigned y = 0; y < H; ++y) {
        for (unsigned x = 0; x < W; ++x) {
            unsigned idx = x + y * W;
            unsigned gids[2] = { static_cast<unsigned>(rawBase[idx]), static_cast<unsigned>(rawObs[idx]) };
            for (int layerId = 0; layerId < 2; ++layerId) {
                unsigned gid = gids[layerId] & ~FLIPPED_MASK;
                if (gid == 0) continue;
                // Найти tileset: последний with firstGid <= gid
                const Tileset* ts = nullptr;
                for (auto it = mTilesets.rbegin(); it != mTilesets.rend(); ++it) {
                    if (gid >= it->firstGid) { ts = &*it; break; }
                }
                if (!ts) continue;
                unsigned local = gid - ts->firstGid;
                int tu = local % ts->columns;
                int tv = local / ts->columns;
                float tx = float(tu * ts->tileWidth);
                float ty = float(tv * ts->tileHeight);
                float px = float(x * tileW);
                float py = float(y * tileH);
                sf::Vertex quad[4];
                quad[0].position = { px,        py };
                quad[1].position = { px + tileW,  py };
                quad[2].position = { px + tileW, py + tileH };
                quad[3].position = { px,      py + tileH };
                sf::Vector2f uv[4] = { {tx,ty},{tx + tileW,ty},{tx + tileW,ty + tileH},{tx,ty + tileH} };
                // flips (опционально, если нужны)
                if (gids[layerId] & FLIPPED_H) { std::swap(uv[0], uv[1]); std::swap(uv[2], uv[3]); }
                if (gids[layerId] & FLIPPED_V) { std::swap(uv[0], uv[3]); std::swap(uv[1], uv[2]); }
                if (gids[layerId] & FLIPPED_D) std::swap(uv[1], uv[3]);
                for (int i = 0; i < 4; ++i) quad[i].texCoords = uv[i];
                size_t tsi = ts - &mTilesets[0];
                auto& layerVA = (layerId == 0 ? mBaseLayers[tsi] : mObstacleLayers[tsi]);
                for (int i = 0; i < 4; ++i) layerVA.append(quad[i]);
                if (layerId == 1) mCollisionMask[idx] = 1;
            }
        }
    }


    // 7) обработка spawnzones
    for (auto& layer : j.at("layers")) {
        if (layer.at("type").get<std::string>() != "objectgroup" ||
            layer.at("name").get<std::string>() != "SpawnZones")
            continue;

        for (auto& obj : layer.at("objects")) {
            // отфильтруем только прямоугольники
            if (!obj.contains("width") || !obj.contains("height"))
                continue;

            SpawnZone sz;
            sz.rect.left = obj.at("x").get<float>();
            sz.rect.top = obj.at("y").get<float>();
            sz.rect.width = obj.at("width").get<float>();
            sz.rect.height = obj.at("height").get<float>();

            // дефолты:
            sz.orientation = "side";
            sz.color = "red";

            if (obj.contains("properties")) {
                for (auto& prop : obj.at("properties")) {
                    auto name = prop.at("name").get<std::string>();
                    auto val = prop.at("value").get<std::string>();
                    if (name == "orientation") sz.orientation = val;
                    if (name == "color")       sz.color = val;
                }
            }
            mSpawnZones.push_back(std::move(sz));
        }
        break;
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
    // Рисуем каждый tileset по очереди
    for (size_t i = 0; i < mTilesets.size(); ++i) {
        states.texture = &mTilesets[i].texture;
        target.draw(mBaseLayers[i], states);
        target.draw(mObstacleLayers[i], states);
    }
}
