#include "GameLogic.h"
#include <stdexcept>
#include "ParkingViolation.h"
#include "Player.h"
#include <algorithm>
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
    mCarTexture.loadFromFile("assets/images/car_1.png");
    mQuestionSheetTexture.loadFromFile("assets/images/mark.png");
    // спавним одну нарушенную парковку
    sf::Vector2f pos(600.f, 300.f);
    mIncidents.emplace_back(std::make_unique<ParkingViolation>(
        pos, mCarTexture, mQuestionSheetTexture, 15, sf::Vector2u(6, 23), 0.1f));

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
