#include "../include/model.h"

Model::Model(vector<int> kernel_sizes, vector<int> input_sizes,
             vector<int> filters, vector<int> cLayer_sizes,
             vector<int> pooling_size)
    : fully_connected(Classification(
          cLayer_sizes, kernel_sizes[kernel_sizes.size() - 1] *
                            (pow((input_sizes[input_sizes.size() - 1] - 2) /
                             pooling_size[pooling_size.size() - 1], 2)))) {
  num_layers = kernel_sizes.size();
  for (int i = 0; i < num_layers; i++) {
    layers.push_back(
        Layer(kernel_sizes[i], input_sizes[i], filters[i], pooling_size[i]));
  };
}

vector<double> Model::forwardPropagate(vector<image> input, bool testing) {
  for (int i = 0; i < num_layers; i++) {
    for (int kernel = 0; kernel < layers[i].kernels.size(); kernel++) {
      if (i == 0) {
        layers[i].correlate(input, kernel);
      } else {
        layers[i].correlate(layers[i - 1].pooled_data, kernel);
      }
    }
    layers[i].maxPool2d(testing, 0.0);
  }
  return fully_connected.forwardPass(layers[layers.size() - 1].pooled_data);
}
