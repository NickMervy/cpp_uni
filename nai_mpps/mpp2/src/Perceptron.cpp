#include "Perceptron.h"

Perceptron::Perceptron(float threshold, float learning_rate, size_t dimensions, std::string label)
    : m_threshold(threshold), m_learning_rate(learning_rate), m_dimensions(dimensions), m_label(label)
{
    m_weights = {};
    for (size_t i = 0; i < m_dimensions; i++)
    {
        m_weights.push_back(rand() % 100);
    }
}

float Perceptron::raw(const std::vector<float> &input) const
{
    std::vector<float> tmp = m_weights;
    normalize(tmp);
    return dot(tmp, input);
}

int Perceptron::guess(const std::vector<float> &input) const
{
    return dot(m_weights, input) >= m_threshold;
};

void Perceptron::learn(const std::vector<float> &input, int answer)
{
    int output;
    while ((output = guess(input)) != answer)
    {
        for (size_t i = 0; i < m_weights.size(); ++i)
        {
            m_weights[i] += (answer - output) * input[i] * m_learning_rate;
        }

        m_threshold -= (answer - output) * m_learning_rate;
    }
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
}
