#pragma once

#include <string>
#include "INeuron.h"

class InputNeuron : public INeuron
{
private:
    size_t m_index;

public:
    InputNeuron(size_t index);
    ~InputNeuron() = default;
    InputNeuron(const InputNeuron &other) = default;
    InputNeuron &operator=(const InputNeuron &other) = default;
    InputNeuron(InputNeuron &&) = default;
    InputNeuron &operator=(InputNeuron &&) = default;

    void feedForward(const std::vector<double> &inputs) override;
    void backPropagate(int answer) override;
};