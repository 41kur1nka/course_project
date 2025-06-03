#pragma once
#include <string>
#include <functional>

struct PlayerSettings {
    std::string name = "Player";
    int skinIndex = 0;

    // API constants
    static const std::string API_KEY;
    static const std::string BIN_ID;

    // Local
    void saveLocal(const std::string& path = "settings.txt") const;
    void loadLocal(const std::string& path = "settings.txt");

    // Online
    void saveOnline(const std::string& apiKey, const std::string& binId, 
                   std::function<void(bool)> callback = nullptr) const;
    void loadOnline(const std::string& apiKey, const std::string& binId,
                   std::function<void(bool)> callback = nullptr);

    // Common function for saving
    void save();
    void load();
};
