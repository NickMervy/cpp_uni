#include <string>
#include <cctype>
#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>
#include <cstdarg>
#include <algorithm>
#include <map>
#include <stdexcept>
#include <unordered_map>

#define WORD_SEPARATOR "\\s"
#define LOCAL "pl-PL"
#define DATA_PATH "./res/iris_training.txt"

struct Point
{
    std::vector<double> Coordinates{};
    std::string DecisionAttribute{};

    friend bool operator!=(const Point &l, const Point &r);
};

struct Cluster
{
    std::vector<Point> Points{};
    Point Centroid{};

    void computeCentroid();
    double computeDistancesSum();
    int cardinality();
    double entropy();
    friend bool operator!=(const Cluster &l, const Cluster &r);
};

double distanceSquare(Point &a, Point &b);
std::vector<std::string> tokenize(const std::string str, const std::regex re);
const std::string format(const char *fmt, ...);
const std::ifstream &operator>>(std::ifstream &ofs, std::vector<Point> &points);
int initializeClustersRandomly(int k, std::vector<Point> points, std::vector<Cluster> &result);

void Cluster::computeCentroid()
{
    if (Points.empty())
    {
        for (size_t i = 0; i < Centroid.Coordinates.size(); i++)
        {
            Centroid.Coordinates[i] = DBL_MAX;
        }

        return;
    }

    if (Centroid.Coordinates.size() != Points[0].Coordinates.size())
    {
        Centroid.Coordinates = std::vector<double>(Points[0].Coordinates.size(), 0.0f);
    }

    for (size_t i = 0; i < Points[0].Coordinates.size(); i++)
    {
        double sumCoordinate = 0;
        for (size_t j = 0; j < Points.size(); j++)
        {
            sumCoordinate += Points[j].Coordinates[i];
        }

        Centroid.Coordinates[i] = sumCoordinate / Points.size();
    }
};

int Cluster::cardinality()
{
    return Points.size();
}

double Cluster::entropy()
{
    std::unordered_map<std::string, int> occurances{};
    for (size_t i = 0; i < Points.size(); i++)
    {
        auto it = occurances.find(Points[i].DecisionAttribute);
        if (occurances.find(Points[i].DecisionAttribute) == occurances.end())
        {
            occurances.insert({Points[i].DecisionAttribute, 1});
            continue;
        }

        (*it).second++;
    }

    double entropy = 0;
    for (auto it = occurances.begin(); it != occurances.end(); it++)
    {
        double probability = (double)(*it).second / Points.size();
        entropy += probability * log2(probability);
    }

    return -entropy + 0.0f;
}

bool operator!=(const Point &l, const Point &r)
{
    if (l.Coordinates.size() != r.Coordinates.size())
    {
        return true;
    }

    for (size_t i = 0; i < l.Coordinates.size(); i++)
    {
        if (l.Coordinates[i] != r.Coordinates[i])
        {
            return true;
        }
    }

    return false;
};

bool operator!=(const Cluster &l, const Cluster &r)
{
    return l.Centroid != r.Centroid || l.Points.size() != r.Points.size();
};

double distanceSquare(Point &a, Point &b)
{
    if (a.Coordinates.size() != b.Coordinates.size())
    {
        throw std::exception("Invalid points dimensions");
    }

    double result = 0;
    size_t size = a.Coordinates.size();
    for (size_t i = 0; i < size; i++)
    {
        result += (a.Coordinates[i] - b.Coordinates[i]) * (a.Coordinates[i] - b.Coordinates[i]);
    }

    return result;
};

const std::string format(const char *fmt, ...)
{
    va_list args;
    char buff[100];
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    return buff;
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

const std::ifstream &operator>>(std::ifstream &ofs, std::vector<Point> &points)
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
        if (line_number != 0 && words.size() - 1 != points[0].Coordinates.size())
        {
            throw std::logic_error(format(
                "Inconsistency in data set columns number at line %d",
                line_number));
        }

        points.push_back({});
        for (size_t i = 0; i < words.size() - 1; i++)
        {
            points[line_number].Coordinates.push_back(std::stof(words[i]));
        }

        points[line_number].DecisionAttribute = words[words.size() - 1];
        line_number++;
    }

    return ofs;
};

