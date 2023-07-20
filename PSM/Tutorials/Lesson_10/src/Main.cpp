#define _USE_MATH_DEFINES

#include <fstream>
#include <iostream>
#include <stack>
#include <map>
#include <cmath>

typedef struct
{
    float X;
    float Y;
    float Alpha; // radians
} Turtle;

using Command = void (*)(Turtle&);

void generate_tree(std::string &w, size_t n);
void present_tree(Turtle &turtle, std::string &w);
void rotate_left(Turtle &turtle);
void rotate_right(Turtle &turtle);
void move_forward(Turtle &turtle);
void push_to_stack(Turtle &turtle);
void pop_from_stack(Turtle &turtle);
constexpr float degreesToRadians(float d);

#define ITERATIONS_NUMBER 6
#define INITIAL_X 0.0f
#define INITIAL_Y 0.0f
#define INITIAL_ALPHA 25.0f  // degrees
#define ROTATION_ALPHA 25.0f // degrees
#define INITIAL_W 'X'

#define RESULT_FILE "./results.csv"
std::ofstream g_result_file(RESULT_FILE);

std::map<char, std::string> g_rules_map //
    {
        {'X', "F+[[X]-X]-F[-FX]+X"},
        {'F', "FF"} //
    };

std::map<char, Command> g_commands_map //
    {
        {'-', &rotate_left},
        {'+', &rotate_right},
        {'F', &move_forward},
        {'[', &push_to_stack},
        {']', &pop_from_stack} //
    };

std::stack<Turtle> g_stack{};

void generate_tree(std::string &w, size_t n)
{
    size_t i = 0;
    std::string tmp_str;
    while (i < n)
    {
        for (auto it = w.begin(); it != w.end(); ++it)
        {
            char current_symbol = *it;
            auto rule_it = g_rules_map.find(current_symbol);
            if (rule_it == g_rules_map.end())
            {
                tmp_str.append({current_symbol});
                continue;
            }

            tmp_str.append(rule_it->second);
        }

        w = tmp_str;
        tmp_str.clear();
        ++i;
    }

    return;
}

void present_tree(Turtle &turtle, std::string &w)
{
    g_result_file << turtle.X << ',' << turtle.Y << std::endl;
    for (auto it = w.begin(); it != w.end(); ++it)
    {
        char current_symbol = *it;
        auto cmd_it = g_commands_map.find(current_symbol);
        if (cmd_it == g_commands_map.end())
        {
            continue;
        }

        cmd_it->second(turtle);

        if (current_symbol == ']')
        {
            g_result_file << ',' << std::endl;
            g_result_file << turtle.X << ',' << turtle.Y << std::endl;
        }
        else if (current_symbol == 'F')
        {
            g_result_file << turtle.X << ',' << turtle.Y << std::endl;
        }
    }
}

void rotate_left(Turtle &turtle)
{
    turtle.Alpha += degreesToRadians(ROTATION_ALPHA);
};

void rotate_right(Turtle &turtle)
{
    turtle.Alpha -= degreesToRadians(ROTATION_ALPHA);
};

void move_forward(Turtle &turtle)
{
    turtle.X += cosf(turtle.Alpha);
    turtle.Y += sinf(turtle.Alpha);
};

void push_to_stack(Turtle &turtle)
{
    g_stack.push(turtle);
};

void pop_from_stack(Turtle &turtle)
{
    turtle = g_stack.top();
    g_stack.pop();
};

constexpr float degreesToRadians(float d)
{
    return d * M_PI / 180.0f;
}

int main(int argc, char const *argv[])
{
    std::string w = {INITIAL_W};
    Turtle v_turtle;
    v_turtle.X = INITIAL_X;
    v_turtle.Y = INITIAL_Y;
    v_turtle.Alpha = degreesToRadians(INITIAL_ALPHA);

    generate_tree(w, ITERATIONS_NUMBER);
    present_tree(v_turtle, w);

    return 0;
}
