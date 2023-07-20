#include <iostream>
#include <fstream>

struct Vector2
{
    float x;
    float y;
};

const float dt = 0.1f;
const float g = -10;
const float m = 0.60;
const float k = 1.05f;

std::ostream &operator<<(std::ostream &os, Vector2 v)
{
    os << "x: " << v.x << " y: " << v.y << std::endl;
    return os;
}

void Euler()
{
    std::ofstream result_file("./res/results.csv");

    Vector2 position = {0, 0};
    Vector2 delta_position = {0, 0};
    Vector2 velocity = {10, 10};
    Vector2 delta_velocity = {};
    Vector2 acceleration = {};

    result_file << position.x << ", " << position.y << std::endl;
    while (position.y >= 0)
    {
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        acceleration.x = (m * 0 - k * velocity.x) / m;
        acceleration.y = (m * g - k * velocity.y) / m;

        velocity.x += acceleration.x * dt;
        velocity.y += acceleration.y * dt;
        result_file << position.x << ", " << position.y << std::endl;
    }
}

void mid_point()
{
    std::ofstream result_file("./res/results.csv");

    Vector2 position = {0, 0};
    Vector2 delta_position = {0, 0};
    Vector2 velocity = {10, 10};
    Vector2 velocity2 = {10, 10};
    Vector2 delta_velocity = {};
    Vector2 acceleration = {};
    Vector2 acceleration2 = {};
    acceleration.x = (m * 0 - k * velocity.x) / m;
    acceleration.y = (m * g - k * velocity.y) / m;

    while (position.y >= 0)
    {
        position.x += delta_position.x;
        position.y += delta_position.y;

        velocity.x += delta_velocity.x;
        velocity.y += delta_velocity.y;

        acceleration.x = (m * 0 - k * velocity.x) / m;
        acceleration.y = (m * g - k * velocity.y) / m;

        velocity2.x = velocity.x + acceleration.x * dt / 2.0f;
        velocity2.y = velocity.y + acceleration.y * dt / 2.0f;

        acceleration2.x = (m * 0 - k * velocity2.x) / m;
        acceleration2.y = (m * g - k * velocity2.y) / m;

        delta_position.x = velocity2.x * dt;
        delta_position.y = velocity2.y * dt;

        delta_velocity.x = acceleration2.x * dt;
        delta_velocity.y = acceleration2.y * dt;

        result_file << position.x << ", " << position.y << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    mid_point();

    return 0;
}