int initializeClustersRandomly(int k, std::vector<Point> points, std::vector<Cluster> &clusters)
{
    int pointsCount = points.size();
    int minNumberPointsInCluster = pointsCount / k;

    for (size_t i = 0; i < k; i++)
    {
        int counter = 0;
        while (counter < minNumberPointsInCluster)
        {
            int selectedElement = rand() % points.size();
            //swap
            Point point = points[selectedElement];
            points[selectedElement] = points.back();
            points.pop_back();

            clusters[i].Points.push_back(point);
            counter++;
        }
    }

    while (points.size() != 0)
    {
        int selectedCluster = rand() % clusters.size();
        int selectedElement = rand() % points.size();
        //swap
        Point point = points[selectedElement];
        points[selectedElement] = points.back();
        points.pop_back();

        clusters[selectedCluster].Points.push_back(point);
    }

    return 0;
};

bool isEqual(const std::vector<Cluster> &l, const std::vector<Cluster> &r)
{
    for (size_t i = 0; i < l.size(); i++)
    {
        if (l[i] != r[i])
        {
            return false;
        }
    }

    return true;
}

double Cluster::computeDistancesSum()
{
    double sum = 0;
    for (size_t i = 0; i < Points.size(); i++)
    {
        sum += distanceSquare(Points[i], Centroid);
    }

    return sum;
}

int main()
{
    setlocale(LC_ALL, LOCAL);
    srand(time(NULL));

    std::vector<Point> points{};
    std::ifstream data_file(DATA_PATH);
    data_file >> points;

    int k = 0;
    std::cout << "Enter K: ";
    std::cin >> k;

    if (k <= 0 || k > points.size())
    {
        std::cout << "Invalid K value" << std::endl;
        return 0;
    }

    if (k == points.size())
    {
        std::cout << "K equals number of data points. The system will be overtrained with such K value" << std::endl;
        return 0;
    }

    std::vector<Cluster> clusters(k, Cluster{});

    initializeClustersRandomly(k, points, clusters);

    int counter = 0;
    std::vector<Cluster> oldClusters(k, Cluster{});
    while (!isEqual(clusters, oldClusters))
    {
        oldClusters = clusters;
        for (size_t i = 0; i < clusters.size(); i++)
        {
            clusters[i].computeCentroid();
            clusters[i].Points.clear();
        }

        for (size_t i = 0; i < points.size(); i++)
        {
            double minDistance = DBL_MAX;
            int minClusterIndex = 0;
            for (size_t j = 0; j < clusters.size(); j++)
            {
                double d = distanceSquare(points[i], clusters[j].Centroid);
                if (d < minDistance)
                {
                    minDistance = d;
                    minClusterIndex = j;
                }
            }

            clusters[minClusterIndex].Points.push_back(points[i]);
        }

        double totalSum = 0;
        std::cout << "ITERATION " << counter++ << std::endl;
        for (size_t i = 0; i < clusters.size(); i++)
        {
            double itSum = clusters[i].computeDistancesSum();
            std::cout << "Cluster " << i << " sum: " << itSum << std::endl;
            totalSum += itSum;
        }

        std::cout << "Total sum: " << totalSum << std::endl;
    }

    std::cout << std::endl;
    std::cout << "FINALL RESULTS" << std::endl;
    for (size_t i = 0; i < clusters.size(); i++)
    {
        std::cout << "Cluster " << i << ": " << std::endl;
        std::cout << "Cardinality = " << clusters[i].cardinality() << std::endl;
        std::cout << "Entropy = " << clusters[i].entropy() << std::endl;
    }

    return 0;
};