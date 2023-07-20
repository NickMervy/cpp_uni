#pragma once

#include <string>
#include "INeuron.h"
#include "Network.h"

class Perceptron : public INeuron
{
private:
    std::vector<double> m_weights;
    int m_label;
    size_t m_dimensions;
    double m_threshold;
    double m_learningRate;
    Layer &m_prevLayer;

    void normalizeW();
public:
    explicit Perceptron(int label,
                        size_t dimensions,
                        double threshold,
                        double learning_rate,
                        Layer &prevLayer);
    ~Perceptron() = default;
    Perceptron(const Perceptron &other) = default;
    Perceptron &operator=(const Perceptron &other) = default;
    Perceptron(Perceptron &&) = default;
    Perceptron &operator=(Perceptron &&) = default;

    void feedForward(const std::vector<double> &inputs) override;
    void backPropagate(int answer) override;
    friend std::ostream &operator<<(std::ostream &os, const class Perceptron &dt);
};