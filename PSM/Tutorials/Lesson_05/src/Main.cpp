#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <cstdarg>

// Constants
#define G 6.66743e-11
#define dt 3170.0f
#define SCALE_MOON_DISTANCE 10.0f
#define SCALE_SUN_SYSTEM 1 / 10000.0f

// Mass:
#define SUN_MASS 1.989 * 1e30
#define EARTH_MASS 5.972 * 1e24
#define MOON_MASS 7.347 * 1e22

// Distance:
#define EARTH_MOON_DISTANCE 384400 * 1e3 //6371 * 1e3
#define SUN_EARTH_DISTANCE 1.5 * 1e11

// Acceleration:
#define EARTH_MOON_g G *EARTH_MASS / (EARTH_MOON_DISTANCE * EARTH_MOON_DISTANCE) //10.0f
#define SUN_EARTH_g G *SUN_MASS / (SUN_EARTH_DISTANCE * SUN_EARTH_DISTANCE)
#define MAX_ITTERATIONS 10010u

// Velocity:
#define INITIAL_MOON_VELOCITY sqrtf(EARTH_MOON_g *EARTH_MOON_DISTANCE)
#define INITIAL_EARTH_VELOCITY sqrtf(SUN_EARTH_g *SUN_EARTH_DISTANCE)

// Output files:
#define RESULT_FILE_EARTH "./res/results_Earth.csv"
#define RESULT_FILE_MOON "./res/results_Moon.csv"

std::ofstream g_result_Earth_file(RESULT_FILE_EARTH);
std::ofstream g_result_Moon_file(RESULT_FILE_MOON);

const std::string format(const char *fmt, ...)
{
    va_list args;
    char buff[100];
    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    return buff;
}

// Earth system:
struct EarthSystem
{
    DirectX::XMFLOAT3 earth_center{0.0f, 0.0f, 1.0f};
    DirectX::XMFLOAT3 earth_velocity{INITIAL_EARTH_VELOCITY, 0.0f, 0.0f};

    DirectX::XMFLOAT3 moon_center{0.0f, EARTH_MOON_DISTANCE, 1.0f};
    DirectX::XMFLOAT3 moon_velocity{INITIAL_MOON_VELOCITY, 0.0f, 0.0f};

    DirectX::XMFLOAT3X3 W{1.0f, 0.0f, 0.0f,
                          0.0f, 1.0f, 0.0f,
                          0.0f, SUN_EARTH_DISTANCE, 1.0f};
};

void mid_point()
{
    EarthSystem earth_system{};
    DirectX::XMVECTOR radiusVector{};
    DirectX::XMVECTOR unit{};

    float moon_velocity2X = 0;
    float moon_velocity2Y = 0;
    float earth_velocity2X = 0;
    float earth_velocity2Y = 0;

    std::cout << format("%.4f", 2 * DirectX::XM_PI * SUN_EARTH_DISTANCE / INITIAL_EARTH_VELOCITY) << std::endl;

    uint32_t i = 0;
    while (i < MAX_ITTERATIONS)
    {
        // EarthSystem:
        float xMoon = -earth_system.moon_center.x;
        float yMoon = -earth_system.moon_center.y;

        radiusVector = DirectX::XMVectorSet(xMoon, yMoon, 0.0f, 0.0f);
        unit = DirectX::XMVector2Normalize(radiusVector);

        float gf1 = EARTH_MOON_g;

        float AxMoon = DirectX::XMVectorGetX(unit) * EARTH_MOON_g;
        float AyMoon = DirectX::XMVectorGetY(unit) * EARTH_MOON_g;

        earth_system.moon_center.x += moon_velocity2X * dt;
        earth_system.moon_center.y += moon_velocity2Y * dt;

        moon_velocity2X = earth_system.moon_velocity.x + AxMoon * dt / 2.0f;
        moon_velocity2Y = earth_system.moon_velocity.y + AyMoon * dt / 2.0f;

        earth_system.moon_velocity.x += AxMoon * dt;
        earth_system.moon_velocity.y += AyMoon * dt;

        // SunSystem:
        float xEarth = -earth_system.W._31;
        float yEarth = -earth_system.W._32;

        radiusVector = DirectX::XMVectorSet(xEarth, yEarth, 0.0f, 0.0f);
        unit = DirectX::XMVector2Normalize(radiusVector);

        float gf2 = SUN_EARTH_g;

        float AxEarth = DirectX::XMVectorGetX(unit) * SUN_EARTH_g;
        float AyEarth = DirectX::XMVectorGetY(unit) * SUN_EARTH_g;

        earth_system.W._31 += earth_velocity2X * dt;
        earth_system.W._32 += earth_velocity2Y * dt;
        
        earth_velocity2X = earth_system.earth_velocity.x + AxEarth * dt / 2.0f;
        earth_velocity2Y = earth_system.earth_velocity.y + AyEarth * dt / 2.0f;

        earth_system.earth_velocity.x += AxEarth * dt;
        earth_system.earth_velocity.y += AyEarth * dt;

        DirectX::XMFLOAT3 moon_center_view{
            earth_system.moon_center.x * SCALE_MOON_DISTANCE,
            earth_system.moon_center.y * SCALE_MOON_DISTANCE,
            1.0f};


        DirectX::XMMATRIX w = XMLoadFloat3x3(&earth_system.W);
        DirectX::XMVECTOR R0 = XMLoadFloat3(&earth_system.earth_center);
        DirectX::XMVECTOR R1 = XMLoadFloat3(&moon_center_view);
        DirectX::XMVECTOR R2 = DirectX::XMVectorZero();
        DirectX::XMVECTOR R3 = DirectX::XMVectorZero();
        DirectX::XMMATRIX m(R0, R1, R2, R3);
        DirectX::XMMATRIX transformed = XMMatrixMultiply(m, w);
        DirectX::XMFLOAT3X3 result_p;
        DirectX::XMStoreFloat3x3(&result_p, transformed);

        g_result_Earth_file << result_p._11 * SCALE_SUN_SYSTEM << ", " << result_p._12 * SCALE_SUN_SYSTEM << std::endl;
        g_result_Moon_file << result_p._21 * SCALE_SUN_SYSTEM << ", " << result_p._22 * SCALE_SUN_SYSTEM << std::endl;

        ++i;
    }
};

int main(int argc, char const *argv[])
{
    if (!DirectX::XMVerifyCPUSupport())
    {
        std::cout << "directx math not supported" << std::endl;
        return 0;
    }

    mid_point();

    return 0;
};
