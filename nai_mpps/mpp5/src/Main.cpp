#define _USE_MATH_DEFINES

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <stdexcept>
#include <cmath>
#include <format>

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
                       [](std::string const &s)
                       {
                           return s.size() == 0;
                       }),
        tokenized.end());

    return tokenized;
};

float normal_distribution(float mean, float std_dev, float x)
{
    float nom = exp(-((x - mean) * (x - mean) / (2 * std_dev * std_dev)));
    float denom = sqrtf(2 * M_PI) * std_dev;
    return nom / denom;
};

struct ConfusionMatrix
{
    int True_positive = 0;
    int False_positive = 0;
    int True_negative = 0;
    int False_negative = 0;

    float accuracy() const;
    float precision() const;
    float recall() const;
    float f1() const;
};

class DecisionTable
{
private:
    std::vector<int> m_decisions;
    std::vector<std::vector<float>> m_valueMatrix;
    std::vector<std::pair<float, float>> m_min_max;
    std::unordered_map<std::string, int> m_decisionMap;
    std::unordered_map<size_t, std::vector<size_t>> m_class_attribute_counts;

    std::string m_text_separator;

public:
    std::unordered_map<size_t, std::vector<float>> class_attribute_means;
    std::unordered_map<size_t, std::vector<float>> class_attribute_std_devs;

    DecisionTable(std::unordered_map<std::string, int> decisionMap, std::string text_separator) : m_decisionMap(decisionMap), m_text_separator(text_separator){};
    std::vector<std::vector<float>> &matrix() { return m_valueMatrix; };
    std::vector<int> &decision() { return m_decisions; };
    size_t rows() const { return m_valueMatrix.size(); };
    size_t columns() const;
    std::string toDecisionString(int value) const;
    int toDecisionValue(std::string key) const;
    DecisionTable &operator=(const DecisionTable &dt);
    void computeStatistics();
    friend std::ostream &operator<<(std::ostream &os, const DecisionTable &dt);
    friend const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt);
};

class Classifier
{
private:
    DecisionTable &m_trainData;
    DecisionTable &m_testData;

public:
    Classifier(DecisionTable &trainData, DecisionTable &testData) : m_trainData(trainData), m_testData(testData){};
    void printPerfMeasurments(std::unordered_map<int, ConfusionMatrix> &result);
    int classify(std::vector<float> &new_case);
};

float ConfusionMatrix::accuracy() const
{
    int denom = True_positive + True_negative + False_positive + False_negative;
    return (float)(True_positive + True_negative) / denom;
};

float ConfusionMatrix::precision() const
{
    int denom = True_positive + False_positive;
    if (denom == 0)
    {
        return std::nanf("");
    }

    return (float)True_positive / denom;
};

float ConfusionMatrix::recall() const
{
    int denom = True_positive + False_negative;
    if (denom == 0)
    {
        return std::nanf("");
    }

    return (float)True_positive / denom;
};

float ConfusionMatrix::f1() const
{
    float p = precision();
    float r = recall();
    float denom = p + r;
    if (denom == 0)
    {
        return std::nanf("");
    }

    return 2.0f * p * r / (p + r);
};

void DecisionTable::computeStatistics()
{
    for (auto &&it : m_decisionMap)
    {
        m_class_attribute_counts.insert({it.second, std::vector<size_t>(columns(), 0)});
        class_attribute_means.insert({it.second, std::vector<float>(columns(), 0)});
        class_attribute_std_devs.insert({it.second, std::vector<float>(columns(), 0)});
    }

    for (size_t i = 0; i < rows(); i++)
    {
        for (size_t j = 0; j < columns(); j++)
        {
            m_class_attribute_counts[m_decisions[i]][j]++;
            class_attribute_means[m_decisions[i]][j] += m_valueMatrix[i][j];
        }
    }

    for (size_t i = 0; i < m_class_attribute_counts.size(); i++)
    {
        for (size_t j = 0; j < m_class_attribute_counts[i].size(); j++)
        {
            class_attribute_means[i][j] /= m_class_attribute_counts[i][j];
        }
    }

    for (size_t i = 0; i < rows(); i++)
    {
        for (size_t j = 0; j < columns(); j++)
        {
            class_attribute_std_devs[m_decisions[i]][j] += (m_valueMatrix[i][j] - class_attribute_means[m_decisions[i]][j]) *
                                                           (m_valueMatrix[i][j] - class_attribute_means[m_decisions[i]][j]);
        }
    }

    for (size_t i = 0; i < m_class_attribute_counts.size(); i++)
    {
        for (size_t j = 0; j < m_class_attribute_counts[i].size(); j++)
        {
            float denom = 1.0f / (m_class_attribute_counts[i][j] - 1);
            class_attribute_std_devs[i][j] = sqrtf(denom * class_attribute_std_devs[i][j]);
        }
    }
};

