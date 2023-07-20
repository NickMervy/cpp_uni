#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <cstdarg>
#include <vector>

// Constants
#define MAX_ITERATIONS 16
#define POINTS_COUNT 11
#define LENGTH DirectX::XM_PI
#define dx LENGTH / (POINTS_COUNT - 1)
#define dt 0.2f
#define RESULT_FILE "./res/results.csv"
#define ENERGIES_FILE "./res/energies.csv"

std::ofstream g_result_file(RESULT_FILE);
std::ofstream g_energies_file(ENERGIES_FILE);

const std::string format(const char *fmt, ...)
{
    va_list args;
    char buff[100];
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    return buff;
};

void printEnergies(const std::vector<float> &velocities, const std::vector<float> &points_y)
{
    float Ek = 0;
    float Ep = 0;
    float sum = 0;

    for (size_t i = 0; i < POINTS_COUNT; i++)
    {
        sum += velocities[i] * velocities[i];
    }

    Ek = (dx / 2.0f) * sum;

    sum = 0;
    float delta_y = 0;
    for (size_t i = 0; i < POINTS_COUNT; i++)
    {
        if (i > 0)
        {
            delta_y = points_y[i] - points_y[i - 1];
        }

        sum += delta_y * delta_y;
    }

    Ep = 1.0f / (2.0f * dx) * sum;

    g_energies_file << Ek << ',' << Ep << ',' << Ek + Ep << std::endl;
};

void mid_point()
{
    std::vector<float> points_x(POINTS_COUNT, 0);
    std::vector<float> points_y(POINTS_COUNT, 0);
    std::vector<float> points_y2(POINTS_COUNT, 0);
    std::vector<float> velocities(POINTS_COUNT, 0);
    std::vector<float> accelerations(POINTS_COUNT, 0);
    std::vector<float> accelerations2(POINTS_COUNT, 0);

    // t0
    float x = 0;
    for (size_t p = 0; p < POINTS_COUNT; p++)
    {
        points_x[p] = x;
        points_y[p] = sinf(x);
        points_y2[p] = points_y[p] + velocities[p] * (dt / 2.0f);

        x += dx;
    }

    for (size_t p = 0; p < POINTS_COUNT; p++)
    {
        g_result_file << points_y[p] << ",";
    }

    g_result_file << std::endl;
    printEnergies(velocities, points_y);

    int i = 0;
    while (i < MAX_ITERATIONS)
    {
        float v2 = 0;
        for (size_t p = 1; p < POINTS_COUNT - 1; p++)
        {
            accelerations2[p] = (points_y2[p + 1] - 2.0f * points_y2[p] + points_y2[p - 1]) / (dx * dx);
            accelerations[p] = (points_y[p + 1] - 2.0f * points_y[p] + points_y[p - 1]) / (dx * dx);
        }

        for (size_t p = 0; p < POINTS_COUNT; p++)
        {
            v2 = velocities[p] + accelerations[p] * dt / 2.0f;
            points_y[p] += v2 * dt;
            velocities[p] += accelerations2[p] * dt;
            points_y2[p] = points_y[p] + velocities[p] * dt / 2.0f;
        }

        for (size_t p = 0; p < POINTS_COUNT; p++)
        {
            g_result_file << points_y[p] << ",";
        }

        g_result_file << std::endl;
        printEnergies(velocities, points_y);

        i++;
    }
};

void Euler()
{
    std::vector<float> points_x(POINTS_COUNT, 0);
    std::vector<float> points_y(POINTS_COUNT, 0);
    std::vector<float> velocities(POINTS_COUNT, 0);
    std::vector<float> accelerations(POINTS_COUNT, 0);

    // t0
    float x = 0;
    for (size_t p = 0; p < POINTS_COUNT; p++)
    {
        points_x[p] = x;
        points_y[p] = sinf(x);

        x += dx;
    }

    for (size_t p = 0; p < POINTS_COUNT; p++)
    {
        g_result_file << points_y[p] << ",";
    }

    g_result_file << std::endl;

    int i = 0;
    while (i < MAX_ITERATIONS)
    {
        float y = 0;
        float v = 0;

        for (size_t p = 1; p < POINTS_COUNT - 1; p++)
        {
            accelerations[p] = (points_y[p + 1] - 2 * points_y[p] + points_y[p - 1]) / (dx * dx);
        }
        
        for (size_t p = 0; p < POINTS_COUNT; p++)
        {
            y = points_y[p] + velocities[p] * dt;
            v = velocities[p] + accelerations[p] * dt;
            points_y[p] = y;
            velocities[p] = v;
        }

        for (size_t p = 0; p < POINTS_COUNT; p++)
        {
            g_result_file << points_y[p] << ",";
        }

        g_result_file << std::endl;
        printEnergies(velocities, points_y);

        i++;
    }
};

int main(int argc, char const *argv[])
{
    mid_point();
    g_result_file.close();
    g_energies_file.close();
    return 0;
};
