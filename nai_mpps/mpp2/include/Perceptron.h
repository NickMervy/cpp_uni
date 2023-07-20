#pragma once

#include "Utils.h"

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <queue>
#include <stdexcept>

class Perceptron
{
private:
    std::vector<float> m_weights;
    float m_threshold;
    float m_learning_rate;
    size_t m_dimensions;
    std::string m_label;

public:
    Perceptron(float threshold, float learning_rate, size_t dimensions, std::string label);
    std::string getLabel() const { return m_label; };
    int Perceptron::guess(const std::vector<float> &input) const;
    float Perceptron::raw(const std::vector<float> &input) const;
    void learn(const std::vector<float> &input, int answer);

    friend std::ostream &operator<<(std::ostream &os, const class Perceptron &dt);
};