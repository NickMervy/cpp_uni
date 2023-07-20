#include "Supervisor.h"

Supervisor::Supervisor(uint32_t iterations, DecisionTable trainData, DecisionTable testData, Network &network)
    : m_iterations(iterations), m_trainSet(trainData), m_testSet(testData), m_network(network){};

void Supervisor::train()
{
    size_t iterations = 0;
    while (iterations < m_iterations)
    {
        for (size_t m = 0; m < m_trainSet.rows(); ++m)
        {
            m_network.feedForward(m_trainSet.matrix()[m]);

            int answer = m_trainSet.decision()[m];
            std::vector<int> answers {};
            for (size_t i = 0; i < 3; i++)
            {
                if(answer == i)
                {
                    answers.push_back(1);
                    continue;
                }

                answers.push_back(0);
            }
            
            m_network.backPropagate(answers);
        }

        iterations++;
    }
}

double Supervisor::getAccuracy()
{
    int correct = 0;
    int total = m_testSet.rows();
    for (size_t m = 0; m < total; ++m)
    {
        m_network.feedForward(m_testSet.matrix()[m]);
        double result = m_network.getResult();

        if(result == m_testSet.decision()[m])
        {
            correct++;
        }
    }

    return (double)correct / total * 100;
}