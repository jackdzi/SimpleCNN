#include "../include/layer.h"
using std::vector;

Layer::Layer(int filter_param) {
  // makes n x n filter of homogenous value
  filter = vector<vector<double>>(filter_param, vector<double>(filter_param, 1));
  data = vector<vector<double>>(28, vector<double>(28, 1));
  pooledData = vector<vector<double>>(14, vector<double>(14, 1));
};

// use when loading in image from matrix
void Layer::loadFromImages(const std::vector<double> input) {
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      data[i][j] = input[i * 28 + j];
    };
  };
  return;
};

//different handling when using 2d array as image vs image from matrix
void Layer::loadIntoLayer(const std::vector<std::vector<double>> input) {
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
  for (int i = 0; i < 28 ; i++) {
    for (int j = 0; j < 28 ; j++) {
      // Fixed indexing issues and undefined filter issue
      padded[i + n][j + n] = data[i][j];
    };
  };
  return padded;
}
// Convolution
void Layer::convolve() {
  int padding = 1;
  vector<vector<double>> padded = Layer::padding(padding);
  vector<vector<double>> res = vector<vector<double>>(28, vector<double>(28, 0));
  for (int i = padding; i < 28; i++) {
    for (int j = padding; j < 28; j++) {
      double sum = 0.0;
      for (int k = 0; k < FILTER_SIZE; k++) {
        for (int w = 0; w < FILTER_SIZE; w++) {
          sum += filter[k][w] * padded[i - 1 + k][j - 1 + w]; //WARN: Change when FILTER_SIZE changes Padded array is the segfault issue
        };
      };
      // res is still 28 x 28 and goes OOB when just i and j used
      res[i][j] = sum;

    };
  };
  loadIntoLayer(res); // use other load function on any 2d array
  return;
}
// Pooling
void Layer::maxPool2d() {

  for (int i = 0; i < 14; i++) {
    for (int j = 0 ; j < 14; j++) {
      double max = 0.0;

      for (int k = 0; k < 2; k++) {
        for (int w = 0; w < 2; w++) {
          if (data[i * 2 + k][j * 2 + w] > max) {
            max = data[i * 2 + k][j * 2 + w];
          };
        };
      pooledData[i][j] = max;
      };
    };
  };
  return;

}
