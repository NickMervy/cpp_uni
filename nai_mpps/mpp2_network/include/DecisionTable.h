#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>

class DecisionTable
{
private:
    std::vector<int> m_decisions;
    std::vector<std::vector<double>> m_valueMatrix;
    std::vector<std::pair<double, double>> m_min_max;
    std::unordered_map<std::string, int> m_decisionMap;
    int DecisionTable::set_min_max(std::vector<bool> &flags);

public:
    explicit DecisionTable(std::unordered_map<std::string, int> decisionMap) : m_decisionMap(decisionMap){};
    std::vector<std::vector<double>> &matrix() { return m_valueMatrix; };
    std::vector<int> &decision() { return m_decisions; };
    size_t rows() const { return m_valueMatrix.size(); };
    size_t columns() const;
    std::string toDecisionString(int value) const;
    int toDecisionValue(std::string key) const;
    bool is_normalized() const;

    int normalize(std::vector<bool> &flags);
    int normalize(std::vector<double> &c) const;
    DecisionTable &operator=(const DecisionTable &dt);
    friend std::ostream &operator<<(std::ostream &os, const DecisionTable &dt);
    friend const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt);
    friend const std::ofstream &operator<<(std::ofstream &ofs, DecisionTable &dt);
};