#pragma once

#include <string>
#include "INeuron.h"

class OutputNeuron : public INeuron
{
public:
    explicit OutputNeuron();
    ~OutputNeuron() = default;
    OutputNeuron(const OutputNeuron &other) = default;
    OutputNeuron &operator=(const OutputNeuron &other) = default;
    OutputNeuron(OutputNeuron &&) = default;
    OutputNeuron &operator=(OutputNeuron &&) = default;

    void feedForward(const std::vector<double> &inputs) override;
    void backPropagate(int answer) override;
};