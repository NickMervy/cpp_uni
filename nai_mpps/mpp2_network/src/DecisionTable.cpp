#include "DecisionTable.h"
#include "Utils.h"

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

bool DecisionTable::is_normalized() const
{
    return std::any_of(m_min_max.begin(), m_min_max.end(), [](auto &p) { return p.first != p.second; });
};

int DecisionTable::set_min_max(std::vector<bool> &flags)
{
    if (flags.size() != columns())
    {
        throw std::out_of_range(format(
            "Inconsistent number of columns to table's (%d)",
            columns()));
    }

    for (size_t n = 0; n < columns(); n++)
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

    if (flags.size() != columns())
    {
        throw std::out_of_range(format(
            "Inconsistent number of columns to table's (%d)",
            columns()));
    }

    set_min_max(flags);
    for (size_t m = 0; m < rows(); m++)
    {
        for (size_t n = 0; n < columns(); n++)
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
    for (size_t n = 0; n < columns(); n++)
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
    const std::regex regex("\\s");
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

    // std::sort(dt.m_valueMatrix.begin(), dt.m_valueMatrix.end());
    // auto last = std::unique(dt.m_valueMatrix.begin(), dt.m_valueMatrix.end());
    // if (last != dt.m_valueMatrix.end())
    // {
    //     std::cout << "WARNING: Found duplicated case (removing)" << std::endl;
    //     dt.m_valueMatrix.erase(last, dt.m_valueMatrix.end());
    // }

    return ofs;
};

const std::ofstream &operator<<(std::ofstream &ofs, DecisionTable &dt)
{
    for (size_t m = 0; m < dt.rows(); m++)
    {
        for (size_t n = 0; n < dt.columns(); n++)
        {
            ofs << dt.m_valueMatrix[m][n] << " ";
        }

        ofs << dt.toDecisionString(dt.m_decisions[m]) << std::endl;
    }

    return ofs;
};
