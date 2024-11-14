#include "../include/layer.h"
using std::vector;

Layer::Layer(int num_kernels, int input_size, int input_depth, int pooling_size,
             int filter_size) {
  // makes n x n filter of homogenous value
  size = input_size;
  pooled_size = input_size / pooling_size;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> d(0, sqrt(2.0 / filter_size));
  kernels = vector<vector<filter>>(
      num_kernels, vector<filter>(input_depth, filter(filter_size, gen, d)));
  data = vector<image>(input_depth, image(input_size, input_size));
  pooled_data = vector<image>(input_depth, image(input_size, input_size));
};

// Convolution
void Layer::correlate(const vector<image> img, int kernel) {
  for (int i = 1; i < input_size - 1; i++) {
    for (int j = 1; j < input_size - 1; j++) {
      for (int filter_i = -1; filter_i < 2; filter_i++) {
        for (int filter_j = -1; filter_j < 2; filter_j++) {
          for (int filter = 0; filter < num_filters; filter++) {
            data[kernel].entry[i - 1][j - 1] +=
                img[filter].entry[i + filter_i][j + filter_j] *
                kernels[kernel][filter].fweights[filter_i + 1][filter_j + 1];
          }
        }
      }
      data[kernel].entry[i - 1][j - 1] += bias[kernel].fweights[i - 1][i - 1];
    }
  }
  return;
}

// Convolution
void Layer::convolution(const vector<image> img, int kernel) {
  for (int i = 1; i < input_size - 1; i++) {
    for (int j = 1; j < input_size - 1; j++) {
      for (int filter_i = -1; filter_i < 2; filter_i++) {
        for (int filter_j = -1; filter_j < 2; filter_j++) {
          for (int filter = 0; filter < num_filters; filter++) {
            data[kernel].entry[i - 1][j - 1] +=
                img[filter].entry[i + filter_i][j + filter_j] *
                kernels[kernel][filter].fweights[-filter_i + 1][-filter_j + 1];
          }
        }
      }
      data[kernel].entry[i - 1][j - 1] += bias[kernel].fweights[i - 1][i - 1];
    }
  }
  return;
}

// Pooling
void Layer::maxPool2d(bool train_mode, double dropout_rate) {
  for (int image = 0; image < num_filters; image++) {
    for (int i = 0; i < pooled_size; i++) {
      for (int j = 0; j < pooled_size; j++) {
        double max = 0.0;

        for (int k = 0; k < 2; k++) {
          for (int w = 0; w < 2; w++) {
            if (data[image].entry[i * 2 + k][j * 2 + w] > max)
              max = data[image].entry[i * 2 + k][j * 2 + w];
          }
        }
        pooled_data[image].entry[i][j] = max;
      }
    }
  }
  if (train_mode == true)
    applyDropout(dropout_rate);
  return;
}

void Layer::applyDropout(double dropout_rate) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distro{0.0, 1.0};
  double scale = 1.0 - dropout_rate;
  for (int image = 0; image < num_filters; image++) {
    for (int i = 0; i < pooled_size; i++) {
      for (int j = 0; j < pooled_size; j++) {
        double rand = distro(gen);
        if (rand < dropout_rate)
          pooled_data[image].entry[i][j] = 0;
        else
          pooled_data[image].entry[i][j] /= scale;
      }
    }
  }
}
