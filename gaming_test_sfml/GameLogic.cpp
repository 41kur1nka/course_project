#include "GameLogic.h"
#include <stdexcept>
#include "ParkingViolation.h"
#include "Player.h"
GameLogic::GameLogic()
{
    // 1) ��������� ����� �� JSON + ��������
    if (!mMap.loadFromJSON("assets/images/map.json", "assets/images/tilemap_packed.png"))
        throw std::runtime_error("Map load failed in GameLogic");

    // 2) ��������� ������ � ��������� �������
    //    ������: � ������ ������ ��� �� �������� �����������
    sf::Vector2u mapPx = { mMap.getMapSize().x * mMap.getTileSize().x,
                           mMap.getMapSize().y * mMap.getTileSize().y };
    mPlayer.setPosition({ float(mapPx.x) / 2.f, float(mapPx.y) / 2.f });

    // 3) (�����������) ������ ��������� �����-�������� � �. �.


}

void GameLogic::handleInput(sf::Keyboard::Key key, bool isPressed)
{
    if (key == sf::Keyboard::Escape && isPressed) {
        togglePause();
        return;  // ������ �� ����������� � ������
    }
    if (mIsPaused)
        return;


    mPlayer.handleInput(key, isPressed);
    if (key == sf::Keyboard::I && isPressed)
        spawnIncident();
}

void GameLogic::update(sf::Time dt)
{
    // 1) ���������� �������� �������� ������
    sf::Vector2f desired = mPlayer.computeMovement(dt);

    // 2) ��������: ��������� ������ ���� �������� ������
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

    // 3) �������� �������� ������
    mPlayer.update(dt);

    // 4) �������� ��������� (�������, ����-���� � �.�.)
    mIncidents.update(dt);
}

void GameLogic::spawnIncident()
{
    // ���������� ��������� ���������� ������ �����
    auto ts = mMap.getTileSize();
    auto ms = mMap.getMapSize();
    float x = float(std::rand() % (ms.x * ts.x));
    float y = float(std::rand() % (ms.y * ts.y));

    mIncidents.addIncident(
        std::make_unique<ParkingViolation>(sf::Vector2f{ x,y })
    );
}

void GameLogic::togglePause()
{
    mIsPaused = !mIsPaused;
}

bool GameLogic::isPaused() const
{
    return mIsPaused;
}

void GameLogic::renderScene(sf::RenderWindow& window)
{
    // ��������� �������� ��������� �����
    window.draw(mMap);                  // �����
    mIncidents.render(window);         // ���������
    mPlayer.render(window);            // �����
}
