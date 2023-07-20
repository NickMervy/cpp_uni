#pragma once

#include "Utils.h"

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <queue>
#include <stdexcept>

class DecisionTable
{
private:
    std::vector<int> m_decisions;
    std::vector<std::vector<float>> m_valueMatrix;
    std::vector<std::pair<float, float>> m_min_max;
    std::unordered_map<std::string, int> m_decisionMap;
    int DecisionTable::set_min_max(std::vector<bool> &flags);

public:
    DecisionTable(std::unordered_map<std::string, int> decisionMap) : m_decisionMap(decisionMap){};
    std::vector<std::vector<float>> &matrix() { return m_valueMatrix; };
    std::vector<int> &decision() { return m_decisions; };
    size_t rows() const { return m_valueMatrix.size(); };
    size_t columns() const;
    std::string toDecisionString(int value) const;
    int toDecisionValue(std::string key) const;
    bool is_normalized() const;

    int normalize(std::vector<bool> &flags);
    int normalize(std::vector<float> &c) const;
    DecisionTable &operator=(const DecisionTable &dt);
    friend std::ostream &operator<<(std::ostream &os, const DecisionTable &dt);
    friend const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt);
    friend const std::ofstream &operator<<(std::ofstream &ofs, DecisionTable &dt);
};