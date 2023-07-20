#include <algorithm>
#include "OutputNeuron.h"

OutputNeuron::OutputNeuron(){};

void OutputNeuron::feedForward(const std::vector<double> &inputs)
{
    auto it = std::max_element(inputs.begin(), inputs.end());
    m_output = std::distance(inputs.begin(), it);
};

void OutputNeuron::backPropagate(int answer){};