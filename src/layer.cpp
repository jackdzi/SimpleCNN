#include "../include/layer.h"
using std::vector;

Layer::Layer(int filter_param, int input_size, int pooling_size) {
  // makes n x n filter of homogenous value
  size = input_size;
  pooled_size = input_size / pooling_size;
  filter = vector<vector<double>>(filter_param, vector<double>(filter_param, 1));
  data = vector<vector<double>>(size, vector<double>(size, 1));
  pooled_data = vector<vector<double>>(pooled_size, vector<double>(pooled_size, 1));
};

// use when loading in image from matrix
void Layer::loadFromImages(const std::vector<double> input) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      data[i][j] = input[i * size + j];
    };
  };
  return;
};

//different handling when using 2d array as image vs image from matrix
void Layer::loadIntoLayer(const std::vector<std::vector<double>> input) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      data[i][j] = input[i][j];
    };
  };
  return;
};

//Padding
vector<vector<double>> Layer::padding(int n) {
  vector<vector<double>> padded = vector<vector<double>>(size + 2*n, vector<double>(size + 2*n, 0));
  for (int i = 0; i < size ; i++) {
    for (int j = 0; j < size ; j++) {
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
  vector<vector<double>> res = vector<vector<double>>(size, vector<double>(size, 0));
  for (int i = padding; i < size; i++) {
    for (int j = padding; j < size; j++) {
      double sum = 0.0;
      for (int k = 0; k < FILTER_SIZE; k++) {
        for (int w = 0; w < FILTER_SIZE; w++) {
          sum += filter[k][w] * padded[i - 1 + k][j - 1 + w]; //WARN: Change when FILTER_SIZE changes; Padded array is the segfault issue
        };
      };
      // res is still size x size and goes OOB when just i and j used
      res[i][j] = sum;

    };
  };
  loadIntoLayer(res); // use other load function on any 2d array
  return;
}
// Pooling
void Layer::maxPool2d(bool train_mode, double dropout_rate) {
  for (int i = 0; i < pooled_size; i++) {
    for (int j = 0 ; j < pooled_size; j++) {
      double max = 0.0;

      for (int k = 0; k < 2; k++) {
        for (int w = 0; w < 2; w++) {
          if (data[i * 2 + k][j * 2 + w] > max) {
            max = data[i * 2 + k][j * 2 + w];
          };
        };
      pooled_data[i][j] = max;
      };
    };
  };
  setTrainingMode(train_mode);
  initializeDropoutMask(pooled_size, pooled_size);
  applyDropout(dropout_rate);
  return;
}

// Dropout
void Layer::setTrainingMode(bool mode) {
  is_training = mode;
}

// Initialize Dropout Mask based on current data size
void Layer::initializeDropoutMask(int rows, int cols) {
  dropout_mask = std::vector<std::vector<bool>>(rows, std::vector<bool>(cols, true));
}

// Dropout rate of pixels, set to 0.0 to disable. 
void Layer::applyDropout(double dropout_rate) {
  if (!is_training) {
    double scale = 1.0 - dropout_rate;
    for (auto& row : pooled_data) {
      for (auto& elem : row) {
        elem *= scale;
      }
    }
    return;
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distro{0.0, 1.0};

  for (int i = 0; i < pooled_size; i++) {
    for (int j = 0; j < pooled_size; j++) {
      double rand = distro(gen);
      if (rand < dropout_rate) {
        dropout_mask[i][j] = false;
        pooled_data[i][j] = 0;
      }
      else
      {
        dropout_mask[i][j] = true;
        pooled_data[i][j] /= (1.0 - dropout_rate);
      }
    }
  }
}