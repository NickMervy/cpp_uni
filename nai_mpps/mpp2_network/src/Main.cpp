#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <stdexcept>
#include <time.h>

#include "Supervisor.h"
#include "InputNeuron.h"
#include "BiasNeuron.h"
#include "Perceptron.h"
#include "OutputNeuron.h"
#include "DecisionTable.h"
#include "Utils.h"

std::unordered_map<std::string, int> decision_map =
    {
        {"Iris-setosa", 0},
        {"Iris-versicolor", 1},
        {"Iris-virginica", 2}};

void constructTopology(DecisionTable &trainTable, std::vector<Layer> &topology)
{
    topology.push_back({});
    topology.push_back({});
    topology.push_back({});
    for (size_t i = 0; i < trainTable.columns(); ++i)
    {
        topology[0].push_back(std::make_unique<InputNeuron>(i));
    }

    topology[0].push_back(std::make_unique<BiasNeuron>(1.0));

    for (size_t i = 0; i < decision_map.size(); ++i)
    {
        topology[1].push_back(std::make_unique<Perceptron>(
            i,
            trainTable.columns(),
            1.0,
            0.1,
            topology[0]));
    }

    topology[2].push_back(std::make_unique<OutputNeuron>());
}

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "pl-PL");
    srand(time(NULL));

    DecisionTable trainTable(decision_map);
    DecisionTable testTable(decision_map);

    std::ifstream trainFile(argv[1]);
    std::ifstream testFile(argv[2]);

    // create training table based of training set
    trainFile >> trainTable;
    std::cout << "Training data loaded" << std::endl;
    trainFile.close();

    testFile >> testTable;
    std::cout << "Testing data loaded" << std::endl;
    testFile.close();
    if (trainTable.columns() != testTable.columns())
    {
        throw std::logic_error(format(
            "Number of columns(%d) in training set is not equel to the number of columns(%d) in training set",
            trainTable.columns(),
            testTable.columns()));
    }

    std::vector<Layer> irisTopology{};
    constructTopology(trainTable, irisTopology);

    Network network(irisTopology);
    Supervisor supervisor(10000, trainTable, testTable, network);
    supervisor.train();
    double accuracy = supervisor.getAccuracy();

    std::cout << accuracy << std::endl;

    return 0;
};
