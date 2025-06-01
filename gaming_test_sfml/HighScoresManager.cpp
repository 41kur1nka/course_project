#include "HighScoresManager.h"
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

HighScoresManager::HighScoresManager(const std::string& filename)
    : mFilename(filename)
{
    load();
}

void HighScoresManager::load() {
    mScores.clear();
    std::ifstream in(mFilename);
    std::string name;
    int score;
    while (in >> name >> score) {
        mScores.emplace_back(name, score);
    }
    // сортируем по убыванию
    std::sort(mScores.begin(), mScores.end(), [](auto& a, auto& b) {
        return a.second > b.second;
        });
}

void HighScoresManager::save() {
    std::ofstream out(mFilename, std::ios::trunc);
    for (const auto& p : mScores) {
        out << p.first << " " << p.second << "\n";
    }
}

void HighScoresManager::addScore(const std::string& name, int score) {
    mScores.emplace_back(name, score);
    std::sort(mScores.begin(), mScores.end(), [](auto& a, auto& b) {
        return a.second > b.second;
        });
    save();
}

std::vector<std::pair<std::string, int>> HighScoresManager::getTop(int n) const {
    if (mScores.size() <= (size_t)n)
        return mScores;
    return std::vector<std::pair<std::string, int>>(mScores.begin(), mScores.begin() + n);
}

void HighScoresManager::sendScoreOnline(const std::string& name, int score)
{
    // 1. Получить массив текущих результатов
    auto r = cpr::Get(
        cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID + "/latest" },
        cpr::Header{ {"X-Master-Key", JSONBIN_API_KEY} }
    );
    nlohmann::json scores;
    if (r.status_code == 200) {
        nlohmann::json doc = nlohmann::json::parse(r.text);
        scores = doc["record"];
    }
    // 2. Добавить свой результат
    scores.push_back({ {"name", name}, {"score", score} });
    // 3. (Опционально) Отсортировать по score (по убыванию):
    std::sort(scores.begin(), scores.end(), [](const auto& a, const auto& b) {
        return a["score"].get<int>() > b["score"].get<int>();
        });
    // 4. Перезаписать Bin
    cpr::Put(
        cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID },
        cpr::Header{
            {"Content-Type", "application/json"},
            {"X-Master-Key", JSONBIN_API_KEY}
        },
        cpr::Body{ scores.dump() }
    );
}

std::vector<std::pair<std::string, int>> HighScoresManager::getScoresOnline()
{
    std::vector<std::pair<std::string, int>> result;
    auto r = cpr::Get(
        cpr::Url{ "https://api.jsonbin.io/v3/b/" + JSONBIN_BIN_ID + "/latest" },
        cpr::Header{ {"X-Master-Key", JSONBIN_API_KEY} }
    );
    if (r.status_code == 200) {
        nlohmann::json doc = nlohmann::json::parse(r.text);
        auto& scores = doc["record"];
        for (auto& item : scores) {
            result.emplace_back(item["name"].get<std::string>(), item["score"].get<int>());
        }
    }
    return result;
}
