﻿#include "GameLogic.h"
#include <stdexcept>
#include <cstdlib> 
#include "ParkingViolation.h"
#include "Player.h"
#include <algorithm>
#include <iostream>


GameLogic::GameLogic()
{
    // 1) Загрузить карту из JSON + тайлсета
    if (!mMap.loadFromJSON("assets/images/map.json"))
        throw std::runtime_error("Map load failed in GameLogic");

    // 2) Поставить игрока в стартовую позицию
    //    Пример: в центре экрана или на заданных координатах
    sf::Vector2u mapPx = { mMap.getMapSize().x * mMap.getTileSize().x,
                           mMap.getMapSize().y * mMap.getTileSize().y };
    mPlayer.setPosition({ float(mapPx.x) / 2.f, float(mapPx.y) / 2.f });

    // 3) (опционально) задать стартовый спавн-интервал и т. д.
    // пример спавна одной неправильно припаркованной машины
    mQuestionSheetTexture.loadFromFile("assets/images/mark.png");
    


    // спавним нарушенную парковку 
    for (auto color : { "red","blue","green","white","yellow"}) {
        sf::Texture side;  side.loadFromFile("assets/images/cars/car_side_" + std::string(color) + ".png");
        sf::Texture front; front.loadFromFile("assets/images/cars/car_front_" + std::string(color) + ".png");
        mCarSideTextures[color] = std::move(side);
        mCarFrontTextures[color] = std::move(front);
    }
    // Сколько машин хотим сразу в мире при старте
    const int initialCount = 0;
    for (int i = 0; i < initialCount; ++i) {
        spawnOne();
    }
}

void GameLogic::handleInput(sf::Keyboard::Key key, bool isPressed)
{
    // Пауза
    if (key == sf::Keyboard::Escape && isPressed) {
        togglePause();
        return;
    }

    // Если в мини-игре — по E выходим
    if (mInInteraction) {
        if (key == sf::Keyboard::E && isPressed)
            finishInteraction(true);
        return;
    }

    // Нажали E рядом с машиной?
    if (key == sf::Keyboard::E && isPressed && !mPaused) {
        for (auto& inc : mIncidents) {
            if (auto pv = dynamic_cast<ParkingViolation*>(inc.get())) {
                if (pv->isPlayerInRange()) {
                    mCurrentViolation = pv;
                    startInteraction();
                    return;
                }
            }
        }
    }

    // WASD – только если не в паузе и не в интеракции
    if (!mPaused && !mInInteraction) {
        if (key == sf::Keyboard::W) mPlayer.setMovingUp(isPressed);
        if (key == sf::Keyboard::S) mPlayer.setMovingDown(isPressed);
        if (key == sf::Keyboard::A) mPlayer.setMovingLeft(isPressed);
        if (key == sf::Keyboard::D) mPlayer.setMovingRight(isPressed);
    }
}

void GameLogic::update(sf::Time dt)
{
    if (mPaused || mInInteraction) return;
    // 1) Рассчитать желаемое смещение игрока
    sf::Vector2f desired = mPlayer.computeMovement(dt);

    // 2) Коллизии: проверяем четыре угла «коробки» игрока
    sf::FloatRect box = mPlayer.getBounds();
    sf::FloatRect next = box;
    next.left += desired.x;
    next.top += desired.y;

    auto tsz = mMap.getTileSize();
    auto canWalk = [&](float px, float py) {
        int tx = int(px) / tsz.x;
        int ty = int(py) / tsz.y;
        return mMap.isWalkable(tx, ty);
        };

    bool ok = canWalk(next.left, next.top)
        && canWalk(next.left + next.width - 1, next.top)
        && canWalk(next.left, next.top + next.height - 1)
        && canWalk(next.left + next.width - 1, next.top + next.height - 1);

    // 3) Коллизии с машинами
    if (ok) {
        for (auto& inc : mIncidents) {
            if (!inc->isResolved()) {
                auto pv = dynamic_cast<ParkingViolation*>(inc.get());
                if (pv && pv->getCarBounds().intersects(next)) {
                    ok = false;
                    break;
                }
            }
        }
    }

    if (ok) {
        mPlayer.move(desired);
    }

    mPlayer.update(dt);

    // Обновляем инциденты
    sf::Vector2f pp = getPlayerPosition();
    for (auto& inc : mIncidents)
        inc->update(pp, dt);

    // Удаляем решённые
    mIncidents.erase(
        std::remove_if(mIncidents.begin(), mIncidents.end(),
            [](const std::unique_ptr<Incident>& i) { return i->isResolved(); }), mIncidents.end());

    // SPAWN LOGIC
    mTimeSinceLast += dt.asSeconds();
    if (mTimeSinceLast >= mSpawnInterval) {
        spawnOne();
        mTimeSinceLast = 0.f;
    }

}

