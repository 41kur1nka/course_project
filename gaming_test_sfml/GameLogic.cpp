#include "GameLogic.h"
#include <stdexcept>
#include <cstdlib> 
#include "ParkingViolation.h"
#include "FightIncident.h"
#include "GraffitiIncident.h"
#include "Player.h"
#include <algorithm>
#include <iostream>


GameLogic::GameLogic()
{
    // 1) Load map form JSON + tileset
    if (!mMap.loadFromJSON("assets/images/map.json"))
        throw std::runtime_error("Map load failed in GameLogic");

    // 2) Place player in start position
    sf::Vector2u mapPx = { mMap.getMapSize().x * mMap.getTileSize().x,
                           mMap.getMapSize().y * mMap.getTileSize().y };
    mPlayer.setPosition({ float(mapPx.x) / 2.f, float(mapPx.y) / 2.f });

    mQuestionSheetTexture.loadFromFile("assets/images/mark.png");
    
    mFightSheetTexture.loadFromFile("assets/images/fight/fight.png");

    mGraffitiLeftTexture.loadFromFile("assets/images/graffiti/graffiti_left.png");
    mGraffitiRightTexture.loadFromFile("assets/images/graffiti/graffiti_right.png");


    for (auto color : { "red","blue","green","white","yellow"}) {
        sf::Texture side;  side.loadFromFile("assets/images/cars/car_side_" + std::string(color) + ".png");
        sf::Texture front; front.loadFromFile("assets/images/cars/car_front_" + std::string(color) + ".png");
        mCarSideTextures[color] = std::move(side);
        mCarFrontTextures[color] = std::move(front);
    }

    const int initialCount = 0;
    for (int i = 0; i < initialCount; ++i) {
        spawnOne();
    }
}

void GameLogic::handleInput(sf::Keyboard::Key key, bool isPressed)
{
    // Pause
    if (key == sf::Keyboard::Escape && isPressed) {
        togglePause();
        return;
    }

    // Escape by pressing E if in pause
    if (mInInteraction && mQTE.active) {
        if (!isPressed) return;

        if (mQTE.isFightQTE) {
            // For fight QTE, check if the pressed key matches the expected key
            // and if it's within the perfect timing window
            if (key == mQTE.expectedKey) {
                float normalizedTime = mQTE.circleTimer / mQTE.circleMaxTime;
                float timeDiff = std::abs(normalizedTime - mQTE.perfectZonePosition);

                if (timeDiff <= mQTE.perfectTimeWindow) {
                    // Perfect timing
                    if (mCurrentViolation) mCurrentViolation->resolve();
                    mScore += mQTE.scorePerAttempt[mQTE.attempt];
                    mInInteraction = false;
                    mQTE.active = false;
                    mCurrentViolation = nullptr;
                }
                else {
                    // Bad timing
                    mQTE.attempt++;
                    if (mQTE.attempt >= 3) {
                        if (mCurrentViolation) mCurrentViolation->resolve();
                        mInInteraction = false;
                        mQTE.active = false;
                        mCurrentViolation = nullptr;
                    }
                    else {
                        startQTE(false);
                    }
                }
            }
            else {
                // Wrong key
                mQTE.attempt++;
                if (mQTE.attempt >= 3) {
                    if (mCurrentViolation) mCurrentViolation->resolve();
                    mInInteraction = false;
                    mQTE.active = false;
                    mCurrentViolation = nullptr;
                }
                else {
                    startQTE(false);
                }
            }
            return;
        }

        // Regular QTE sequence logic
        if (key == mQTE.sequence[mQTE.currentIndex]) {
            mQTE.currentIndex++;
            if (mQTE.currentIndex >= mQTE.sequence.size()) {
                // Success!
                if (mCurrentViolation) mCurrentViolation->resolve();
                mScore += mQTE.scorePerAttempt[mQTE.attempt];
                mInInteraction = false;
                mQTE.active = false;
                mCurrentViolation = nullptr;
            }
            return;
        }

        // Only if pressed any key from allowing keys
        static std::vector<sf::Keyboard::Key> allowed = {
            sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::S,
            sf::Keyboard::D, sf::Keyboard::E, sf::Keyboard::Q
        };
        if (isPressed
            && std::find(allowed.begin(), allowed.end(), key) != allowed.end()
            && key != mQTE.sequence[mQTE.currentIndex])
        {
            mQTE.attempt++;
            if (mQTE.attempt >= 3) {
                if (mCurrentViolation) mCurrentViolation->resolve();
                mInInteraction = false;
                mQTE.active = false;
                mCurrentViolation = nullptr;
            }
            else {
                // Starting new sequence, Начинаем новую последовательность, not reset attempt!
                startQTE(false);
            }
            return;
        }
    }


    // If E pressed close to car
    if (key == sf::Keyboard::E && isPressed && !mPaused) {
        for (auto& inc : mIncidents) {
            if (auto pv = dynamic_cast<ParkingViolation*>(inc.get())) {
                if (pv->isPlayerInRange()) {
                    mCurrentViolation = pv;
                    startInteraction();
                    return;
                }
            }
            if (auto fi = dynamic_cast<FightIncident*>(inc.get())) {
                if (fi->isPlayerInRange()) {
                    mCurrentViolation = fi;
                    startInteraction();
                    return;
                }
            }
            if (auto gi = dynamic_cast<GraffitiIncident*>(inc.get())) {
                if (gi->isPlayerInRange()) {
                    mCurrentViolation = gi;
                    startInteraction();
                    return;
                }
            }
        }
    }

    // WASD – only if not in pause and not in iteration
    if (!mPaused && !mInInteraction) {
        if (key == sf::Keyboard::W) mPlayer.setMovingUp(isPressed);
        if (key == sf::Keyboard::S) mPlayer.setMovingDown(isPressed);
        if (key == sf::Keyboard::A) mPlayer.setMovingLeft(isPressed);
        if (key == sf::Keyboard::D) mPlayer.setMovingRight(isPressed);
    }
}

