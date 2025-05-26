#include "GameRenderer.h"
#include <iomanip>
#include <sstream>
#include <stdexcept>

GameRenderer::GameRenderer(sf::RenderWindow& window, GameLogic& logic)
    : mWindow(window)
    , mLogic(logic)
{

    // ��������� ����� ��� HUD
    if (!mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf"))
        throw std::runtime_error("Failed to load font for HUD");

    // ����������� ��� ��������� ����
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
    // ���������� �������������� ��� �� ���� �����
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
    // 1) ��������� �����
    // �����
    mWindow.draw(mLogic.getMap());

    // ���������
    for (auto& inc : mLogic.getIncidents())
        inc->render(mWindow);

    // �����
    mLogic.getPlayer().render(mWindow);

    // 2) ��������� HUD
    // 2.1 ����
    mScoreText.setString("Score: " + std::to_string(mLogic.getScore()));
    mScoreText.setPosition(10.f, 10.f);
    mWindow.draw(mScoreText);

    // 2.2 ����� (mm:ss)
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

    // 2.3 �������� ���������
    mActiveText.setString("Active: " + std::to_string(mLogic.getActiveIncidents()));
    mActiveText.setPosition(10.f, 70.f);
    mWindow.draw(mActiveText);

    if (mLogic.isInteracting()) {
        mWindow.draw(mInteractionOverlay);

        // ���������: ��� �� QTE? (��������, ����� GameLogic::isQTEActive())
        if (mLogic.isQTEActive()) {
            // �������� ������������������ � ������ �� GameLogic
            auto seq = mLogic.getQTESequence();
            size_t index = mLogic.getQTECurrentIndex();

            // �������� ������, ��� ������� ������ �������� [ ]
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

            // ���������� �������
            display += "\nAttempt: " + std::to_string(mLogic.getQTEAttempt() + 1) + " / 7";

            mQTEText.setString(display);
            // ���������� �� ������
            auto gb = mQTEText.getGlobalBounds();
            mQTEText.setPosition(
                mWindow.getSize().x / 2.f - gb.width / 2.f,
                mWindow.getSize().y / 2.f - gb.height / 2.f
            );
            mWindow.draw(mQTEText);
        }
        else {
            // fallback, ���� QTE �� �������
            mWindow.draw(mInteractionText);
        }
    }


}
