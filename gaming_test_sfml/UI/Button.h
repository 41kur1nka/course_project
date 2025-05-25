#pragma once

#include <SFML/Graphics.hpp>
#include <functional>

class Button {
public:
    // texNormal, texHover, texPressed � ��� ��������� ������
    // position � �������-����� ���� ������
    // onClick    � ���������� ��� ���������� ��� ��� �������
    Button(const sf::Texture& texNormal,
        const sf::Texture& texHover,
        const sf::Texture& texPressed,
        const sf::Vector2f& position,
        std::function<void()> onClick);

    // �������� ��������� (hover, pressed) � ������� onClick, ���� ����
    void update(const sf::RenderWindow& window);

    // ���������� ������
    void draw(sf::RenderTarget& target) const;

private:
    enum class State { Normal, Hover, Pressed };
    State                   mState = State::Normal;
    sf::Sprite              mSprite;
    const sf::Texture& mTexN;
    const sf::Texture& mTexH;
    const sf::Texture& mTexP;
    std::function<void()>   mOnClick;
    bool                    mWasPressed = false;
};
