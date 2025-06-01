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

    const std::vector<std::pair<std::string, int>>& getAll() const { return mScores; }

    // работа с онлайн таблицой
    void sendScoreOnline(const std::string& name, int score);

    std::vector<std::pair<std::string, int>> getScoresOnline();

private:
    std::string mFilename;
    std::vector<std::pair<std::string, int>> mScores;

    const std::string JSONBIN_BIN_ID = "683c96218a456b7966a822fb";
    const std::string JSONBIN_API_KEY = "$2a$10$6vJGt4xm./2k86V8dSn9k.byyRPOExZO9Niqipr3P9cTtPXSRuf5C";
};
