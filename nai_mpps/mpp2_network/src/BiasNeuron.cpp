#include "BiasNeuron.h"

BiasNeuron::BiasNeuron(double constant) : m_constant(constant){};

void BiasNeuron::feedForward(const std::vector<double> &inputs)
{
    m_output = m_constant;
};

void BiasNeuron::backPropagate(int answer){};