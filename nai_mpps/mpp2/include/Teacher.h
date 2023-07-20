#pragma once

#include "Perceptron.h"
#include "DecisionTable.h"
#include "Utils.h"

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <queue>
#include <stdexcept>


class Teacher
{
private:
    float m_minAccuracy;
    size_t m_maxIterations;
    std::queue<Perceptron> m_notTrained;
    std::vector<Perceptron> m_trained;
    DecisionTable m_trainData;
    DecisionTable m_testData;

public:
    Teacher(DecisionTable trainData, DecisionTable testData, size_t maxIterations, float minAccuracy, std::queue<Perceptron> notTrained);

    std::vector<Perceptron> trainAll();
    void train(Perceptron &perceptron_class);
    float checkAccuracy(Perceptron &perceptron_class, DecisionTable &table);
};