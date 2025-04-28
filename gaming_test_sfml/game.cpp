#include "game.h"
#include "ParkingViolation.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;


// Простейшая загрузка двух первых tile-слоев: первый — база, второй — коллизии
bool loadMapJSON(const std::string& path,
    int& W, int& H,
    int& tileW, int& tileH,
    std::vector<int>& rawBase,
    std::vector<int>& rawCollision)
{
    std::ifstream in(path);
    if (!in.is_open()) return false;
    json j;
    in >> j;

    // размеры карты и тайлов
    W = j["width"].get<int>();
    H = j["height"].get<int>();
    tileW = j["tilewidth"].get<int>();
    tileH = j["tileheight"].get<int>();

    // читаем первые два слоя типа "tilelayer"
    int found = 0;
    for (auto& layer : j["layers"]) {
        if (layer["type"] == "tilelayer") {
            auto data = layer["data"].get<std::vector<int>>();
            if (found == 0) rawBase = std::move(data);
            else if (found == 1) rawCollision = std::move(data);
            ++found;
            if (found == 2) break;
        }
    }
    return found == 2
        && rawBase.size() == size_t(W) * size_t(H)
        && rawCollision.size() == size_t(W) * size_t(H);
}

Game::Game()
    : mWindow(sf::VideoMode(1920, 1080), "Beat Cop Prototype")
{
    int W, H, tileW, tileH;
    std::vector<int> rawBase, rawCollision;

    if (!loadMapJSON("assets/images/map.json",
        W, H, tileW, tileH,
        rawBase, rawCollision))
    {
        throw std::runtime_error("Failed to load map.json");
    }

    // сохраняем размеры для дальнейшей логики
    mMapSize = { static_cast<unsigned>(W), static_cast<unsigned>(H) };
    mTileSize = { static_cast<unsigned>(tileW), static_cast<unsigned>(tileH) };

    // загружаем PNG-тайлсет
    if (!mTilesetTexture.loadFromFile("assets/images/tilemap_packed.png"))
        throw std::runtime_error("Failed to load tileset.png");

    std::cout << "[DEBUG] map.json loaded: " << W << "x" << H
        << ", tile=" << tileW << "x" << tileH << std::endl;
    std::cout << "[DEBUG] tileset.png loaded: "
        << mTilesetTexture.getSize().x << "x"
        << mTilesetTexture.getSize().y << std::endl;

    // строим VertexArray для базового слоя
    static constexpr unsigned FLIPPED_H = 0x80000000;
    static constexpr unsigned FLIPPED_V = 0x40000000;
    static constexpr unsigned FLIPPED_D = 0x20000000;
    static constexpr unsigned FLIPPED_ALL = FLIPPED_H | FLIPPED_V | FLIPPED_D;

    int tilesPerRow = mTilesetTexture.getSize().x / tileW;
    int tilesPerCol = mTilesetTexture.getSize().y / tileH;
    int maxLocalId = tilesPerRow * tilesPerCol;

    mBaseVertices = sf::VertexArray(sf::Quads, W * H * 4);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int idx = x + y * W;
            unsigned gid = rawBase[idx];
            bool flipH = gid & FLIPPED_H;
            bool flipV = gid & FLIPPED_V;
            bool flipD = gid & FLIPPED_D;
            unsigned plain = gid & ~FLIPPED_ALL;

            if (plain == 0) continue;
            int local = int(plain) - 1;
            if (local < 0 || local >= maxLocalId) continue;
            
            
            int tu = local % tilesPerRow;
            int tv = local / tilesPerRow;
            float tx = float(tu * tileW);
            float ty = float(tv * tileH);

            sf::Vertex* quad = &mBaseVertices[idx * 4];
            // экранные координаты
            float px = float(x * tileW);
            float py = float(y * tileH);
            quad[0].position = { px,         py };
            quad[1].position = { px + tileW, py };
            quad[2].position = { px + tileW, py + tileH };
            quad[3].position = { px,         py + tileH };

            
            sf::Vector2f tc[4] = {
           { tx,           ty           }, // top-left
           { tx + tileW,   ty           }, // top-right
           { tx + tileW,   ty + tileH   }, // bottom-right
           { tx,           ty + tileH   }  // bottom-left
            };
            
            if (flipD) {
                std::swap(tc[1], tc[3]);
            }
            // затем горизонтальное
            if (flipH) {
                std::swap(tc[0], tc[1]);
                std::swap(tc[3], tc[2]);
            }
            // затем вертикальное
            if (flipV) {
                std::swap(tc[0], tc[3]);
                std::swap(tc[1], tc[2]);
            }

            quad[0].texCoords = tc[0];
            quad[1].texCoords = tc[1];
            quad[2].texCoords = tc[2];
            quad[3].texCoords = tc[3];
        }
    }

    mObstacleVertices = sf::VertexArray(sf::Quads, W * H * 4);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            int idx = x + y * W;
            unsigned gid = rawCollision[idx];
            bool flipH = gid & FLIPPED_H;
            bool flipV = gid & FLIPPED_V;
            bool flipD = gid & FLIPPED_D;
            unsigned plain = gid & ~FLIPPED_ALL;

            if(plain == 0) continue;
            int local = int(plain) - 1;
            if (local < 0 || local >= maxLocalId) continue;

            int tu = local % tilesPerRow, tv = local / tilesPerRow;
            float tx = tu * tileW, ty = tv * tileH;

            sf::Vertex* quad = &mObstacleVertices[idx * 4];
            float px = x * tileW, py = y * tileH;
            quad[0].position = { px, py };
            quad[1].position = { px + tileW, py };
            quad[2].position = { px + tileW, py + tileH };
            quad[3].position = { px, py + tileH };

            sf::Vector2f tc[4] = {
           { tx,           ty           }, // top-left
           { tx + tileW,   ty           }, // top-right
           { tx + tileW,   ty + tileH   }, // bottom-right
           { tx,           ty + tileH   }  // bottom-left
            };

            // сначала диагональное (меняет порядок по главной диагонали)
            if (flipD) {
                std::swap(tc[1], tc[3]);
            }
            // затем горизонтальное
            if (flipH) {
                std::swap(tc[0], tc[1]);
                std::swap(tc[3], tc[2]);
            }
            // затем вертикальное
            if (flipV) {
                std::swap(tc[0], tc[3]);
                std::swap(tc[1], tc[2]);
            }
            
            quad[0].texCoords = tc[0];
            quad[1].texCoords = tc[1];
            quad[2].texCoords = tc[2];
            quad[3].texCoords = tc[3];
        }
    }

    // создаём маску коллизий
    mCollisionMask.resize(W * H);
    for (int i = 0; i < W * H; ++i)
        mCollisionMask[i] = (rawCollision[i] > 0 ? 1 : 0);

    // начинаем игрока в центре карты
    float sx = (W * tileW) * 0.5f;
    float sy = (H * tileH) * 0.5f;
    mPlayer.setPosition({ sx, sy });
}

