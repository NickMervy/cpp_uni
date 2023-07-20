#include "assert.h"
#include "Network.h"

Network::Network(std::vector<Layer> &topology)
{
    assert(topology.empty() == false);

    m_layers = topology;
}

std::vector<Layer> Network::getLayers() const
{
    return m_layers;
}

void Network::feedForward(const std::vector<double> &inputs)
{
    std::vector<double> previous = inputs;
    std::vector<double> current{};

    for (size_t layerNum = 0; layerNum < m_layers.size(); ++layerNum)
    {
        for (size_t neuronNum = 0; neuronNum < m_layers[layerNum].size(); ++neuronNum)
        {
            m_layers[layerNum][neuronNum]->feedForward(previous);
            current.push_back(m_layers[layerNum][neuronNum]->getOutput());
        }

        previous = current;
        current.clear();
    }
}

void Network::backPropagate(const std::vector<int> &answers)
{
    for (size_t layerNum = m_layers.size() - 2; layerNum > 0; --layerNum)
    {
        const size_t &neuronsCount = m_layers[layerNum].size();
        for (size_t neuronNum = 0; neuronNum < neuronsCount; ++neuronNum)
        {
            m_layers[layerNum][neuronNum]->backPropagate(answers[neuronNum]);
        }
    }
}

double Network::getResult()
{
    return m_layers.back().back()->getOutput();
}
