#pragma once
#include <vector>
#include <string>
#include <utility>

class HighScoresManager {
public:
    HighScoresManager(const std::string& filename = "scores.txt");

    // Загрузить/сохранить из файла
    void load();
    void save();

    // Добавить новый рекорд
    void addScore(const std::string& name, int score);

    // Получить топ-N
    std::vector<std::pair<std::string, int>> getTop(int n = 10) const;

    // Получить ВСЕ (для будущего онлайна: это будут get/set с сервера)
    const std::vector<std::pair<std::string, int>>& getAll() const { return mScores; }

private:
    std::string mFilename;
    std::vector<std::pair<std::string, int>> mScores;
};
