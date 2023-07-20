#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <fstream>
#include <iostream>
#include <cmath>
#include <regex>
#include <vector>

#define INPUT_FILE "knapsack.txt"

struct Dataset
{
    std::string Name{};
    std::vector<int> Sizes{};
    std::vector<int> Values{};
};

struct Knapsack
{
    size_t Length;
    size_t Capacity;

    std::vector<Dataset> Datasets{};
};

void parse(const std::string &filename, Knapsack &knapsack);
std::pair<uint32_t, uint32_t> compute_total_value_and_size(uint32_t bits, const Dataset &dataset);
void print_results(uint32_t combination, const Dataset &dataset, double delta_time);

void parse(const std::string &filename, Knapsack &knapsack)
{
    static const std::regex HEADER_REGEX("^\\s*length\\s*-\\s*(\\d+)\\s*,\\s*capacity\\s*(\\d+)\\s*$");
    static const std::regex DATASET_NAME_REGEX("^\\s*(\\w+\\s*\\d*)\\s*:*\\s*$");
    static const std::regex DATASET_SIZE_REGEX("^\\s*[sS][iI][zZ][eE][sS]\\s*=\\s*\\{(.*)\\}\\s*$");
    static const std::regex DATASET_VALUES_REGEX("^\\s*[vV][aA][lL][sS]\\s*=\\s*\\{(.*)\\}\\s*$");
    static const std::regex DIGIT_REGEX("\\d+");

    std::ifstream v_input_file(filename);
    std::string v_line{};

    if (v_input_file.is_open() == false)
    {
        std::cerr << "Could not open the file" << std::endl;
        v_input_file.close();
        return;
    }

    int v_line_counter = 0;
    bool v_headers_read = false;

    while (std::getline(v_input_file, v_line))
    {
        v_line_counter++;
        std::smatch matches{};
        if (v_headers_read == false)
        {
            if (std::regex_match(v_line, matches, HEADER_REGEX))
            {
                knapsack.Length = std::stoi(matches[1]);
                knapsack.Capacity = std::stoi(matches[2]);
                v_headers_read = true;
            }

            if (knapsack.Length > 32)
            {
                std::cerr << "Too big length in \"" << std::endl;
                return;
            }

            continue;
        }

        if (std::regex_match(v_line, matches, DATASET_NAME_REGEX))
        {
            knapsack.Datasets.push_back({});
            knapsack.Datasets.back().Name = matches[1];
            continue;
        }

        if (std::regex_match(v_line, matches, DATASET_SIZE_REGEX))
        {
            std::string str = matches[1];
            while (std::regex_search(str, matches, DIGIT_REGEX))
            {
                int size = std::stoi(matches.str());
                knapsack.Datasets.back().Sizes.push_back(size);
                str = matches.suffix();
            }

            continue;
        }

        if (std::regex_match(v_line, matches, DATASET_VALUES_REGEX))
        {
            std::string str = matches[1];
            while (std::regex_search(str, matches, DIGIT_REGEX))
            {
                int value = std::stoi(matches.str());
                knapsack.Datasets.back().Values.push_back(value);
                str = matches.suffix();
            }

            continue;
        }

        auto items = knapsack.Datasets.back();
        if (items.Sizes.size() != items.Values.size())
        {
            std::cerr << "Non-equal dimensions in \"" << items.Name << '\"' << std::endl;
            return;
        }
    }

    if (v_line_counter == 0)
    {
        std::cerr << "No input data" << std::endl;
    }

    v_input_file.close();
};

std::pair<uint32_t, uint32_t> compute_total_value_and_size(uint32_t bits, const Dataset &dataset)
{
    uint32_t mask = 0x01u;
    uint32_t total_value = 0;
    uint32_t total_size = 0;

    for (size_t i = 0; i < dataset.Sizes.size(); i++)
    {
        if (!((bits >> i) & mask))
            continue;
        total_value += dataset.Values[i];
        total_size += dataset.Sizes[i];
    }

    return {total_value, total_size};
};

void print_results(uint32_t combination, const Dataset &dataset, float delta_time)
{
    uint32_t mask = 0x01u;
    uint32_t total_value = 0;
    uint32_t total_size = 0;

    std::cout << "Best combination:" << std::endl;
    std::cout << "nr val sz" << std::endl;
    for (size_t i = 0; i < dataset.Sizes.size(); i++)
    {
        if (!((combination >> i) & mask))
            continue;

        total_value += dataset.Values[i];
        total_size += dataset.Sizes[i];
        printf("[%d, %d, %d]\n", i, dataset.Sizes[i], dataset.Values[i]);
    }

    std::cout << "Total value: " << total_value << std::endl;
    std::cout << "Total size: " << total_size << std::endl;
    std::cout << "Delta time: " << delta_time << std::endl;
};

int main(int argc, char const *argv[])
{
    srand(time(NULL));

    __int64 counts_per_sec;
    QueryPerformanceFrequency((LARGE_INTEGER *)&counts_per_sec);
    double seconds_per_count = 1.0 / (double)counts_per_sec;

    __int64 start_time;
    QueryPerformanceCounter((LARGE_INTEGER *)&start_time);

    Knapsack knapsack{};
    parse(INPUT_FILE, knapsack);

    Dataset chosen_dataset = knapsack.Datasets[std::rand() % knapsack.Datasets.size()];
    std::cout << "Randomly chosen dataset: "
              << "\"" << chosen_dataset.Name << "\"" << std::endl;

    uint32_t max_total_value = 0;
    uint32_t combination = 0x00000000;
    uint32_t best_combination = combination;
    uint32_t last_combination = ~((~combination) << knapsack.Length);
    for (; combination <= last_combination; ++combination)
    {
        auto total_value_and_size = compute_total_value_and_size(combination, chosen_dataset);
        if (total_value_and_size.second > knapsack.Capacity)
            continue;
        if (total_value_and_size.first > max_total_value)
        {
            max_total_value = total_value_and_size.first;
            best_combination = combination;
        }
    }

    __int64 end_time;
    QueryPerformanceCounter((LARGE_INTEGER *)&end_time);

    float delta_time = (float)((end_time - start_time) * seconds_per_count);
    print_results(best_combination, chosen_dataset, delta_time);

    std::cin.get();
    return 0;
}
