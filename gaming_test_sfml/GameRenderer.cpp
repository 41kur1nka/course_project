#include "GameRenderer.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{

    // Загружаем шрифт для HUD
    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf"))
        throw std::runtime_error("Failed to load font for HUD");

    // Настраиваем три текстовых поля
    auto initText = [&](sf::Text& t, unsigned size) {
        t.setFont(mFont);
        t.setCharacterSize(size);
        t.setFillColor(sf::Color::White);
        };
    initText(mScoreText, 20);
    initText(mTimeText, 20);
    initText(mActiveText, 20);
    initText(mQTEText, 20);


    // === Interaction Screen setup ===
    // Прозрачный полупрозрачный фон на весь экран
    mInteractionOverlay.setSize(
        sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    mInteractionOverlay.setFillColor(sf::Color(0, 0, 0, 180));

    mQTEText.setFont(mFont);
    mQTEText.setCharacterSize(36);
    mQTEText.setFillColor(sf::Color::White);
    mQTEText.setOutlineColor(sf::Color::Black);
    mQTEText.setOutlineThickness(2.f);
}

void GameRenderer::render()
{
    // 1) Отрисовка сцены
    // Карта
    mWindow.draw(mLogic.getMap());

    // Инциденты
    for (auto& inc : mLogic.getIncidents())
        inc->render(mWindow);

    // Игрок
    mLogic.getPlayer().render(mWindow);

    // 2) Отрисовка HUD
    // 2.1 Счёт
    mScoreText.setString("Score: " + std::to_string(mLogic.getScore()));
    mScoreText.setPosition(10.f, 10.f);
    mWindow.draw(mScoreText);

    // 2.2 Время (mm:ss)
    {
        int total = static_cast<int>(mLogic.getElapsedSeconds());
        int mm = total / 60;
        int ss = total % 60;
        std::ostringstream oss;
        oss << "Time: "
            << std::setw(2) << std::setfill('0') << mm
            << ":"
            << std::setw(2) << std::setfill('0') << ss;
        mTimeText.setString(oss.str());
    }
    mTimeText.setPosition(10.f, 40.f);
    mWindow.draw(mTimeText);

    // 2.3 Активные инциденты
    mActiveText.setString("Active: " + std::to_string(mLogic.getActiveIncidents()));
    mActiveText.setPosition(10.f, 70.f);
    mWindow.draw(mActiveText);

    if (mLogic.isInteracting()) {
        mWindow.draw(mInteractionOverlay);

        if (mLogic.isQTEActive()) {
            if (mLogic.getQTE().isFightQTE) {
                // Draw the fight QTE circle
                float radius = 60.f;  // Increased radius

                // Draw main circle
                sf::CircleShape outerCircle(radius);
                outerCircle.setFillColor(sf::Color(20, 20, 40, 200));
                outerCircle.setOutlineColor(sf::Color(150, 150, 255));
                outerCircle.setOutlineThickness(2.f);
                outerCircle.setPosition(
                    mWindow.getSize().x / 2.f - radius,
                    mWindow.getSize().y / 2.f - radius
                );
                mWindow.draw(outerCircle);

                // Draw perfect zone arc
                const int arcSegmentCount = 30;  // More segments for smoother arc
                const float arcThickness = 12.f;  // Thickness of the arc

                float perfectStart = mLogic.getQTE().perfectZonePosition - mLogic.getQTE().perfectTimeWindow;
                float perfectEnd = mLogic.getQTE().perfectZonePosition + mLogic.getQTE().perfectTimeWindow;

                // Draw the arc using consecutive quads
                for (int i = 0; i < arcSegmentCount; i++) {
                    float t = perfectStart + (perfectEnd - perfectStart) * (i / float(arcSegmentCount - 1));
                    float nextT = perfectStart + (perfectEnd - perfectStart) * ((i + 1) / float(arcSegmentCount - 1));

                    float angle1 = (t * 360.f - 90.f) * 3.14159f / 180.f;
                    float angle2 = (nextT * 360.f - 90.f) * 3.14159f / 180.f;

                    float innerRadius = radius - arcThickness / 2;
                    float outerRadius = radius + arcThickness / 2;

                    // Calculate the four corners of the quad
                    float x1Inner = innerRadius * std::cos(angle1);
                    float y1Inner = innerRadius * std::sin(angle1);
                    float x2Inner = innerRadius * std::cos(angle2);
                    float y2Inner = innerRadius * std::sin(angle2);

                    float x1Outer = outerRadius * std::cos(angle1);
                    float y1Outer = outerRadius * std::sin(angle1);
                    float x2Outer = outerRadius * std::cos(angle2);
                    float y2Outer = outerRadius * std::sin(angle2);

                    // Create a quad
                    sf::VertexArray quad(sf::Quads, 4);

                    // Define the quad vertices
                    quad[0].position = sf::Vector2f(mWindow.getSize().x / 2.f + x1Inner, mWindow.getSize().y / 2.f + y1Inner);
                    quad[1].position = sf::Vector2f(mWindow.getSize().x / 2.f + x1Outer, mWindow.getSize().y / 2.f + y1Outer);
                    quad[2].position = sf::Vector2f(mWindow.getSize().x / 2.f + x2Outer, mWindow.getSize().y / 2.f + y2Outer);
                    quad[3].position = sf::Vector2f(mWindow.getSize().x / 2.f + x2Inner, mWindow.getSize().y / 2.f + y2Inner);

                    // Set solid color for all vertices
                    sf::Color zoneColor(50, 255, 50, 200); // Solid green with fixed alpha
                    quad[0].color = zoneColor;
                    quad[1].color = zoneColor;
                    quad[2].color = zoneColor;
                    quad[3].color = zoneColor;

                    mWindow.draw(quad);
                }

                // Draw the perfect timing marker (line pointing to center)
                float perfectAngle = (mLogic.getQTE().perfectZonePosition * 360.f - 90.f) * 3.14159f / 180.f;
                float markerLength = 15.f;  // Length of the marker line

                sf::RectangleShape marker(sf::Vector2f(markerLength, 2.f));  // 2 pixels thick
                marker.setFillColor(sf::Color::White);

                // Calculate the position and rotation for the marker
                float markerX = radius * std::cos(perfectAngle);
                float markerY = radius * std::sin(perfectAngle);

                marker.setPosition(
                    mWindow.getSize().x / 2.f + markerX,
                    mWindow.getSize().y / 2.f + markerY
                );

                // Set rotation to point towards the center
                float rotation = (mLogic.getQTE().perfectZonePosition * 360.f) + 180.f;  // Point inward
                marker.setRotation(rotation);

                // Set the origin to the outer end of the line
                marker.setOrigin(0.f, 1.f);

                mWindow.draw(marker);

                // Draw timing indicator with trail effect
                float progress = mLogic.getQTE().circleTimer / mLogic.getQTE().circleMaxTime;
                float angle = progress * 360.f;

                // Draw the red dot
                float indicatorAngle = (angle - 90.f) * 3.14159f / 180.f;
                float indicatorX = radius * std::cos(indicatorAngle);
                float indicatorY = radius * std::sin(indicatorAngle);

                sf::CircleShape indicator(4.f);
                indicator.setFillColor(sf::Color::Red);
                indicator.setPosition(
                    mWindow.getSize().x / 2.f + indicatorX - 4.f,
                    mWindow.getSize().y / 2.f + indicatorY - 4.f
                );
                mWindow.draw(indicator);

                // Draw the expected key with style
                std::string keyStr;
                switch (mLogic.getQTE().expectedKey) {
                case sf::Keyboard::W: keyStr = "W"; break;
                case sf::Keyboard::A: keyStr = "A"; break;
                case sf::Keyboard::S: keyStr = "S"; break;
                case sf::Keyboard::D: keyStr = "D"; break;
                default: keyStr = "?"; break;
                }

                // Draw key background
                sf::CircleShape keyBackground(25.f);
                keyBackground.setFillColor(sf::Color(40, 40, 60, 230));
                keyBackground.setOutlineColor(sf::Color(150, 150, 255));
                keyBackground.setOutlineThickness(2.f);
                keyBackground.setPosition(
                    mWindow.getSize().x / 2.f - 25.f,
                    mWindow.getSize().y / 2.f - 25.f
                );
                mWindow.draw(keyBackground);

                // Draw key text
                sf::Text keyText;
                keyText.setFont(mFont);
                keyText.setString(keyStr);
                keyText.setCharacterSize(35);
                keyText.setFillColor(sf::Color::White);
                keyText.setOutlineColor(sf::Color(100, 100, 255));
                keyText.setOutlineThickness(1.f);
                auto bounds = keyText.getLocalBounds();
                keyText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                keyText.setPosition(
                    mWindow.getSize().x / 2.f,
                    mWindow.getSize().y / 2.f - 5.f
                );
                mWindow.draw(keyText);

                // Draw attempt counter with style
                std::string attemptStr = "Attempt " + std::to_string(mLogic.getQTEAttempt() + 1) + " of 3";
                sf::Text attemptText;
                attemptText.setFont(mFont);
                attemptText.setString(attemptStr);
                attemptText.setCharacterSize(24);
                attemptText.setFillColor(sf::Color(200, 200, 255));
                attemptText.setOutlineColor(sf::Color::Black);
                attemptText.setOutlineThickness(2.f);
                bounds = attemptText.getLocalBounds();
                attemptText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                attemptText.setPosition(
                    mWindow.getSize().x / 2.f,
                    mWindow.getSize().y / 2.f + radius + 35.f
                );
                mWindow.draw(attemptText);
            }
            else {
                // Regular QTE sequence display
                auto seq = mLogic.getQTESequence();
                size_t index = mLogic.getQTECurrentIndex();

                //  ,   ������ �������� [ ]
                std::string display;
                for (size_t i = 0; i < seq.size(); ++i) {
                    char ch = '?';
                    switch (seq[i]) {
                    case sf::Keyboard::W: ch = 'W'; break;
                    case sf::Keyboard::A: ch = 'A'; break;
                    case sf::Keyboard::S: ch = 'S'; break;
                    case sf::Keyboard::D: ch = 'D'; break;
                    case sf::Keyboard::E: ch = 'E'; break;
                    case sf::Keyboard::Q: ch = 'Q'; break;
                    }
                    if (i == index)
                        display += std::string("[") + ch + "] ";
                    else
                        display += std::string(" ") + ch + "  ";
                }

                //  
                display += "\nAttempt: " + std::to_string(mLogic.getQTEAttempt() + 1) + " / 3";

                mQTEText.setString(display);
                //   
                auto gb = mQTEText.getGlobalBounds();
                mQTEText.setPosition(
                    mWindow.getSize().x / 2.f - gb.width / 2.f,
                    mWindow.getSize().y / 2.f - gb.height / 2.f
                );
                mWindow.draw(mQTEText);
            }
        }
        else {
            // fallback,  QTE  
            mWindow.draw(mInteractionText);
        }
    }


}
