#include "../include/layer.h"
using std::vector;

Layer::Layer(int num_kernels, int input_size, int input_depth,
             int pooling_size) {
  // makes n x n filter of homogenous value
  size = input_size;
  pooled_size = (input_size - 2) / pooling_size;
  psize = pooling_size;

  kernels = vector<vector<filter>>(num_kernels,
                                   vector<filter>(input_depth, filter(3)));
  data = vector<image>(num_kernels, image(size - 2, size - 2));
  pooled_data = vector<image>(num_kernels, image(pooled_size, pooled_size));
  bias = vector<double>(num_kernels);
  pooled_deltas = vector<image>(num_kernels, image(2, 2));
  std::random_device rd;
  std::mt19937 gen(rd());
  std::normal_distribution<> d(0, sqrt(2.0 / size));
  for (auto &val: bias)
       val = d(gen);
};

// Convolution
void Layer::correlate(const vector<image> img, int kernel) {
  for (int i = 1; i < size - 1; i++) {
    for (int j = 1; j < size - 1; j++) {
      for (int filter = 0; filter < kernels[kernel].size(); filter++) {
        for (int filter_i = -1; filter_i < 2; filter_i++) {
          for (int filter_j = -1; filter_j < 2; filter_j++) {
            data[kernel].entry[i - 1][j - 1] +=
                img[filter].entry[i + filter_i][j + filter_j] *
                kernels[kernel][filter].fweights[filter_i + 1][filter_j + 1];
          }
        }
      }
      data[kernel].entry[i - 1][j - 1] += bias[kernel];
    }
  }
  return;
}

// Convolution
void Layer::convolution(const vector<image> img, int kernel) {
  for (int i = 1; i < size - 1; i++) {
    for (int j = 1; j < size - 1; j++) {
      for (int filter = 0; filter < kernels[kernel].size(); filter++) {
        for (int filter_i = -1; filter_i < 2; filter_i++) {
          for (int filter_j = -1; filter_j < 2; filter_j++) {
            data[kernel].entry[i - 1][j - 1] +=
                img[filter].entry[i + filter_i][j + filter_j] *
                kernels[kernel][filter].fweights[-filter_i + 1][-filter_j + 1];
          }
        }
      }
      data[kernel].entry[i - 1][j - 1] += bias[kernel];
    }
  }
  return;
}

// Pooling
void Layer::maxPool2d(bool training, double dropout_rate) {
  if (psize == 1) {
    for (int image = 0; image < data.size(); image++) {
      for (int i = 0; i < pooled_size; i++) {
        for (int j = 0; j < pooled_size; j++) {
          pooled_data[image].entry[i][j] = data[image].entry[i][j];
        }
      }
    }
  }
  for (int image = 0; image < data.size(); image++) {
    for (int i = 0; i < pooled_size; i++) {
      for (int j = 0; j < pooled_size; j++) {
        double max = -99999.0;
        int idx_i = 0;
        int idx_j = 0;
        for (int k = 0; k < psize; k++) {
          for (int w = 0; w < psize; w++) {
            if (data[image].entry[i * psize + k][j * psize + w] > max) {
              max = data[image].entry[i * psize + k][j * psize + w];
              idx_i = k;
              idx_j = w;
            }
          }
        }
        pooled_data[image].entry[i][j] = max;
        pooled_deltas[image].entry[idx_i][idx_j] = 0;
      }
    }
  }
  if (training == true)
    applyDropout(dropout_rate);
  return;
}

void Layer::applyDropout(double dropout_rate) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distro{0.0, 1.0};
  double scale = 1.0 - dropout_rate;
  for (int kernel = 0; kernel < kernels.size(); kernel++) {
    for (int image = 0; image < kernels[kernel].size(); image++) {
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
}
