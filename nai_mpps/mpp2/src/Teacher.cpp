#include "Teacher.h"

Teacher::Teacher(DecisionTable trainData, DecisionTable testData, size_t maxIterations, float minAccuracy, std::queue<Perceptron> notTrained)
    : m_trainData(trainData), m_testData(testData), m_maxIterations(maxIterations), m_minAccuracy(minAccuracy), m_notTrained(notTrained){};

std::vector<Perceptron> Teacher::trainAll()
{
    int iterations = 0;

    for (size_t i = m_notTrained.size(); i > 0; --i)
    {
        train(m_notTrained.front());
    }

    return m_trained;
};

void Teacher::train(Perceptron &perceptron)
{
    int iterations = 0;
    float accuracy = 0;
    while ((accuracy = checkAccuracy(perceptron, m_trainData)) < m_minAccuracy &&
           iterations < m_maxIterations)
    {
        for (size_t m = 0; m < m_trainData.rows(); ++m)
        {
            perceptron.learn(
                m_trainData.matrix()[m],
                m_trainData.toDecisionValue(perceptron.getLabel()) == m_trainData.decision()[m] ? 1 : 0);
        }

        ++iterations;
    }

    Perceptron tmp = perceptron;
    m_notTrained.pop();
    m_trained.push_back(tmp);
};

float Teacher::checkAccuracy(Perceptron &perceptron, DecisionTable &table)
{
    int correct = 0;
    int total = 0;
    total = table.rows();
    for (size_t m = 0; m < table.rows(); ++m)
    {
        if (table.toDecisionValue(perceptron.getLabel()) == table.decision()[m])
        {
            if (perceptron.guess(table.matrix()[m]) == 1)
            {
                correct++;
            }
        }

        else
        {
            if (perceptron.guess(table.matrix()[m]) == 0)
            {
                correct++;
            }
        }
    }

    if (total == 0)
    {
        throw std::logic_error(format(
            "Total number of %s in train data is equal 0", perceptron.getLabel()));
    }

    float accuracy = (float)correct / total * 100;
    return accuracy;
};
