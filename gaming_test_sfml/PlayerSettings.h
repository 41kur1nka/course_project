#pragma once
#include <string>
#include <functional>

struct PlayerSettings {
    std::string name = "Player";
    int skinIndex = 0; // 0, 1, 2 � �� ���������� ������

    // API константы
    static const std::string API_KEY;
    static const std::string BIN_ID;

    // Локальное сохранение/загрузка
    void saveLocal(const std::string& path = "settings.txt") const;
    void loadLocal(const std::string& path = "settings.txt");

    // Онлайн сохранение/загрузка через jsonbin.io
    void saveOnline(const std::string& apiKey, const std::string& binId, 
                   std::function<void(bool)> callback = nullptr) const;
    void loadOnline(const std::string& apiKey, const std::string& binId,
                   std::function<void(bool)> callback = nullptr);

    // Общий метод сохранения (пытается сохранить онлайн, если не получается - локально)
    void save();
    void load();
};
