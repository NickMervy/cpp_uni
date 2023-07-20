#include <DirectXMath.h>
#include <fstream>
#include <iostream>

using namespace DirectX;

#define dt 0.05f
#define h 20.0f
#define r 2.0f
#define alpha XM_PIDIV4
#define g 10
#define m 1
#define I_COEF /* 2.0f / 5.0f //ball*/ 2.0f / 3.0f //sphere

#define MAX_ITTERATIONS 75u
#define RESULT__FILE_TRANSLATION "./res/results_translation.csv"
#define RESULT_FILE_ROTATION "./res/results_rotation.csv"
#define RESULT_FILE_ENERGIES "./res/results_energies.csv"

std::ofstream result_translation(RESULT__FILE_TRANSLATION);
std::ofstream result_rotation(RESULT_FILE_ROTATION);
std::ofstream result_energies(RESULT_FILE_ENERGIES);

struct Object
{
    float radius = r;
    float beta = 0.0f;
    float omega = 0.0f;
    float a = g * sinf(alpha) / (1 + I_COEF);
    float eps = a / radius;

    XMFLOAT3 center{0.0f, radius, 1.0f};
    XMFLOAT3 trajectory{0.0f, 0.0f, 1.0f};
    XMFLOAT3 velocity{0.0f, 0.0f, 0.0f};

    XMFLOAT3X3 W{cosf(-alpha), sinf(-alpha), 0.0f,
                 -sinf(-alpha), cosf(-alpha), 0.0f,
                 0.0f, h, 1.0f};
};

void printEnergies(const Object &obj, float world_y)
{
    float Ep = m * g * world_y;
    float Ek = m * obj.velocity.x * obj.velocity.x / 2.0f + I_COEF * m * obj.radius * obj.radius * obj.omega * obj.omega / 2.0f;
    float E = Ep + Ek;

    result_energies << Ep << ", " << Ek << ", " << E << std::endl;
};

void Euler(Object &obj)
{
    uint32_t i = 0;
    while (i < MAX_ITTERATIONS)
    {
        // Local system transformations
        obj.trajectory.x = obj.center.x + obj.radius * std::sin(obj.beta - alpha);
        obj.trajectory.y = obj.radius + obj.radius * std::cos(obj.beta - alpha);

        obj.beta += obj.omega * dt;
        obj.omega += obj.eps * dt;

        obj.center.x += obj.velocity.x * dt;
        obj.velocity.x += obj.a * dt;

        XMMATRIX ball_w = XMLoadFloat3x3(&obj.W);
        XMVECTOR R0 = XMLoadFloat3(&obj.center);
        XMVECTOR R1 = XMLoadFloat3(&obj.trajectory);
        XMVECTOR R2 = XMLoadFloat3(&obj.trajectory);
        XMVECTOR R3 = XMLoadFloat3(&obj.trajectory);
        XMMATRIX ball_verts(R0, R1, R2, R3);
        XMMATRIX ball_transformed = XMMatrixMultiply(ball_verts, ball_w);
        XMFLOAT3X3 result_p;
        XMStoreFloat3x3(&result_p, ball_transformed);

        result_translation << result_p._11 << ", " << result_p._12 << std::endl;
        result_rotation << result_p._21 << ", " << result_p._22 << std::endl;
        printEnergies(obj, result_p._12);
        ++i;
    }
};

void mid_point(Object &obj)
{
    float v2;
    float o2;
    uint32_t i = 0;
    while (i < MAX_ITTERATIONS)
    {
        // Local system transformations
        obj.trajectory.x = obj.center.x + obj.radius * std::sin(obj.beta - alpha);
        obj.trajectory.y = obj.radius + obj.radius * std::cos(obj.beta - alpha);

        v2 = obj.velocity.x + obj.a * dt / 2;
        obj.center.x += v2 * dt;
        obj.velocity.x += obj.a * dt;

        o2 = obj.omega + obj.eps * dt / 2;
        obj.beta += o2 * dt;
        obj.omega += obj.eps * dt;

        XMMATRIX ball_w = XMLoadFloat3x3(&obj.W);
        XMVECTOR R0 = XMLoadFloat3(&obj.center);
        XMVECTOR R1 = XMLoadFloat3(&obj.trajectory);
        XMVECTOR R2 = XMLoadFloat3(&obj.trajectory);
        XMVECTOR R3 = XMLoadFloat3(&obj.trajectory);
        XMMATRIX ball_verts(R0, R1, R2, R3);
        XMMATRIX ball_transformed = XMMatrixMultiply(ball_verts, ball_w);
        XMFLOAT3X3 result_p;
        XMStoreFloat3x3(&result_p, ball_transformed);

        result_translation << result_p._11 << ", " << result_p._12 << std::endl;
        result_rotation << result_p._21 << ", " << result_p._22 << std::endl;
        printEnergies(obj, result_p._12);
        ++i;
    }
};

int main(int argc, char const *argv[])
{
    if (!XMVerifyCPUSupport())
    {
        std::cout << "directx math not supported" << std::endl;
        return 0;
    }

    Object obj{};
    // Euler(obj);
    mid_point(obj);

    return 0;
};
