#include "Perceptron.h"
#include "Utils.h"

Perceptron::Perceptron(int label, size_t dimensions, double threshold, double learningRate, Layer &prevLayer)
    : m_label(label), m_dimensions(dimensions), m_threshold(threshold), m_learningRate(learningRate), m_prevLayer(prevLayer)
{
    m_weights = {};
    for (size_t i = 0; i < m_dimensions; i++)
    {
        m_weights.push_back(rand() % 10);
    }

    m_weights.push_back(-m_threshold);
};

void Perceptron::feedForward(const std::vector<double> &inputs)
{
    // normalizeW();
    m_output = dot(m_weights, inputs);
};

void Perceptron::normalizeW()
{
    double length = 0;
    for (size_t i = 0; i < m_weights.size(); ++i)
    {
        length += m_weights[i] * m_weights[i];
    }

    length = std::sqrt(length);

    for (size_t i = 0; i < m_weights.size(); ++i)
    {
        m_weights[i] /= length;
    }
}

void Perceptron::backPropagate(int answer)
{
    std::vector<double> X {};
    for (size_t i = 0; i < m_weights.size(); ++i)
    {
        X.push_back(m_prevLayer[i]->getOutput());
    }
    
    int step_result = m_output;
    while ((step_result = dot(m_weights, X) >= 0) != answer)
    {
        for (size_t i = 0; i < m_weights.size(); ++i)
        {
            m_weights[i] += (answer - step_result) * X[i] * m_learningRate;
        }
    }

    m_output = dot(m_weights, X);
};

std::ostream &operator<<(std::ostream &os, const class Perceptron &p)
{
    os << p.m_label << ": W = [";
    for (size_t i = 0; i < p.m_weights.size(); i++)
    {
        if (i == p.m_weights.size() - 1)
        {
            os << p.m_weights[i];
            break;
        }

        os << p.m_weights[i] << ", ";
    }

    os << "], Threshold = " << p.m_threshold;
    return os;
};