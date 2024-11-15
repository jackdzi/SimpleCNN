#include "../include/cLayer.h"

cLayer::cLayer(int num_nodes, int prev_nodes)
    : size(num_nodes), data(vector<double>(size, 0)),
      activation(vector<double>(size, 0)),
      bias(vector<double>(size, 0)),
      weights(vector<vector<double>>(prev_nodes, vector<double>(size, 0))) {
  // data[i][j] is jth input node and
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> d(
      0, sqrt(2.0 / prev_nodes)); // He method of initialization

  for (auto &row : weights) {
    for (auto &weight : row) {
      weight = d(gen);
    }
  }
  for (auto &val: bias)
    val = d(gen);
}

void cLayer::loadFromLayer(vector<image> pooled) {
  for (int node = 0; node < size; node++) {
    double sum = 0;

    for (int image = 0; image < pooled.size(); image++) {
      for (int i = 0; i < pooled[image].entry.size(); i++) {
        for (int j = 0; j < pooled[image].entry[i].size(); j++) {
          sum += pooled[image].entry[i][j] * weights[image * pooled[image].entry.size() * pooled[image].entry[i].size() + i * pooled[image].entry.size() + j][node];
        }
      }
    }
    data[node] = sum + bias[node];
  }
}

void cLayer::loadFromPrevious(cLayer input) {
  for (int node = 0; node < size; node++) {
    double sum = 0;
    for (int input_node = 0; input_node < input.size; input_node++) {
      sum += input.activation[input_node] * weights[input_node][node];
    };
    data[node] = sum;
  };
  return;
}

int cLayer::applyActivation(int application_type) {
  std::function<double(double)> some_function;
  if (application_type == 0)
    some_function = [](double x) { return x > 0 ? x : 0.0; };
  else if (application_type == 1) {
    double maximum = -999999;
    for (double num: data) {
      if (num > maximum)
        maximum = num;
    }
    double denom =
        std::accumulate(data.begin(), data.end(), 0.0,
                        [maximum](double sum, double x) { return sum + std::exp(x-maximum); });
    some_function = [denom, maximum](double x) { return std::exp(x-maximum) / denom; };
  } else
    return 1;
  for (int i = 0; i < size; i++) {
    activation[i] = some_function(data[i]);
  }
  return 0;
}
