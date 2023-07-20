#pragma once

#include <string>
#include <queue>

#include "Perceptron.h"
#include "Network.h"
#include "DecisionTable.h"

class Supervisor
{
private:
    uint32_t m_iterations;
    DecisionTable m_trainSet;
    DecisionTable m_testSet;
    Network &m_network;

public:
    explicit Supervisor(uint32_t iterations,
                        DecisionTable trainData,
                        DecisionTable testData,
                        Network &network);
    Supervisor() = delete;
    Supervisor(const Supervisor &other) = delete;
    Supervisor &operator=(const Supervisor &other) = delete;
    Supervisor(Supervisor &&) = delete;
    Supervisor &operator=(Supervisor &&) = delete;

    void train();
    double getAccuracy();
};