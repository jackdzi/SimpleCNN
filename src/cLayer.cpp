#include "../include/cLayer.h"


cLayer::cLayer(int num_nodes, int prev_nodes) : 
size(num_nodes), data(std::vector<double>(size, 0)), weights(std::vector<std::vector<double>>(size, std::vector<double>(prev_nodes, 0))) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> d(0, sqrt(2.0/prev_nodes)); // He method of initialization

  for (auto &row : weights) {
    for (auto &weight : row) {
      weight = d(gen);
    }
  }
}

void cLayer::loadFromLayer(const std::vector<vector<double>> input, int dim_prev) {
  for (int i = 0; i < dim_prev; i++) {
    for (int j = 0; j < dim_prev; j++) {
      data[i * 7 + j] = input[i][j];
    };
  };
  return;
}

void cLayer::loadFromPrevious(cLayer input) {
  for (int i = 0; i < size; i++) {
    double sum = 0;
    for (int j = 0; j < input.size; j++) {
      sum += input.activation[i] * weights[i][j];
    };
    data[i] = sum;
  };
  return;
}

int cLayer::applyActivation(int application_type) {
  auto relu = [](double x) {
    return x > 0 ? x : 0.0;
  };
  double denom = std::accumulate(data.begin(), data.end(), 0.0,
                                 [](double sum, double x) {return sum + std::exp(x);});
  auto softmax = [denom](double x) {
    return std::exp(x) / denom;
  };
  std::function<double(double)> some_function;
  if (application_type == 0)
    some_function = relu;
  else if (application_type == 1)
    some_function = softmax;
  else
    return 1;
  for (int i = 0; i < size; i++){
    data[i] = some_function(data[i]);
  }
  return 0;
}

