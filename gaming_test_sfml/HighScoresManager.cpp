#include "HighScoresManager.h"
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
