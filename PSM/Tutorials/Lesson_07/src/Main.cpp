#include <iostream>
#include <Eigen/Dense>
#include <fstream>

using Eigen::Inverse;
using Eigen::MatrixXd;
using Eigen::VectorXd;

#define M 42
#define N 42
#define UNKNOWN_M (M - 2)
#define UNKNOWN_N (N - 2)
#define X_SIZE UNKNOWN_M *UNKNOWN_N

#define TOP_TEMPERATURE 200
#define RIGHT_TEMPERATURE 50
#define BOT_TEMPERATURE 150
#define LEFT_TEMPERATURE 100

#define RESULT_FILE "./res/results.csv"
std::ofstream g_result_file(RESULT_FILE);

int main()
{
  std::unique_ptr<MatrixXd> finallMatrix = std::make_unique<MatrixXd>(M, N);
  finallMatrix->setZero();
  std::unique_ptr<MatrixXd> coefMatrix = std::make_unique<MatrixXd>(X_SIZE, X_SIZE);
  coefMatrix->setZero();
  std::unique_ptr<VectorXd> xVector = std::make_unique<VectorXd>(X_SIZE);
  std::unique_ptr<VectorXd> bVector = std::make_unique<VectorXd>(X_SIZE);

  // top
  for (size_t n = 1; n < N - 1; n++)
  {
    (*finallMatrix)(0, n) = TOP_TEMPERATURE;
  }

  // right
  for (size_t m = 1; m < M - 1; m++)
  {
    (*finallMatrix)(m, N - 1) = RIGHT_TEMPERATURE;
  }

  // bot
  for (size_t n = 1; n < N - 1; n++)
  {
    (*finallMatrix)(M - 1, n) = BOT_TEMPERATURE;
  }

  // left
  for (size_t m = 1; m < M - 1; m++)
  {
    (*finallMatrix)(m, 0) = LEFT_TEMPERATURE;
  }

  // std::cout << "Initial matrix:" << std::endl;
  // std::cout << (*finallMatrix) << std::endl
  //           << std::endl;

  // generate coefficient matrix
  for (size_t m = 0; m < coefMatrix->rows(); m++)
  {
    double value = 0;
    for (size_t n = 0; n < coefMatrix->cols(); n++)
    {
      if (m == n)
      {
        value = -4;
      }

      else if (n == m - UNKNOWN_N ||
               n == m + UNKNOWN_N ||
               (m - 1 == n && m % UNKNOWN_N != 0) ||
               (m + 1 == n && m % UNKNOWN_N != UNKNOWN_N - 1))
      {
        value = 1;
      }

      else
      {
        value = 0;
      }

      (*coefMatrix)(m, n) = value;
    }

    // left-top corner
    if (m == 0)
    {
      (*bVector)(m) = -TOP_TEMPERATURE - LEFT_TEMPERATURE;
    }

    // right-top corner
    else if (m == UNKNOWN_N - 1)
    {
      (*bVector)(m) = -TOP_TEMPERATURE - RIGHT_TEMPERATURE;
    }

    // right-bot corner
    else if (m == X_SIZE - 1)
    {
      (*bVector)(m) = -BOT_TEMPERATURE - RIGHT_TEMPERATURE;
    }

    // left-bot corner
    else if (m == X_SIZE - UNKNOWN_N)
    {
      (*bVector)(m) = -BOT_TEMPERATURE - LEFT_TEMPERATURE;
    }

    // top edge
    else if (m > 0 && m < UNKNOWN_N - 1)
    {
      (*bVector)(m) = -TOP_TEMPERATURE;
    }

    // right edge
    else if (m % UNKNOWN_N == UNKNOWN_N - 1)
    {
      (*bVector)(m) = -RIGHT_TEMPERATURE;
    }

    // bottom edge
    else if (m > X_SIZE - UNKNOWN_N)
    {
      (*bVector)(m) = -BOT_TEMPERATURE;
    }

    // left edge
    else if (m % UNKNOWN_N == 0)
    {
      (*bVector)(m) = -LEFT_TEMPERATURE;
    }

    else
    {
      (*bVector)(m) = 0;
    }
  }

  // std::cout << "Coefficient matrix:" << std::endl;
  // std::cout << *coefMatrix << std::endl
  //           << std::endl;

  // std::cout << "B vector:" << std::endl;
  // std::cout << *bVector << std::endl
  //           << std::endl;

  std::unique_ptr<Inverse<MatrixXd>> inverseMatrix = std::make_unique<Inverse<MatrixXd>>(coefMatrix->inverse());
  *xVector = (*inverseMatrix) * (*bVector);

  // std::cout << "X vector:" << std::endl;
  // std::cout << *xVector << std::endl
  //           << std::endl;

  int counter = 1;
  for (size_t m = 0; m < xVector->size(); m++)
  {
    if (m != 0 && m % UNKNOWN_N == 0)
    {
      counter++;
    }

    (*finallMatrix)(counter, m % UNKNOWN_N + 1) = (*xVector)(m);
  }

  // std::cout << "Finall result:" << std::endl;
  // std::cout << *finallMatrix << std::endl
  //           << std::endl;

  for (size_t m = 0; m < finallMatrix->rows(); m++)
  {
    for (size_t n = 0; n < finallMatrix->cols(); n++)
    {
      if (n == finallMatrix->cols() - 1)
      {
        g_result_file << (*finallMatrix)(m,n);
        break;
      }

      g_result_file << (*finallMatrix)(m,n) << ",";
    }

    g_result_file << std::endl;
  }
}