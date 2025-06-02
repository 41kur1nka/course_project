#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include "../PlayerSettings.h"
#include "Button.h"


enum class GameState;

class SettingsScreen {
public:
    SettingsScreen(GameState& stateRef, PlayerSettings& settings);
    void loadAssets(const sf::RenderWindow& window);
    void update(const sf::RenderWindow& window, const sf::Event& event);
    void draw(sf::RenderTarget& target);

private:
    GameState& mState;
    PlayerSettings& mSettings;

    sf::Font mFont;
    sf::Text mNameLabel, mSkinLabel, mEditLabel;
    sf::RectangleShape mNameBox;
    std::string mEditingName;
    bool mEditing = false;

    std::vector<std::string> mSkinNames = { "Classic", "Skin1", "Skin2" };
    std::vector<sf::Texture> mSkinTextures;
    std::unique_ptr<Button> mPrevSkinBtn, mNextSkinBtn, mSaveBtn, mBackBtn;

    // Текстуры кнопок
    sf::Texture mTexN, mTexH, mTexP;
    sf::Texture mTeN, mTeH, mTeP;
    sf::Texture mTexPrev, mTexPrevH, mTexPrevP;
    sf::Texture mTexNext, mTexNextH, mTexNextP;

    void loadSkinTextures();
};
