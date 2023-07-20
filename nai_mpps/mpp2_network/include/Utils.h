#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <regex>
#include <cstdarg>
#include <unordered_map>
#include <queue>
#include <stdexcept>

static float dot(const std::vector<double> &v1, const std::vector<double> &v2)
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

static std::vector<std::string> tokenize(const std::string str, const std::regex re)
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