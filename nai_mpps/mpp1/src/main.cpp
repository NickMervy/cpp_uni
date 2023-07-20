#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <regex>
#include <cstdarg>

#define WORD_SEPARATOR "\\s"
#define LOCAL "pl-PL"

struct Flower
{
    enum class Type;
    const static std::unordered_map<std::string, Type> flowerMap;

    static std::string getString(Type type);
};

class DecisionTable
{
private:
    std::vector<std::vector<double>> m_valueMatrix;
    std::vector<std::pair<double, double>> m_min_max;
    int DecisionTable::set_min_max(std::vector<bool> &flags);

public:
    const std::vector<std::vector<double>> &matrix() const { return m_valueMatrix; };
    std::vector<std::vector<double>> &matrix() { return m_valueMatrix; };
    size_t rows() const { return m_valueMatrix.size(); }
    size_t columns() const
    {
        if (m_valueMatrix.empty())
            throw std::logic_error("accessing empty matrix");
        return m_valueMatrix[0].size();
    }

    bool is_normalized() const
    {
        return std::any_of(m_min_max.begin(), m_min_max.end(), [](auto &p) { return p.first != p.second; });
    }

    int normalize(std::vector<bool> &flags);
    int normalize(std::vector<double> &c) const;
    DecisionTable &operator=(const DecisionTable &dt);
    friend std::ostream &operator<<(std::ostream &os, const DecisionTable &dt);
    friend const std::ifstream &operator>>(std::ifstream &ofs, DecisionTable &dt);
    friend const std::ofstream &operator<<(std::ofstream &ofs, DecisionTable &dt);
};

std::vector<std::string> tokenize(const std::string str, const std::regex re);
const std::string format(const char *fmt, ...);
int applyKNN(int k, std::list<std::pair<float, Flower::Type>> &distances, Flower::Type &result);
int predict(int k, const DecisionTable &training_table, std::vector<double> &new_case);
int predict(int k, DecisionTable &training_table, const DecisionTable &test_table, DecisionTable &result);
int process_data(const DecisionTable &testTable, const DecisionTable &result, int &correct, float &accuracy);

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

const std::string format(const char *fmt, ...)
{
    va_list args;
    char buff[100];
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    return buff;
}

enum class Flower::Type
{
    IrisSetosa,
    IrisVersicolor,
    IrisVirginica,
    LAST
};

std::string Flower::getString(Type type)
{
    auto it = flowerMap.begin();
    std::advance(it, (int)type);
    return it->first;
}

const std::unordered_map<std::string, Flower::Type> Flower::flowerMap =
    {
        {"Iris-setosa", Type::IrisSetosa},
        {"Iris-versicolor", Type::IrisVersicolor},
        {"Iris-virginica", Type::IrisVirginica}};

int DecisionTable::set_min_max(std::vector<bool> &flags)
{
    if (flags.size() != columns() - 1)
    {
        throw std::out_of_range(format(
            "Inconsistent number of columns to table's (%d)",
            columns() - 1));
    }

    for (size_t n = 0; n < columns() - 1; n++)
    {
        m_min_max.push_back({});
        if (flags[n] == false)
        {
            continue;
        }

        m_min_max[n].first = m_valueMatrix[0][n];
        m_min_max[n].second = m_valueMatrix[0][n];
        for (size_t m = 0; m < rows(); m++)
        {
            if (m_valueMatrix[m][n] < m_min_max[n].first)
            {
                m_min_max[n].first = m_valueMatrix[m][n];
            }

            if (m_valueMatrix[m][n] > m_min_max[n].second)
            {
                m_min_max[n].second = m_valueMatrix[m][n];
            }
        }
    }

    return 0;
};

int DecisionTable::normalize(std::vector<bool> &flags)
{
    if (is_normalized())
    {
        std::cout << "WARNING: Table already normalized" << std::endl;
        return 0;
    }

    if (flags.size() != columns() - 1)
    {
        throw std::out_of_range(format(
            "Inconsistent number of columns to table's (%d)",
            columns() - 1));
    }

    set_min_max(flags);
    for (size_t m = 0; m < rows(); m++)
    {
        for (size_t n = 0; n < columns() - 1; n++)
        {
            if (flags[n] == false)
                continue;
            m_valueMatrix[m][n] = (m_valueMatrix[m][n] - m_min_max[n].first) /
                             (m_min_max[n].second - m_min_max[n].first);
        }
    }

    return 0;
};