void Game::run()
{
    sf::Clock clock;
    while (mWindow.isOpen()) {
        sf::Time dt = clock.restart();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        switch (event.type) {
        case sf::Event::KeyPressed:
            handlePlayerInput(event.key.code, true);
            if (event.key.code == sf::Keyboard::I)
                spawnIncident();
            break;
        case sf::Event::KeyReleased:
            handlePlayerInput(event.key.code, false);
            break;
        case sf::Event::Closed:
            mWindow.close();
            break;
        default:
            break;
        }
    }
}

void Game::handlePlayerInput(sf::Keyboard::Key key, bool isPressed)
{
    mPlayer.handleInput(key, isPressed);
}

void Game::update(sf::Time dt)
{
    // 1) вычисляем движение
    sf::Vector2f desired = mPlayer.computeMovement(dt);
    // 2) проверяем коллизию по четырём краям
    sf::FloatRect nextB = mPlayer.getBounds();
    nextB.left += desired.x;
    nextB.top += desired.y;

    auto canWalk = [&](float px, float py) {
        int tx = int(px) / mTileSize.x;
        int ty = int(py) / mTileSize.y;
        if (tx < 0 || ty < 0 || tx >= int(mMapSize.x) || ty >= int(mMapSize.y))
            return false;
        return mCollisionMask[tx + ty * mMapSize.x] == 0;
        };

    bool ok = canWalk(nextB.left, nextB.top)
        && canWalk(nextB.left + nextB.width - 1, nextB.top)
        && canWalk(nextB.left, nextB.top + nextB.height - 1)
        && canWalk(nextB.left + nextB.width - 1, nextB.top + nextB.height - 1);

    if (ok)
        mPlayer.move(desired);

    mPlayer.update(dt);
}

void Game::render()
{
    mWindow.clear();
    sf::RenderStates rs;
    rs.texture = &mTilesetTexture;
    mWindow.draw(mBaseVertices, rs);
    mWindow.draw(mObstacleVertices, rs);
    mPlayer.render(mWindow);
    mIncidentManager.render(mWindow);
    mWindow.display();
}

void Game::spawnIncident()
{
    float x = float(std::rand() % (mWindow.getSize().x - 50));
    float y = float(std::rand() % (mWindow.getSize().y - 50));
    mIncidentManager.addIncident(
        std::make_unique<ParkingViolation>(sf::Vector2f(x, y))
    );
}
