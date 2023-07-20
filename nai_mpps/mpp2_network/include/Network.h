#pragma once

#include <vector>
#include <memory>
#include "INeuron.h"

using Layer = std::vector<std::shared_ptr<INeuron>>;

class Network
{
private:
    std::vector<Layer> m_layers;
public:
    Network(std::vector<Layer> &topology);
    Network() = delete;
    Network(const Network &other) = delete;
    Network &operator=(const Network &other) = delete;
    Network(Network &&) = delete;
    Network &operator=(Network &&) = delete;
    ~Network() = default;

    std::vector<Layer> getLayers() const;
    void feedForward(const std::vector<double> &inputs);
    void backPropagate(const std::vector<int> &answers);
    double getResult();
};