void GameLogic::update(sf::Time dt)
{
    if (mPaused) return;

    // Update QTE circle timer if active
    if (mInInteraction && mQTE.active && mQTE.isFightQTE) {
        mQTE.circleTimer += dt.asSeconds();
        if (mQTE.circleTimer >= mQTE.circleMaxTime) {
            // Time's up - count as a failed attempt
            mQTE.attempt++;
            if (mQTE.attempt >= 3) {
                if (mCurrentViolation) mCurrentViolation->resolve();
                mInInteraction = false;
                mQTE.active = false;
                mCurrentViolation = nullptr;
            }
            else {
                startQTE(false);
            }
        }
    }

    mElapsedTime += dt.asSeconds();

    if (mPaused || mInInteraction) return;

    sf::Vector2f desired = mPlayer.computeMovement(dt);

    // 2) Collisions
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

    // 3) Collisions with cars
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

    // Update incidents
    sf::Vector2f pp = getPlayerPosition();
    for (auto& inc : mIncidents)
        inc->update(pp, dt, mPlayer.getBounds());

    // Delete completed
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
    // Map
    window.draw(mMap);

    // Incidents
    for (auto& inc : mIncidents)
        inc->render(window);

    // Player
    mPlayer.render(window);
}

void GameLogic::startInteraction()
{
    mInInteraction = true;
    startQTE(true);
}

void GameLogic::finishInteraction(bool ok)
{
    if (ok && mCurrentViolation) {
        mCurrentViolation->resolve();
        mScore += 10;
    }
    mCurrentViolation = nullptr;
    mInInteraction = false;

    //reset moving flags
    mPlayer.setMovingUp(false);
    mPlayer.setMovingDown(false);
    mPlayer.setMovingLeft(false);
    mPlayer.setMovingRight(false);
}

