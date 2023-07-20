#include <string>
#include <cctype>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <cstdarg>
#include <map>
#include <stdexcept>

namespace fs = std::filesystem;

#define ERROR_THRESHOLD 0.01f
#define LETTERS_COUNT 26
#define THRESHOLD 0.01f
#define ALPHA 0.01f
#define MIN_WEIGHT 0.01f
#define MAX_WEIGHT 1.0f

#define TRAIN_DATA_PATH "./res/train"
#define TEST_DATA_PATH "./res/test"

using DecisionTable = std::map<std::string, std::vector<std::map<char, float>>>;

float dot(const std::vector<float> &v1, const std::vector<float> &v2)
{
    if (v1.size() != v2.size())
    {
        throw "non-equal dimentions";
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
    float weightSum = 0.0f;
    for (size_t i = 0; i < v.size(); i++)
    {
        weightSum += v[i];
    }

    float normCoef = 1.0f / weightSum;

    for (size_t i = 0; i < v.size(); i++)
    {
        v[i] = v[i] * normCoef;
    }
}

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

struct Perceptron
{
    std::string Label;
    float Threshold;
    std::vector<float> Weights;

    float predict(std::vector<float> &input);
    void deltaRule(std::vector<float> &input, float desired, float output);
};

float Perceptron::predict(std::vector<float> &input)
{
    float net = dot(input, Weights) - Threshold;
    return net;
}

void Perceptron::deltaRule(std::vector<float> &input, float desired, float output)
{
    float error = desired - output;
    for (size_t i = 0; i < Weights.size(); i++)
    {
        Weights[i] += error * ALPHA * input[i];
    }

    Threshold += error * ALPHA * (-1.0f);
    normalize(Weights);
};

void getDistribution(std::string str, std::map<char, float> &result)
{
    unsigned int num_of_chars = 0;
    int occurrences[LETTERS_COUNT]{};

    for (size_t i = 0; i < str.length(); i++)
    {
        str[i] = tolower(str[i]);
        if (str[i] < 'a' || str[i] > 'z')
        {
            continue;
        }

        occurrences[str[i] - 'a']++;
        num_of_chars++;
    }

    for (size_t i = 0; i < LETTERS_COUNT; i++)
    {
        char character = i + 'a';
        float relative_freq = (float)occurrences[i] / num_of_chars;
        result.insert({character, relative_freq});
    }
}

int parse(const std::string &path, DecisionTable &result)
{
    std::vector<std::string> language_paths{};
    for (const auto &dir_entry : fs::directory_iterator(path))
    {
        std::string dir_name = fs::path(dir_entry.path()).filename().string();
        result.insert({dir_name, {}});
        for (const auto &file_entry : fs::directory_iterator(dir_entry.path()))
        {
            std::ifstream file(file_entry.path());
            std::string str((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

            std::map<char, float> chars_distribution{};
            getDistribution(str, chars_distribution);
            result[dir_name].push_back(chars_distribution);
            file.close();
        }
    }

    return 0;
}

std::string answer(std::vector<float> &input, std::vector<Perceptron> &perceptrons)
{
    float net = 0;
    std::vector<float> nets{};
    for (size_t w = 0; w < perceptrons.size(); w++)
    {
        net = perceptrons[w].predict(input);
        nets.push_back(net);
    }

    std::vector<float>::iterator result_it = std::max_element(nets.begin(), nets.end());
    return perceptrons[std::distance(nets.begin(), result_it)].Label;
}

float calculateAccuracy(DecisionTable &data, std::vector<Perceptron> &perceptrons)
{
    int correct = 0;
    int total = 0;
    DecisionTable::iterator it_test = data.begin();
    while (it_test != data.end())
    {
        ConfusionMatrix conf_matrix;
        std::vector<std::map<char, float>> input = it_test->second;
        for (size_t v = 0; v < input.size(); v++)
        {
            std::vector<float> input_values{};
            std::for_each(input[v].begin(), input[v].end(), [&](std::pair<const char, float> &element) {
                input_values.push_back(element.second);
            });

            std::string answer_str = answer(input_values, perceptrons);
            if (it_test->first == answer_str)
            {
                correct++;
            }

            total++;
        }

        it_test++;
    }

    float accuracy = ((float)correct / total) * 100.0f;
    return accuracy;
}

void calculateConfMatrix(DecisionTable &data, std::vector<Perceptron> &perceptrons, std::map<std::string, ConfusionMatrix> &result)
{
    DecisionTable::iterator it_test = data.begin();
    while (it_test != data.end())
    {
        std::vector<std::map<char, float>> input = it_test->second;
        for (size_t v = 0; v < input.size(); v++)
        {
            std::vector<float> input_values{};
            std::for_each(input[v].begin(), input[v].end(), [&](std::pair<const char, float> &element) {
                input_values.push_back(element.second);
            });

            std::string answer_str = answer(input_values, perceptrons);
            if (it_test->first == answer_str)
            {
                for (auto &it = result.begin(); it != result.end(); ++it)
                {
                    if (it->first == answer_str)
                    {
                        result[it_test->first].True_positive++;
                        continue;
                    }

                    result[it->first].True_negative++;
                }
            }
            else
            {
                result[it_test->first].False_negative++;
                result[answer_str].False_positive++;
                for (auto &it = result.begin(); it != result.end(); ++it)
                {
                    if (it->first == answer_str || it->first == it_test->first)
                        continue;
                    result[it->first].True_negative++;
                }
            }
        }

        it_test++;
    }
}

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    DecisionTable train_data{};
    DecisionTable test_data{};

    // Foreach language folder add a new language name key and associated map with symbol distribution
    parse(TRAIN_DATA_PATH, train_data);
    parse(TEST_DATA_PATH, test_data);

    // Create perceptrons
    std::vector<Perceptron> perceptrons{};
    DecisionTable::iterator it = train_data.begin();
    while (it != train_data.end())
    {
        Perceptron perceptron{};
        perceptron.Label = it->first;
        perceptron.Threshold = THRESHOLD;

        // Weights generation
        for (size_t i = 0; i < LETTERS_COUNT; i++)
        {
            float randomf = 0.0f;
            perceptron.Weights.push_back(randomf);
        }

        perceptrons.push_back(perceptron);
        it++;
    }

    // Training
    float accuracy = 0.0f;
    while (std::abs(100 - accuracy) > ERROR_THRESHOLD)
    {
        DecisionTable::iterator it_train = train_data.begin();
        while (it_train != train_data.end())
        {
            std::vector<std::map<char, float>> input = it_train->second;
            for (size_t v = 0; v < input.size(); v++)
            {
                std::vector<float> input_values{};
                std::for_each(input[v].begin(), input[v].end(), [&](std::pair<const char, float> &element) {
                    input_values.push_back(element.second);
                });

                float output = 0;
                float desired = 0;
                for (size_t i = 0; i < perceptrons.size(); i++)
                {
                    output = perceptrons[i].predict(input_values);
                    //std::distance(train_data.begin(), it_train)
                    if (perceptrons[i].Label == it_train->first)
                    {
                        desired = 1.0f;
                    }
                    else
                    {
                        desired = 0.0f;
                    }

                    perceptrons[i].deltaRule(input_values, desired, output);
                }
            }

            it_train++;
        }

        accuracy = calculateAccuracy(test_data, perceptrons);
    }

    std::map<std::string, ConfusionMatrix> confusion_matrices{};
    for (auto &it = train_data.begin(); it != train_data.end(); ++it)
    {
        confusion_matrices.insert({it->first, {}});
    }

    calculateConfMatrix(test_data, perceptrons, confusion_matrices);
    for (auto &it = confusion_matrices.begin(); it != confusion_matrices.end(); ++it)
    {
        std::cout << "Language: " << it->first << std::endl;
        std::cout << "TP: " << it->second.True_positive << " FP : " << it->second.False_positive << std::endl;
        std::cout << "FN: " << it->second.False_negative << " TN : " << it->second.True_negative << std::endl;
        std::cout << "Accuracy: " << it->second.accuracy() << std::endl;
        std::cout << "Precision: " << it->second.precision() << std::endl;
        std::cout << "Recall: " << it->second.recall() << std::endl;
        std::cout << "F1: " << it->second.f1() << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Finall accuracy: " << accuracy << std::endl;

    char c;
    std::string input_text;
    while (true)
    {
        std::cout << "Enter your text (q to exit)" << std::endl;
        getline(std::cin, input_text);

        if (input_text == "q")
        {
            break;
        }

        std::map<char, float> char_distribution{};
        getDistribution(input_text, char_distribution);
        std::vector<float> input_values{};
        std::for_each(char_distribution.begin(), char_distribution.end(), [&](std::pair<const char, float> &element) {
            input_values.push_back(element.second);
        });

        std::cout << answer(input_values, perceptrons) << std::endl;
    }
}