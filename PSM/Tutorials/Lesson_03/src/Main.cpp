#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>

using namespace std;

#define RESULT_FILE "./res/results.csv"
#define MAX_ITERATIONS 20

const double DT = 0.025f;
const double g = -10;
const double m = 1;
const double r = 1;

void mid_point();
void Runge_Kutta(int k);
pair<double, double> derivative(vector<double> &awe, pair<double, double> last_kakw, double dt);
vector<pair<double, double>> k1(vector<double> &initial_conditions);
vector<pair<double, double>> k2(vector<double> &initial_conditions);
vector<pair<double, double>> k4(vector<double> &initial_conditions);
void printEnergies(ofstream &ofs, double x, double y, double w);
double toRadians(double degree);

void Runge_Kutta(int k)
{
    std::ofstream result_file(RESULT_FILE);

    vector<pair<double, double>> ks{};
    vector<double> Y{toRadians(45), 0, 0};

    printEnergies(result_file,
                  r * cos(Y[0] - M_PI_2),
                  r * sin(Y[0] - M_PI_2),
                  Y[1]);

    int i = 0;
    while (i < MAX_ITERATIONS)
    {
        switch (k)
        {
        case 1:
            ks = k1(Y);
            Y[0] += (ks[0].first) * DT;
            Y[1] += (ks[0].second) * DT;
            break;

        case 2:
            ks = k2(Y);
            Y[0] += (ks[1].first) * DT;
            Y[1] += (ks[1].second) * DT;
            break;

        case 4:
            ks = k4(Y);
            Y[0] += ((ks[0].first + 2 * ks[1].first + 2 * ks[2].first + ks[3].first) / 6) * DT;
            Y[1] += ((ks[0].second + 2 * ks[1].second + 2 * ks[2].second + ks[3].second) / 6) * DT;
            break;
        default:
            cout << "Unknown K" << endl;
        }

        printEnergies(result_file,
                      r * cos(Y[0] - M_PI_2),
                      r * sin(Y[0] - M_PI_2),
                      Y[1]);
        i++;
    }
}

vector<pair<double, double>> k1(vector<double> &initial_conditions)
{
    pair<double, double> kakw{};
    vector<pair<double, double>> result{};

    kakw = derivative(initial_conditions, kakw, 0);
    result.push_back(kakw);

    return result;
}

vector<pair<double, double>> k2(vector<double> &initial_conditions)
{
    pair<double, double> kakw{};
    vector<pair<double, double>> result{};

    kakw = derivative(initial_conditions, kakw, 0);
    result.push_back(kakw);
    kakw = derivative(initial_conditions, kakw, DT / 2);
    result.push_back(kakw);

    return result;
}

vector<pair<double, double>> k4(vector<double> &initial_conditions)
{
    pair<double, double> kakw{};
    vector<pair<double, double>> result{};

    kakw = derivative(initial_conditions, kakw, 0);
    result.push_back(kakw);
    kakw = derivative(initial_conditions, kakw, DT / 2);
    result.push_back(kakw);
    kakw = derivative(initial_conditions, kakw, DT / 2);
    result.push_back(kakw);
    kakw = derivative(initial_conditions, kakw, DT);
    result.push_back(kakw);

    return result;
}

pair<double, double> derivative(vector<double> &awe, pair<double, double> last_Ks, double dt)
{
    pair<double, double> result{};

    double a2 = 0;
    double w2 = 0;
    double e2 = 0;

    w2 = awe[1] + last_Ks.second * dt;
    a2 = awe[0] + last_Ks.first * dt;
    e2 = g / r * sin(a2);

    result.first = w2;
    result.second = e2;

    return result;
}

void mid_point()
{
    std::ofstream result_file(RESULT_FILE);

    double a = toRadians(45);
    double a2 = 0;
    double da = 0;
    double w = 0;
    double w2 = 0;
    double dw = 0;
    double e = 0;
    double e2 = 0;

    printEnergies(result_file,
                  r * std::cos(a - M_PI_2),
                  r * std::sin(a - M_PI_2),
                  w);

    int i = 0;
    while (i < MAX_ITERATIONS)
    {
        e = g / r * sin(a);
        w2 = w + e * DT / 2;
        da = w2 * DT;

        a2 = a + w * DT / 2;
        e2 = g / r * sin(a2);
        dw = e2 * DT;

        a += da;
        w += dw;

        printEnergies(result_file,
                      r * cos(a - M_PI_2),
                      r * sin(a - M_PI_2),
                      w);

        i++;
    }
}

double toRadians(double degree)
{
    return degree * M_PI / 180;
}

void printEnergies(ofstream &ofs, double x, double y, double w)
{
    double Ep = abs(m * g * (r + y));
    double Ek = m * (w * w * r * r) / 2;

    ofs << Ep << ", " << Ek << ", " << Ep + Ek << endl;
}

int main(int argc, char const *argv[])
{
    Runge_Kutta(4);
    return 0;
}