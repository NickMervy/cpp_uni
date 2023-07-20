#pragma once

#include <string>
#include "INeuron.h"

class BiasNeuron : public INeuron
{
private:
    double m_constant;

public:
    BiasNeuron(double constant);
    ~BiasNeuron() = default;
    BiasNeuron(const BiasNeuron &other) = default;
    BiasNeuron &operator=(const BiasNeuron &other) = default;
    BiasNeuron(BiasNeuron &&) = default;
    BiasNeuron &operator=(BiasNeuron &&) = default;

    void feedForward(const std::vector<double> &inputs) override;
    void backPropagate(int answer) override;
};