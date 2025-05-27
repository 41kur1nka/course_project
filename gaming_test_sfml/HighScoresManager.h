#pragma once
#include <vector>
#include <string>
#include <utility>

class HighScoresManager {
public:
    HighScoresManager(const std::string& filename = "scores.txt");

    // ���������/��������� �� �����
    void load();
    void save();

    // �������� ����� ������
    void addScore(const std::string& name, int score);

    // �������� ���-N
    std::vector<std::pair<std::string, int>> getTop(int n = 10) const;

    // �������� ��� (��� �������� �������: ��� ����� get/set � �������)
    const std::vector<std::pair<std::string, int>>& getAll() const { return mScores; }

private:
    std::string mFilename;
    std::vector<std::pair<std::string, int>> mScores;
};
