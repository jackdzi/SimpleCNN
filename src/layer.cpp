#include "../include/layer.h"
using std::vector;

Layer::Layer(int filter_param) {
  // TODO: Initialize filter to specific distribution
  data = vector<vector<double>>(28, vector<double>(28, 0));
};
void Layer::load(const std::vector<std::vector<double>> input) {
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      data[i][j] = input[i][j];
    };
  };
  return;
};
//Padding
vector<vector<double>> Layer::padding(int n) {
  vector<vector<double>> padded = vector<vector<double>>(28 + 2*n, vector<double>(28 + 2*n, 0));
  for (int i = n; i < 28 + n; i++) {
    for (int j = n; j < 28 + n; j++) {
      padded[i][j] = data[i][j];
    };
  };
  return padded;
}
// Convolution
void Layer::convolve() {
  int padding = 2;
  vector<vector<double>> padded = Layer::padding(padding);
  vector<vector<double>> res = vector<vector<double>>(28, vector<double>(28, 0));
  for (int i = padding; i < 28 + padding; i++) {
    for (int j = padding; j < 28 + padding; j++) {
      double sum = 0.0;
      for (int k = 0; k < FILTER_SIZE; k++) {
        for (int w = 0; w < FILTER_SIZE; w++) {
          sum += filter[k][w] * padded[i - 1 + k][j - 1 + w]; //WARN: Change when FILTER_SIZE changes
        };
      };
      res[i][j] = sum;
    };
  };
  load(res);
  return;
}
// Pooling
//TODO: Implement pooling