bool GameLogic::spawnOne()
{
    // If maxIncidents then continue
    if (mIncidents.size() >= mMaxIncidents)
        return false;

    // Taking random zone
    const auto& zones = mMap.getSpawnZones();
    if (zones.empty()) return false;

    const int MAX_TRIES = 20;
    for (int attempt = 0; attempt < MAX_TRIES; ++attempt)
    {
        // random zone
        const auto& sz = zones[std::rand() % zones.size()];

        if (sz.incidentType == "car") {
            // choosing car texture
            auto& dict = (sz.orientation == "front")
                ? mCarFrontTextures
                : mCarSideTextures;
            std::string colorKey = pickColor(sz.color, dict);
            const sf::Texture& carTex = dict.at(colorKey);

            // create temporary sprite, to find out its size
            sf::Sprite tempCar(carTex);
            float scale = 1.5f;
            tempCar.setScale(scale, scale);
            tempCar.setPosition(0.f, 0.f);
            // Gettins size taking into account the scale
            sf::FloatRect carBounds = tempCar.getGlobalBounds();
            float spriteW = carBounds.width;
            float spriteH = carBounds.height;

            // Calculate spawn range
            float minX = sz.rect.left;
            float minY = sz.rect.top;
            float maxX = sz.rect.left + sz.rect.width - spriteW;
            float maxY = sz.rect.top + sz.rect.height - spriteH;
            if (maxX < minX || maxY < minY)
                continue;  // zone too small

            // 5) Random dot in range
            float rx = minX + (std::rand() / float(RAND_MAX)) * (maxX - minX);
            float ry = minY + (std::rand() / float(RAND_MAX)) * (maxY - minY);

            // 6) Checking crossing with exist cars
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

            // settings for sign spritesheet
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
        else if (sz.incidentType == "fight") {
            unsigned frameCount = 6;
            sf::Vector2u frameSize{ 23, 15 };
            float scale = 1.5f;

            sf::Sprite tempFight(mFightSheetTexture);
            tempFight.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
            tempFight.setScale(scale, scale);
            tempFight.setPosition(0.f, 0.f);

            sf::FloatRect fightBounds = tempFight.getGlobalBounds();
            float spriteW = fightBounds.width;
            float spriteH = fightBounds.height;

            float minX = sz.rect.left;
            float minY = sz.rect.top;
            float maxX = sz.rect.left + sz.rect.width - spriteW;
            float maxY = sz.rect.top + sz.rect.height - spriteH;
            if (maxX < minX || maxY < minY)
                continue;

            float rx = minX + (std::rand() / float(RAND_MAX)) * (maxX - minX);
            float ry = minY + (std::rand() / float(RAND_MAX)) * (maxY - minY);

            tempFight.setPosition(rx, ry);
            auto newBounds = tempFight.getGlobalBounds();

            bool overlap = false;
            for (auto& inc : mIncidents) {
                if (!inc->isResolved()) {
                    if (auto pv = dynamic_cast<ParkingViolation*>(inc.get())) {
                        if (pv->getCarBounds().intersects(newBounds)) { overlap = true; break; }
                    }
                    if (auto fi = dynamic_cast<FightIncident*>(inc.get())) {
                        if (fi->getFightBounds().intersects(newBounds)) { overlap = true; break; }
                    }
                }
            }
            if (overlap) continue;

            float frameDuration = 0.1f;
            mIncidents.emplace_back(std::make_unique<FightIncident>(
                sf::Vector2f(rx, ry),
                mFightSheetTexture,
                mQuestionSheetTexture,
                frameCount,
                frameSize,
                frameDuration
            ));
            return true;
        }
        else if (sz.incidentType == "graffiti") {
            // Choosing needed spritesheet at orientation
            const sf::Texture* graffitiTex = &mGraffitiRightTexture; // right - default
            if (sz.orientation == "left") {
                graffitiTex = &mGraffitiLeftTexture;
            }

            unsigned frameCount = 18;
            sf::Vector2u frameSize{ 20, 16 };
            float frameDuration = 0.1f;
            float scale = 1.5f;

            sf::Sprite tempGraffiti(*graffitiTex);
            tempGraffiti.setTextureRect(sf::IntRect(0, 0, frameSize.x, frameSize.y));
            tempGraffiti.setScale(scale, scale);
            tempGraffiti.setPosition(0.f, 0.f);

            sf::FloatRect graffitiBounds = tempGraffiti.getGlobalBounds();
            float spriteW = graffitiBounds.width;
            float spriteH = graffitiBounds.height;

            float rx = sz.rect.left + sz.rect.width / 2.f - spriteW / 2.f;
            float ry = sz.rect.top + sz.rect.height / 2.f - spriteH / 2.f;

            tempGraffiti.setPosition(rx, ry);
            auto newBounds = tempGraffiti.getGlobalBounds();

            bool overlap = false;
            for (auto& inc : mIncidents) {
                if (!inc->isResolved()) {
                    if (auto gi = dynamic_cast<GraffitiIncident*>(inc.get())) {
                        if (gi->getGraffitiBounds().intersects(newBounds)) { overlap = true; break; }
                    }
                }
            }
            if (overlap) continue;

            mIncidents.emplace_back(std::make_unique<GraffitiIncident>(
                sf::Vector2f(rx, ry),
                *graffitiTex,
                mQuestionSheetTexture,
                frameCount,
                frameSize,
                frameDuration
            ));
            return true;
            }


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


void GameLogic::startQTE(bool resetAttempt) {
    // Reset QTE status
    mQTE.sequence.clear();
    mQTE.currentIndex = 0;
    if (resetAttempt) mQTE.attempt = 0;
    mQTE.active = true;


    // Check if this is a fight incident
    if (auto* fightIncident = dynamic_cast<FightIncident*>(mCurrentViolation)) {
        mQTE.isFightQTE = true;
        mQTE.circleTimer = 0.f;

        // Set random position for perfect zone (avoiding edges of circle)
        mQTE.perfectZonePosition = 0.2f + (static_cast<float>(rand()) / RAND_MAX) * 0.6f;

        // Choose a random key for the fight QTE
        static std::vector<sf::Keyboard::Key> possible = {
            sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D
        };
        mQTE.expectedKey = possible[rand() % possible.size()];
        return;
    }

    mQTE.isFightQTE = false;

    static std::vector<sf::Keyboard::Key> possible = {
        sf::Keyboard::W, sf::Keyboard::A, sf::Keyboard::S, sf::Keyboard::D, sf::Keyboard::E, sf::Keyboard::Q
    };
    int QTE_LENGTH = 7;
    for (int i = 0; i < QTE_LENGTH; ++i) {
        mQTE.sequence.push_back(possible[rand() % possible.size()]);
    }
}
