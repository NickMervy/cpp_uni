#pragma once

#include <vector>
#include <functional>

using ActivationFunc = std::function<double(std::vector<double>)>;

class INeuron
{
protected:
    double m_output = 0;

public:
    INeuron() = default;
    virtual ~INeuron() = default;
    INeuron(const INeuron &other) = default;
    INeuron &operator=(const INeuron &other) = default;
    INeuron(INeuron &&) = default;
    INeuron &operator=(INeuron &&) = default;

    double getOutput()
    {
        return m_output;
    };

    virtual void feedForward(const std::vector<double> &inputs) = 0;
    virtual void backPropagate(int answer) = 0;
};