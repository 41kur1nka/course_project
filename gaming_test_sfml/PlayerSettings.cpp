#include <stdexcept>
#include <sstream>
#include <fstream>
#include <iostream>
#include "PlayerSettings.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Константы для API
const std::string PlayerSettings::API_KEY = "$2a$10$6vJGt4xm./2k86V8dSn9k.byyRPOExZO9Niqipr3P9cTtPXSRuf5C";
const std::string PlayerSettings::BIN_ID = "683d65f08561e97a501e8897"; // Новый bin для настроек игрока

// Callback функция для curl
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void PlayerSettings::saveLocal(const std::string& path) const
{
    std::ofstream out(path);
    if (out) {
        out << name << std::endl << skinIndex << std::endl;
    }
}

void PlayerSettings::loadLocal(const std::string& path)
{
    std::ifstream in(path);
    if (in) {
        std::getline(in, name);
        in >> skinIndex;
        in.ignore();
    }
}

void PlayerSettings::saveOnline(const std::string& apiKey, const std::string& binId,
                              std::function<void(bool)> callback) const {
    CURL* curl = curl_easy_init();
    if (curl) {
        try {
            // Подготовка данных в правильном формате
            json data = {
    {"record", {
        {"name", name},
        {"skinIndex", skinIndex}
    }}
            };
            std::string jsonStr = data.dump();
            std::cout << "Saving settings. JSON structure: " << data.dump(2) << std::endl;

            // Заголовки
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, ("X-Master-Key: " + apiKey).c_str());
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "X-Bin-Versioning: false");

            // URL
            std::string url = "https://api.jsonbin.io/v3/b/" + binId;

            // Настройка запроса
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());

            // Получение ответа
            std::string response;
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Выполнение запроса
            CURLcode res = curl_easy_perform(curl);
            bool success = (res == CURLE_OK);

            if (success) {
                std::cout << "Settings saved successfully. Response: " << response << std::endl;
                
                // Попробуем распарсить ответ для проверки
                try {
                    json responseJson = json::parse(response);
                    std::cout << "Save response structure: " << responseJson.dump(2) << std::endl;
                } catch (const std::exception& e) {
                    std::cerr << "Warning: Could not parse save response: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "CURL error during settings save: " << curl_easy_strerror(res) << std::endl;
            }

            // Очистка
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            if (callback) {
                callback(success);
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error during settings save: " << e.what() << std::endl;
            if (callback) {
                callback(false);
            }
        }
    } else {
        if (callback) {
            callback(false);
        }
    }
}

void PlayerSettings::loadOnline(const std::string& apiKey, const std::string& binId,
                              std::function<void(bool)> callback) {

    CURL* curl = curl_easy_init();
    if (curl) {
        // Заголовки
        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, ("X-Master-Key: " + apiKey).c_str());

        // URL
        std::string url = "https://api.jsonbin.io/v3/b/" + binId + "/latest";

        // Настройка запроса
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Получение ответа
        std::string response;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Выполнение запроса
        CURLcode res = curl_easy_perform(curl);
        bool success = false;

        if (res == CURLE_OK) {
            try {
                std::cout << "Settings loaded. Raw response: " << response << std::endl;
                json j = json::parse(response);
                std::cout << "Parsed JSON structure: " << j.dump(2) << std::endl;
                
                // Рекурсивная функция для поиска настроек
                const std::function<const json*(const json&)> findSettingsObject = 
                    [&findSettingsObject](const json& obj) -> const json* {
                    if (obj.is_object()) {
                        // Если текущий объект содержит нужные поля, возвращаем его
                        if (obj.contains("name") && obj.contains("skinIndex")) {
                            return &obj;
                        }
                        // Иначе ищем в его полях
                        for (const auto& [key, value] : obj.items()) {
                            if (value.is_object()) {
                                if (const json* found = findSettingsObject(value)) {
                                    return found;
                                }
                            }
                        }
                    }
                    return nullptr;
                };

                // Ищем объект с настройками на любом уровне вложенности
                if (const json* settingsObj = findSettingsObject(j)) {
                    name = (*settingsObj)["name"].get<std::string>();
                    skinIndex = (*settingsObj)["skinIndex"].get<int>();
                    success = true;
                    std::cout << "Successfully loaded settings. Name: " << name 
                            << ", SkinIndex: " << skinIndex << std::endl;
                } else {
                    std::cerr << "Could not find settings object in the response" << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Error parsing settings JSON: " << e.what() << std::endl;
            }
        } else {
            std::cerr << "CURL error while loading settings: " << curl_easy_strerror(res) << std::endl;
        }

        // Очистка
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (callback) {
            callback(success);
        }
    }
}

void PlayerSettings::save() {
    // Сначала сохраняем онлайн
    saveOnline(API_KEY, BIN_ID, [this](bool success) {
        if (!success) {
            std::cerr << "Failed to save settings online, saving locally..." << std::endl;
            saveLocal();
        } else {
            // Если онлайн сохранение успешно, все равно сохраняем локально как бэкап
            saveLocal();
        }
    });
}

void PlayerSettings::load() {
    // Сначала пробуем загрузить онлайн
    loadOnline(API_KEY, BIN_ID, [this](bool success) {
        if (!success) {
            std::cout << "Failed to load settings online, loading from local file..." << std::endl;
            loadLocal();
        } else {
            // Если онлайн загрузка успешна, обновляем локальный файл
            saveLocal();
        }
    });
}
