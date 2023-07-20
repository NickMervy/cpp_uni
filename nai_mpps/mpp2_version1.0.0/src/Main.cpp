#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <queue>
#include <stdexcept>

#define MAX_ITERATIONS 500
#define MIN_ACCURACY 100
#define LOCAL "pl-PL"
#define SEPARATOR "\\s"

float dot(const std::vector<float> &v1, const std::vector<float> &v2)
{
    if (v1.size() != v2.size())
    {
        throw "non-equal dimenstions";
    }

    float result = 0;
    for (size_t i = 0; i < v1.size(); ++i)
    {
        result += v1[i] * v2[i];
    }

    return result;
}

void normalize(std::vector<float> &v)
{
    double length = 0;
    for (size_t i = 0; i < v.size(); ++i)
    {
        length += v[i] * v[i];
    }

    length = std::sqrt(length);

    for (size_t i = 0; i < v.size(); ++i)
    {
        v[i] /= length;
    }
}

std::vector<std::string> tokenize(const std::string str, const std::regex re)
{
    std::sregex_token_iterator it{str.begin(), str.end(), re, -1};
    std::vector<std::string> tokenized{it, {}};

    // Additional check to remove empty strings
    tokenized.erase(
        std::remove_if(tokenized.begin(),
                       tokenized.end(),
                       [](std::string const &s) {
                           return s.size() == 0;
                       }),
        tokenized.end());

    return tokenized;
};

static const std::string format(const char *fmt, ...)
{
    va_list args;
    char buff[100];
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    return buff;
}
class DecisionTable
{
private:
    std::vector<int> m_decisions;
    std::vector<std::vector<float>> m_valueMatrix;
    std::vector<std::pair<float, float>> m_min_max;
    std::unordered_map<std::string, int> m_decisionMap;

public:
    DecisionTable(std::unordered_map<std::string, int> decisionMap) : m_decisionMap(decisionMap){};
    std::vector<std::vector<float>> &matrix() { return m_valueMatrix; };
    std::vector<int> &decision() { return m_decisions; };
    size_t rows() const { return m_valueMatrix.size(); };
    size_t columns() const;
    std::string toDecisionString(int value) const;
    int toDecisionValue(std::string key) const;
    DecisionTable &operator=(const DecisionTable &dt);
    friend std::ostream &operator<<(std::ostream &os, const DecisionTable &dt);
    friend const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt);
};

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

class Teacher
{
private:
    float m_minAccuracy;
    size_t m_maxIterations;
    std::queue<Perceptron> m_notTrained;
    std::vector<Perceptron> m_trained;
    DecisionTable m_trainData;
    DecisionTable m_testData;

public:
    Teacher(DecisionTable trainData, DecisionTable testData, size_t maxIterations, float minAccuracy, std::queue<Perceptron> notTrained);

    std::vector<Perceptron> trainAll();
    void train(Perceptron &perceptron_class);
    float checkAccuracy(Perceptron &perceptron_class, DecisionTable &table);
};

size_t DecisionTable::columns() const
{
    if (m_valueMatrix.empty())
        throw std::logic_error("accessing empty matrix");
    return m_valueMatrix[0].size();
};

std::string DecisionTable::toDecisionString(int value) const
{
    auto it = m_decisionMap.begin();
    std::advance(it, (int)value);
    if (it == m_decisionMap.end())
    {
        throw std::runtime_error(
            format("No key was found by given value: %d", value));
    }

    return it->first;
};

int DecisionTable::toDecisionValue(std::string key) const
{
    auto it = m_decisionMap.find(key);
    if (it == m_decisionMap.end())
    {
        throw std::runtime_error(
            format("Unknown decision attribute "
                   "%s"
                   " (check registered keys)",
                   key));
    }

    return it->second;
};

DecisionTable &DecisionTable::operator=(const DecisionTable &dt)
{
    if (this == &dt)
    {
        return *this;
    }

    this->m_valueMatrix = dt.m_valueMatrix;
    return *this;
};

