#include "SettingsScreen.h"
#include "../Game.h"
#include <iostream>

SettingsScreen::SettingsScreen(GameState& stateRef, PlayerSettings& settings)
    : mState(stateRef), mSettings(settings) {
}

void SettingsScreen::loadSkinTextures() {
    mSkinTextures.resize(mSkinNames.size());
    
    // Loading textures for every skin
    std::vector<std::string> skinPaths = {
        "assets/skins/player_classic.png",
        "assets/skins/player_blue.png",
        "assets/skins/player_red.png"
    };

    for (size_t i = 0; i < skinPaths.size(); ++i) {
        if (!mSkinTextures[i].loadFromFile(skinPaths[i])) {
            throw std::runtime_error("Failed to load skin texture: " + skinPaths[i]);
        }
    }
    std::cout << "[DEBUG] mSkinTextures.size() = " << mSkinTextures.size() << std::endl;
    
}

void SettingsScreen::loadAssets(const sf::RenderWindow& window) {
    mFont.loadFromFile("assets/fonts/Montserrat-Black.ttf");

    loadSkinTextures();

    std::cout << "[DEBUG] mSkinTextures.size() = " << mSkinTextures.size() << std::endl;

    if (mSettings.skinIndex < 0 || mSettings.skinIndex >= mSkinTextures.size()) {
        std::cout << "BAD skinIndex in settings loadassets: " << mSettings.skinIndex << std::endl;
        mSettings.skinIndex = 0;
    }

    // Loading texture buttons "Save" and "Back to menu"
    if (!mTexN.loadFromFile("assets/ui/btn_gamemenu_norm.png") ||
        !mTexH.loadFromFile("assets/ui/btn_gamemenu_hover.png") ||
        !mTexP.loadFromFile("assets/ui/btn_gamemenu_down.png"))
    {
        throw std::runtime_error("Can't load save/back button textures (norm/hover/down)");
    }

    if (!mTeN.loadFromFile("assets/ui/btn_save_norm.png") ||
        !mTeH.loadFromFile("assets/ui/btn_save_hover.png") ||
        !mTeP.loadFromFile("assets/ui/btn_save_down.png"))
    {
        throw std::runtime_error("Can't load save/back button textures (norm/hover/down)");
    }

    // Button "<"
    if (!mTexPrev.loadFromFile("assets/ui/btn_prev_norm.png") ||
        !mTexPrevH.loadFromFile("assets/ui/btn_prev_hover.png") ||
        !mTexPrevP.loadFromFile("assets/ui/btn_prev_down.png"))
    {
        throw std::runtime_error("Can't load previous skin button textures");
    }

    // Button ">"
    if (!mTexNext.loadFromFile("assets/ui/btn_next_norm.png") ||
        !mTexNextH.loadFromFile("assets/ui/btn_next_hover.png") ||
        !mTexNextP.loadFromFile("assets/ui/btn_next_down.png"))
    {
        throw std::runtime_error("Can't load next skin button textures");
    }

    // Setting text elements
    mNameLabel.setFont(mFont);
    mNameLabel.setString("Your Name:");
    mNameLabel.setCharacterSize(28);
    mNameLabel.setFillColor(sf::Color::White);
    mNameLabel.setPosition(120, 120);

    mNameBox.setPosition(300, 120);
    mNameBox.setSize({ 300, 36 });
    mNameBox.setFillColor(sf::Color(40, 40, 40));
    mNameBox.setOutlineColor(sf::Color::White);
    mNameBox.setOutlineThickness(2.f);

    mEditLabel.setFont(mFont);
    mEditLabel.setCharacterSize(26);
    mEditLabel.setFillColor(sf::Color::White);
    mEditingName = mSettings.name;

    // Skins
    mSkinLabel.setFont(mFont);
    mSkinLabel.setPosition(120, 220);
    mSkinLabel.setCharacterSize(28);
    mSkinLabel.setFillColor(sf::Color::White);

    float buttonWidth = 200.f;
    float buttonHeight = 50.f;
    float buttonSpacing = 20.f;

    mSaveBtn = std::make_unique<Button>(
        mTeN, mTeH, mTeP,
        sf::Vector2f(window.getSize().x / 2.f - buttonWidth - buttonSpacing/2, 400),
        [this]() {
            try {
                mSettings.name = mEditingName;
                mSettings.saveLocal();
                mSettings.saveOnline(PlayerSettings::API_KEY, PlayerSettings::BIN_ID,
                    [this](bool success) {
                        if (!success) {
                            std::cerr << "Failed to save settings online, but local save succeeded" << std::endl;
                        }
                    });
                mState = GameState::MainMenu;
            }
            catch (const std::exception& e) {
                std::cerr << "Error saving settings: " << e.what() << std::endl;
            }
        }
    );

    mBackBtn = std::make_unique<Button>(
        mTexN, mTexH, mTexP,
        sf::Vector2f(window.getSize().x / 2.f + buttonSpacing/2, 400),
        [this]() { mState = GameState::MainMenu; }
    );

    // Select button for skins
    float skinButtonY = 220;
    mPrevSkinBtn = std::make_unique<Button>(
        mTexPrev, mTexPrevH, mTexPrevP,
        sf::Vector2f(320, skinButtonY),
        [this]() {
            mSettings.skinIndex = (mSettings.skinIndex + mSkinNames.size() - 1) % mSkinNames.size();
        }
    );

    mNextSkinBtn = std::make_unique<Button>(
        mTexNext, mTexNextH, mTexNextP,
        sf::Vector2f(520, skinButtonY),
        [this]() {
            mSettings.skinIndex = (mSettings.skinIndex + 1) % mSkinNames.size();
        }
    );
   
}

