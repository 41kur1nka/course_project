#pragma once
#include <vector>
#include <string>
#include <utility>

class HighScoresManager {
public:
    HighScoresManager(const std::string& filename = "scores.txt");

    // Download/save in file
    void load();
    void save();

    // Add new record
    void addScore(const std::string& name, int score);

    // Get top-N
    std::vector<std::pair<std::string, int>> getTop(int n = 10) const;

    const std::vector<std::pair<std::string, int>>& getAll() const { return mScores; }

    void sendScoreOnline(const std::string& name, int score);

    std::vector<std::pair<std::string, int>> getScoresOnline();

private:
    std::string mFilename;
    std::vector<std::pair<std::string, int>> mScores;

    const std::string JSONBIN_BIN_ID = "683c96218a456b7966a822fb";
    const std::string JSONBIN_API_KEY = "$2a$10$6vJGt4xm./2k86V8dSn9k.byyRPOExZO9Niqipr3P9cTtPXSRuf5C";
};