int DecisionTable::normalize(std::vector<double> &c) const
{
    if (is_normalized() == false)
    {
        throw std::logic_error("Table is not normilized");
    }

    double denom;
    for (size_t n = 0; n < columns() - 1; n++)
    {
        if (m_min_max[n].first == m_min_max[n].second)
            continue;

        denom = m_min_max[n].second - m_min_max[n].first;
        if (fabs(denom) <= DBL_EPSILON)
        {
            throw std::overflow_error("Division by zero exception");
        }

        c[n] = (c[n] - m_min_max[n].first) / denom;
    }

    return 0;
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
        for (size_t n = 0; n < dt.columns() - 1; n++)
        {
            std::cout << dt.m_valueMatrix[m][n] << " ";
        }

        std::cout << Flower::getString((Flower::Type)dt.m_valueMatrix[m][dt.columns() - 1]) << std::endl;
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
    const std::regex regex(WORD_SEPARATOR);
    while (std::getline(ofs, line))
    {
        std::vector<std::string> words = tokenize(line, regex);
        if (line_number != 0 && words.size() != dt.columns())
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

        auto it = Flower::flowerMap.find(words[words.size() - 1]);
        if (it == Flower::flowerMap.end())
        {
            throw std::runtime_error(
                format("Unknown decision attribute %s at line %d (check word separator)",
                       words[words.size() - 1],
                       line_number));
        }

        dt.m_valueMatrix[line_number].push_back((double)it->second);
        line_number++;
    }

    std::sort(dt.m_valueMatrix.begin(), dt.m_valueMatrix.end());
    auto last = std::unique(dt.m_valueMatrix.begin(), dt.m_valueMatrix.end());
    if (last != dt.m_valueMatrix.end())
    {
        std::cout << "WARNING: Found duplicated case (removing)" << std::endl;
        dt.m_valueMatrix.erase(last, dt.m_valueMatrix.end());
    }

    return ofs;
};

const std::ofstream &operator<<(std::ofstream &ofs, DecisionTable &dt)
{
    for (size_t m = 0; m < dt.rows(); m++)
    {
        for (size_t n = 0; n < dt.columns() - 1; n++)
        {
            ofs << dt.m_valueMatrix[m][n] << " ";
        }

        ofs << Flower::getString((Flower::Type)dt.m_valueMatrix[m][dt.columns() - 1]) << std::endl;
    }

    return ofs;
};

int applyKNN(int k, std::vector<std::pair<double, Flower::Type>> &distances, Flower::Type &result)
{
    const int length = (int)Flower::Type::LAST;
    int countArr[length]{};
    std::sort(distances.begin(), distances.end());
    for (size_t i = 0; i < k; i++)
    {
        countArr[(int)distances[i].second]++;
    }

    int max_index = std::distance(countArr, std::max_element(countArr, countArr + length));
    result = (Flower::Type)max_index;
    return 0;
}

int predict(int k, const DecisionTable &training_table, std::vector<double> &new_case)
{
    auto matrix = training_table.matrix();
    std::vector<std::pair<double, Flower::Type>> distances{};

    if (training_table.is_normalized())
    {
        training_table.normalize(new_case);
    }

    for (size_t mTrain = 0; mTrain < training_table.rows(); mTrain++)
    {
        double distance = 0;
        for (size_t n = 0; n < new_case.size() - 1; n++)
        {
            distance += (matrix[mTrain][n] - new_case[n]) *
                        (matrix[mTrain][n] - new_case[n]);
        }

        std::pair<double, Flower::Type> pair(
            distance,
            (Flower::Type)matrix[mTrain][training_table.columns() - 1]);
        distances.push_back(pair);
    }

    Flower::Type answer;
    applyKNN(k, distances, answer);
    new_case.back() = (double)answer;

    return 0;
}

int predict(int k, DecisionTable &training_table, const DecisionTable &test_table, DecisionTable &result)
{
    result = test_table;
    auto train_matrix = training_table.matrix();
    auto test_matrix = test_table.matrix();

    for (size_t m = 0; m < result.rows(); m++)
    {
        predict(k, training_table, result.matrix()[m]);
    }

    return 0;
};

int process_data(const DecisionTable &testTable, const DecisionTable &result, int &correct, double &accuracy)
{
    int total = result.rows();
    int last_column = result.columns() - 1;
    for (size_t m = 0; m < result.rows(); m++)
    {
        if (result.matrix()[m][last_column] == testTable.matrix()[m][last_column])
        {
            correct++;
        }
    }

    accuracy = (double)correct / total * 100;
    return 0;
}

int main(int argc, char const *argv[])
{
    try
    {
        setlocale(LC_ALL, LOCAL);
        DecisionTable train_table{};
        DecisionTable test_table{};
        DecisionTable result_table{};

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

        int k = 0;
        std::cout << "Enter K" << std::endl;
        std::cin >> k;
        if (k <= 0 || k > train_table.rows())
        {
            throw std::invalid_argument(
                format("K is out of interval (0, %d]",
                       train_table.rows()));
        }

        int correct = 0;
        double accuracy = 0;
        predict(k, train_table, test_table, result_table);
        process_data(test_table, result_table, correct, accuracy);
        std::cout << format("K: %d Correct: %d Accuracy: %.3f%% Normalized: %d\n",
                            k,
                            correct,
                            accuracy,
                            train_table.is_normalized());

        char c;
        std::string value;
        std::vector<double> new_case{};
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
            for (size_t n = 0; n < train_table.columns() - 1; n++)
            {
                std::cin >> value;
                new_case.push_back(std::stof(value));
            }
            new_case.push_back(0);

            predict(k, train_table, new_case);
            std::cout << format("K: %d Answer: %s\n\n",
                                k,
                                Flower::getString((Flower::Type)new_case[new_case.size() - 1]));
        }
    }

    catch (std::exception e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    };
}
