#include "Perceptron.h"
#include "DecisionTable.h"
#include "Teacher.h"

#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <stdexcept>

using Network = std::vector<Perceptron>;
#define MAX_ITERATIONS 1000
#define MIN_ACCURACY 100

struct Classifier
{
    Network network;
    std::unordered_map<std::string, int> map;

    int classify(std::vector<float> inputs)
    {
        float setosa = network[0].guess(inputs);
        float versi = network[1].guess(inputs);
        float virgi = network[2].guess(inputs);

        if (setosa == 1)
        {
            return map.find(network[0].getLabel())->second;
        }

        if (virgi == 1)
        {
            return map.find(network[2].getLabel())->second;
        }

        if (setosa == 0 && virgi == 0)
        {
            return map.find(network[1].getLabel())->second;
        }

        // setosa = network[0].raw(inputs);
        // versi = network[1].raw(inputs);
        // virgi = network[2].raw(inputs);

        // int maxIndex = setosa >= versi ? 0 : (versi >= virgi ? 1 : 2);

        return map.find(network[1].getLabel())->second;
    };
};

int checkAccuracy(DecisionTable &testTable, Classifier &classifier, int &correct, float &accuracy)
{
    int total = testTable.rows();
    for (size_t m = 0; m < testTable.rows(); m++)
    {
        if (classifier.classify(testTable.matrix()[m]) == testTable.decision()[m])
        {
            correct++;
        }
    }

    accuracy = (float)correct / total * 100;
    return 0;
}

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "pl-PL");
    srand(time(NULL));

    std::unordered_map<std::string, int> decision_map =
        {
            {"Iris-setosa", 0},
            {"Iris-versicolor", 1},
            {"Iris-virginica", 2}};

    DecisionTable train_table(decision_map);
    DecisionTable test_table(decision_map);

    std::ifstream train_file(argv[1]);
    std::ifstream test_file(argv[2]);

    // create training table based of training set
    train_file >> train_table;
    std::cout << "Training data loaded" << std::endl;
    train_file.close();

    test_file >> test_table;
    std::cout << "Testing data loaded" << std::endl;
    test_file.close();
    if (train_table.columns() != test_table.columns())
    {
        throw std::logic_error(format(
            "Number of columns(%d) in training set is not equel to the number of columns(%d) in training set",
            train_table.columns(),
            test_table.columns()));
    }

    std::queue<Perceptron> perceptrons{};
    perceptrons.push({1, 1.0f, train_table.columns(), "Iris-setosa"});
    perceptrons.push({1, 0.1f, train_table.columns(), "Iris-versicolor"});
    perceptrons.push({1, 0.1f, train_table.columns(), "Iris-virginica"});

    Teacher teacher(train_table, test_table, MAX_ITERATIONS, MIN_ACCURACY, perceptrons);
    Network trained = teacher.trainAll();

    std::cout << trained[0].getLabel() << " " << teacher.checkAccuracy(trained[0], test_table) << std::endl;
    std::cout << trained[1].getLabel() << " " << teacher.checkAccuracy(trained[1], test_table) << std::endl;
    std::cout << trained[2].getLabel() << " " << teacher.checkAccuracy(trained[2], test_table) << std::endl;

    for (size_t i = 0; i < perceptrons.size(); ++i)
    {
        std::cout << trained[i] << std::endl;
    }

    Classifier classifier{trained, decision_map};
    int correct = 0;
    float accuracy = 0;
    checkAccuracy(test_table, classifier, correct, accuracy);
    std::cout << "Correct: " << correct << " Accuracy: " << accuracy << std::endl;

    char c;
    std::string value;
    std::vector<float> new_case{};
    while (true)
    {
        std::cout << "Continue? (y/n)" << std::endl;
        std::cin >> c;
        if (c != 'y')
        {
            break;
        }

        new_case.clear();
        std::cout << "Enter your values" << std::endl;
        for (size_t n = 0; n < train_table.columns(); n++)
        {
            std::cin >> value;
            new_case.push_back(std::stof(value));
        }

        int answer = classifier.classify(new_case);
        std::cout << format("Answer: %s\n\n", train_table.toDecisionString(answer));
    }

    return 0;
};
