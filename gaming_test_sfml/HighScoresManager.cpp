#include "HighScoresManager.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <iostream>

using json = nlohmann::json;

HighScoresManager::HighScoresManager(const std::string& filename)
    : mFilename(filename)
{
    load();
}

void HighScoresManager::load() {
    // Сначала пробуем загрузить онлайн
    auto onlineScores = getScoresOnline();
    if (!onlineScores.empty()) {
        mScores = onlineScores;
    } else {
        // Если не получилось - загружаем локально
        mScores.clear();
        std::ifstream in(mFilename);
        std::string name;
        int score;
        while (in >> name >> score) {
            mScores.emplace_back(name, score);
        }
    }
    // Сортировка по убыванию
    std::sort(mScores.begin(), mScores.end(), 
        [](const auto& a, const auto& b) { return a.second > b.second; });
}

void HighScoresManager::save() {
    std::ofstream out(mFilename, std::ios::trunc);
    for (const auto& p : mScores) {
        out << p.first << " " << p.second << "\n";
    }
}

void HighScoresManager::addScore(const std::string& name, int score) {
    std::cout << "[DEBUG] addScore called for name: " << name << " score: " << score << std::endl;
    mScores.emplace_back(name, score);
    std::sort(mScores.begin(), mScores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    save();
    // Отправляем очки онлайн
    sendScoreOnline(name, score);

    // После успешного онлайна — чистим локальный файл (или не записываем туда этот результат)
    // Например, очищаем файл:
    std::ofstream ofs(mFilename, std::ios::trunc); // очистить файл
}

std::vector<std::pair<std::string, int>> HighScoresManager::getTop(int n) const {
    if (mScores.size() <= (size_t)n)
        return mScores;
    return std::vector<std::pair<std::string, int>>(mScores.begin(), mScores.begin() + n);
}

void HighScoresManager::sendScoreOnline(const std::string& name, int score)
{
    try {
        // 1. Получаем текущие результаты
        auto r = cpr::Get(
            cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID + "/latest" },
            cpr::Header{ {"X-Master-Key", JSONBIN_API_KEY} }
        );

        json scores;
        if (r.status_code == 200 && !r.text.empty()) {
            json doc = json::parse(r.text);
            if (doc.contains("record") && doc["record"].contains("record") && doc["record"]["record"].is_array()) {
                scores = doc["record"]["record"];
            }
            else {
                scores = json::array();
            }

        } else {
            scores = json::array(); // Создаем новый пустой массив
        }

        // 2. Добавляем новый результат
        scores.push_back({
            {"name", name},
            {"score", score}
        });

        // 3. Сортируем по убыванию очков
        std::sort(scores.begin(), scores.end(),
            [](const auto& a, const auto& b) {
                return a["score"].get<int>() > b["score"].get<int>();
            });

        // 4. Ограничиваем количество сохраняемых результатов
        if (scores.size() > 100) {
            scores.erase(scores.begin() + 100, scores.end());
        }

        // 5. Обновляем Bin
        json finalJson = { {"record", scores} };
        auto response = cpr::Put(
            cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID },
            cpr::Header{
                {"Content-Type", "application/json"},
                {"X-Master-Key", JSONBIN_API_KEY}
            },
            cpr::Body{ finalJson.dump() }
        );

        if (response.status_code != 200) {
            std::cerr << "Failed to update online scores. Status code: " 
                     << response.status_code << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error sending score online: " << e.what() << std::endl;
    }
}

std::vector<std::pair<std::string, int>> HighScoresManager::getScoresOnline()
{
    std::vector<std::pair<std::string, int>> result;
    try {
        auto r = cpr::Get(
            cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID + "/latest" },
            cpr::Header{ {"X-Master-Key", JSONBIN_API_KEY} }
        );

        if (r.status_code == 200 && !r.text.empty()) {
            try {
                nlohmann::json doc = nlohmann::json::parse(r.text);

                // Переходим внутрь doc["record"]["record"] если возможно
                if (doc.contains("record") && doc["record"].contains("record") && doc["record"]["record"].is_array()) {
                    const auto& scores = doc["record"]["record"];
                    for (const auto& item : scores) {
                        if (item.contains("name") && item.contains("score")) {
                            result.emplace_back(
                                item["name"].get<std::string>(),
                                item["score"].get<int>()
                            );
                        }
                    }
                }
                else {
                    std::cerr << "[ERROR] Structure not as expected (no record/record array)!\n";
                    std::cerr << doc.dump(2) << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "JSON parse error in getScoresOnline: " << e.what() << std::endl;
            }
        }
        else {
            std::cerr << "Failed to get online scores. Status code: "
                << r.status_code << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error getting scores online: " << e.what() << std::endl;
    }
    return result;
}