void SettingsScreen::update(const sf::RenderWindow& window, const sf::Event& event) {
    if (event.type == sf::Event::MouseButtonPressed) {
        auto pos = sf::Mouse::getPosition(window);
        if (mNameBox.getGlobalBounds().contains((sf::Vector2f)pos)) {
            mEditing = true;
            if (mEditingName.empty()) {
                mEditingName = mSettings.name;
            }
        }
        else mEditing = false;
    }

    if (mEditing && event.type == sf::Event::TextEntered) {
        if (event.text.unicode == '\b') { // backspace
            if (!mEditingName.empty()) mEditingName.pop_back();
        }
        else if (event.text.unicode == '\r' || event.text.unicode == '\n') { // enter
            mEditing = false;
        }
        else if (event.text.unicode < 128 && event.text.unicode >= 32) {
            if (mEditingName.size() < 16)
                mEditingName += static_cast<char>(event.text.unicode);
        }
    }

    // Updating buttons
    if (mSaveBtn) mSaveBtn->update(window);
    if (mBackBtn) mBackBtn->update(window);
    if (mPrevSkinBtn) mPrevSkinBtn->update(window);
    if (mNextSkinBtn) mNextSkinBtn->update(window);
}
void SettingsScreen::draw(sf::RenderTarget& target) {
    // Draw background
    sf::RectangleShape background(sf::Vector2f(target.getSize()));
    background.setFillColor(sf::Color(20, 20, 40, 255));
    target.draw(background);

    // Draw interface elements
    target.draw(mNameLabel);
    target.draw(mNameBox);
    
    mEditLabel.setString(mEditing ? mEditingName + "_" : mEditingName);
    mEditLabel.setPosition(310, 123);
    target.draw(mEditLabel);

    if (mSettings.skinIndex < 0 || mSettings.skinIndex >= mSkinTextures.size()) {
        std::cout << "BAD skinIndex in settings draw: " << mSettings.skinIndex << std::endl;
        mSettings.skinIndex = 0;
    }


    mSkinLabel.setString("Skin: " + mSkinNames[mSettings.skinIndex]);
    target.draw(mSkinLabel);

    sf::Sprite skinPreview(mSkinTextures[mSettings.skinIndex]);
    float previewSize = 100.f;
    float scale = previewSize / std::max(skinPreview.getTexture()->getSize().x, 
                                       skinPreview.getTexture()->getSize().y);
    skinPreview.setScale(scale, scale);
    
    sf::Vector2f previewPos(400, 220);
    sf::Vector2f textureSize(skinPreview.getTexture()->getSize());
    skinPreview.setPosition(
        previewPos.x + (previewSize - textureSize.x * scale) / 2.f,
        previewPos.y + (previewSize - textureSize.y * scale) / 2.f
    );
    
    sf::RectangleShape previewBorder(sf::Vector2f(previewSize, previewSize));
    previewBorder.setPosition(previewPos);
    previewBorder.setFillColor(sf::Color::Transparent);
    previewBorder.setOutlineColor(sf::Color::White);
    previewBorder.setOutlineThickness(2.f);
    
    target.draw(previewBorder);
    target.draw(skinPreview);

    if (mPrevSkinBtn) mPrevSkinBtn->draw(target);
    if (mNextSkinBtn) mNextSkinBtn->draw(target);
    if (mSaveBtn) mSaveBtn->draw(target);
    if (mBackBtn) mBackBtn->draw(target);
}