void Classifier::printPerfMeasurments(std::unordered_map<int, ConfusionMatrix> &result)
{
    int correct = 0;
    for (size_t i = 0; i < m_testData.rows(); i++)
    {
        int answer = classify(m_testData.matrix()[i]);
        if (answer == m_testData.decision()[i])
        {
            ++correct;
            for (auto &&it : result)
            {
                if (it.first == m_testData.decision()[i])
                {
                    it.second.True_positive++;
                    continue;
                }

                it.second.True_negative++;
            }
        }

        else
        {
            result[m_testData.decision()[i]].False_negative++;
            result[answer].False_positive++;
            for (auto &&it : result)
            {
                if (it.first == answer || it.first == m_testData.decision()[i])
                    continue;
                it.second.True_negative++;
            }
        }
    }

    for (auto &&it : result)
    {
        std::cout << "Flower: " << m_trainData.toDecisionString(it.first) << std::endl;
        std::cout << "TP: " << it.second.True_positive << " FP : " << it.second.False_positive << std::endl;
        std::cout << "FN: " << it.second.False_negative << " TN : " << it.second.True_negative << std::endl;
        std::printf("Accuracy: %.2f%%\n", it.second.accuracy() * 100);
        std::printf("Precision:%.2f%%\n", it.second.precision() * 100);
        std::printf("Recall: %.2f%%\n", it.second.recall() * 100);
        std::printf("F1: %.2f%%\n", it.second.f1() * 100);
        std::cout << std::endl;
    }

    std::printf("Accuracy: %.2f%%\n", (float)correct / m_testData.rows() * 100);
};

int Classifier::classify(std::vector<float> &new_case)
{
    if (new_case.size() != m_trainData.columns())
    {
        return -1;
    }

    int answer = -1;
    float max_prob = 0.0f;

    for (size_t i = 0; i < m_trainData.class_attribute_means.size(); i++)
    {
        float total = 1;
        for (size_t j = 0; j < new_case.size(); j++)
        {
            total *= normal_distribution(m_trainData.class_attribute_means[i][j],
                                         m_trainData.class_attribute_std_devs[i][j],
                                         new_case[j]);
        }

        if (total > max_prob)
        {
            max_prob = total;
            answer = i;
        }
    }

    return answer;
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
            std::format("No key was found by given value: %d", value));
    }

    return it->first;
};

int DecisionTable::toDecisionValue(std::string key) const
{
    auto it = m_decisionMap.find(key);
    if (it == m_decisionMap.end())
    {
        throw std::runtime_error(
            std::format("Unknown decision attribute "
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
    const std::regex regex(dt.m_text_separator);
    while (std::getline(ofs, line))
    {
        std::vector<std::string> words = tokenize(line, regex);
        if (line_number != 0 && words.size() != dt.columns() + 1)
        {
            throw std::logic_error(std::format(
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

int main(int argc, char const *argv[])
{
    setlocale(NULL, argv[4]);

    std::unordered_map<std::string, int> decision_map =
        {
            {"Iris-setosa", 0},
            {"Iris-versicolor", 1},
            {"Iris-virginica", 2}};

    DecisionTable train_table(decision_map, argv[3]);
    DecisionTable test_table(decision_map, argv[3]);

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
        throw std::logic_error(std::format(
            "Number of columns(%d) in training set is not equel to the number of columns(%d) in training set",
            train_table.columns(),
            test_table.columns()));
    }

    train_table.computeStatistics();
    Classifier classifier(train_table, test_table);

    std::unordered_map<int, ConfusionMatrix> confusion_matrices{};
    for (auto &&it : decision_map)
    {
        confusion_matrices.insert({it.second, {}});
    }

    classifier.printPerfMeasurments(confusion_matrices);

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
        std::cout << "Answer: " << train_table.toDecisionString(answer) << std::endl;
        std::cout << std::endl;
    }

    return 0;
};