void GameLogic::renderScene(sf::RenderWindow& window)
{
    // Сначала карта и всё, что на ней
    window.draw(mMap);

    // Инциденты
    for (auto& inc : mIncidents)
        inc->render(window);

    // Игрок
    mPlayer.render(window);
}

void GameLogic::startInteraction()
{
    mInInteraction = true;
    // можно ещё остановить логику движения, если нужно
}

void GameLogic::finishInteraction(bool ok)
{
    if (ok && mCurrentViolation) {
        mCurrentViolation->resolve();
        mScore += 10;
    }
    mCurrentViolation = nullptr;
    mInInteraction = false;
}

bool GameLogic::spawnOne()
{
    // Если уже достигли бюджета — выходим
    if (mIncidents.size() >= mMaxIncidents)
        return false;

    // Берём случайную зону
    const auto& zones = mMap.getSpawnZones();
    if (zones.empty()) return false;

    const int MAX_TRIES = 20;
    for (int attempt = 0; attempt < MAX_TRIES; ++attempt)
    {
        // 1) случайная зона
        const auto& sz = zones[std::rand() % zones.size()];

        // 2) Выбираем текстуру машины
        auto& dict = (sz.orientation == "front")
            ? mCarFrontTextures
            : mCarSideTextures;
        std::string colorKey = pickColor(sz.color, dict);
        const sf::Texture& carTex = dict.at(colorKey);

        // 3) Создаем временный спрайт, чтобы узнать его размер
        sf::Sprite tempCar(carTex);
        float scale = 1.5f;  // или тот же const 1.5f
        tempCar.setScale(scale, scale);
        tempCar.setPosition(0.f, 0.f);
        // Получаем размеры с учётом масштаба
        sf::FloatRect carBounds = tempCar.getGlobalBounds();
        float spriteW = carBounds.width;
        float spriteH = carBounds.height;

        // 4) Вычисляем допустимый диапазон точек спавна
        float minX = sz.rect.left;
        float minY = sz.rect.top;
        float maxX = sz.rect.left + sz.rect.width - spriteW;
        float maxY = sz.rect.top + sz.rect.height - spriteH;
        if (maxX < minX || maxY < minY)
            continue;  // зона слишком мала — пропускаем

        // 5) Случайная точка в этом диапазоне
        float rx = minX + (std::rand() / float(RAND_MAX)) * (maxX - minX);
        float ry = minY + (std::rand() / float(RAND_MAX)) * (maxY - minY);

        // 6) Проверяем пересечение с уже существующими машинами
        tempCar.setPosition(rx, ry);
        auto newBounds = tempCar.getGlobalBounds();

        bool overlap = false;
        for (auto& inc : mIncidents) {
            if (!inc->isResolved()) {
                auto pv = dynamic_cast<ParkingViolation*>(inc.get());
                if (pv && pv->getCarBounds().intersects(newBounds)) {
                    overlap = true;
                    break;
                }
            }
        }
        if (overlap) continue;

        // 7) Всё чисто — спавним настоящую машину

        // параметры вопросика-спрайтшита:
        unsigned        frameCount = 15;
        sf::Vector2u    frameSize{ 6,23 };
        float           frameDuration = 0.1f;
        mIncidents.emplace_back(std::make_unique<ParkingViolation>(
            sf::Vector2f(rx, ry),
            carTex,
            mQuestionSheetTexture,
            frameCount,
            frameSize,
            frameDuration
        ));
        return true;
    }

    return false;
}

std::string GameLogic::pickColor(const std::string& desired, const std::unordered_map<std::string, sf::Texture>& dict)
{
    // Если дизайнер задал конкретный цвет и он есть в словаре — вернём его
    auto it = dict.find(desired);
    if (it != dict.end())
        return desired;

    // Иначе — выберем случайный из доступных ключей
    std::vector<std::string> keys;
    keys.reserve(dict.size());
    for (auto& p : dict)
        keys.push_back(p.first);

    return keys[std::rand() % keys.size()];
}
