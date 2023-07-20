#include "InputNeuron.h"

InputNeuron::InputNeuron(size_t index) : m_index(index){};

void InputNeuron::feedForward(const std::vector<double> &inputs)
{
    m_output = inputs[m_index];
};

void InputNeuron::backPropagate(int answer){};