std::ostream &operator<<(std::ostream &os, const DecisionTable &dt)
{
    for (size_t m = 0; m < dt.rows(); m++)
    {
        for (size_t n = 0; n < dt.columns(); n++)
        {
            std::cout << dt.m_valueMatrix[m][n] << " ";
        }

        std::cout << dt.toDecisionString(dt.m_decisions[m]) << std::endl;
    }

    return os;
};

const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt)
{
    if (ofs.good() == false)
    {
        throw std::ifstream::failure("Exception opening/reading/closing file");
    }

    std::string line;
    size_t line_number = 0;
    const std::regex regex(SEPARATOR);
    while (std::getline(ofs, line))
    {
        std::vector<std::string> words = tokenize(line, regex);
        if (line_number != 0 && words.size() != dt.columns() + 1)
        {
            throw std::logic_error(format(
                "Inconsistency in data set columns number at line %d",
                line_number));
        }

        dt.m_valueMatrix.push_back({});
        for (size_t i = 0; i < words.size() - 1; i++)
        {
            dt.m_valueMatrix[line_number].push_back(std::stof(words[i]));
        }

        int value = dt.toDecisionValue(words[words.size() - 1]);
        dt.m_decisions.push_back(value);
        line_number++;
    }

    return ofs;
};

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

using Network = std::vector<Perceptron>;

struct Classifier
{
    Network network;
    std::unordered_map<std::string, int> map;

    int classify(std::vector<float> inputs)
    {
        // Global
        int setosa = network[0].guess(inputs);
        int versi = network[1].guess(inputs);
        int virgi = network[2].guess(inputs);

        if (setosa == 1)
        {
            return map.find(network[0].getLabel())->second;
        }

        if (virgi == 1)
        {
            return map.find(network[2].getLabel())->second;
        }

        return map.find(network[1].getLabel())->second;

        // Local
        // float setosa = network[0].guess(inputs);
        // float versi = network[1].guess(inputs);
        // float virgi = network[2].guess(inputs);

        // if (setosa == 1)
        // {
        //     return map.find(network[0].getLabel())->second;
        // }

        // if (versi == 1)
        // {
        //     return map.find(network[2].getLabel())->second;
        // }

        // if (virgi == 1)
        // {
        //     return map.find(network[2].getLabel())->second;
        // }

        // setosa = network[0].raw(inputs);
        // versi = network[1].raw(inputs);
        // virgi = network[2].raw(inputs);

        // int maxIndex = setosa >= versi ? 0 : (versi >= virgi ? 1 : 2);
        // return map.find(network[maxIndex].getLabel())->second;
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
    setlocale(LC_ALL, LOCAL);
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
    perceptrons.push({1, 0.1f, train_table.columns(), "Iris-setosa"});
    perceptrons.push({-1, 0.1f, train_table.columns(), "Iris-versicolor"});
    perceptrons.push({10, 0.1f, train_table.columns(), "Iris-virginica"});

    Teacher teacher(train_table, test_table, MAX_ITERATIONS, MIN_ACCURACY, perceptrons);
    Network trained = teacher.trainAll();

    std::cout << trained[0].getLabel() << " " << teacher.checkAccuracy(trained[0], test_table) << "%" << std::endl;
    std::cout << trained[1].getLabel() << " " << teacher.checkAccuracy(trained[1], test_table) << "%" << std::endl;
    std::cout << trained[2].getLabel() << " " << teacher.checkAccuracy(trained[2], test_table) << "%" << std::endl;

    for (size_t i = 0; i < perceptrons.size(); ++i)
    {
        std::cout << trained[i] << std::endl;
    }

    Classifier classifier{trained, decision_map};
    int correct = 0;
    float accuracy = 0;
    checkAccuracy(test_table, classifier, correct, accuracy);
    std::cout << "Correct: " << correct << " Accuracy: " << accuracy << "%" << std::endl;

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
