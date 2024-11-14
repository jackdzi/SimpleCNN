#include "../include/model.h"

Model::Model(vector<int> kernel_sizes, vector<int> input_sizes,
             vector<int> filters, vector<int> cLayer_sizes,
             vector<int> filter_type, vector<int> pooling_size)
    : fully_connected(Classification(
          cLayer_sizes, std::pow(kernel_sizes[kernel_sizes.size() - 1] /
                                     pooling_size[pooling_size.size() - 1],
                                 2))) {
  num_layers = kernel_sizes.size();
  for (int i = 0; i < num_layers; i++) {
    layers.push_back(
        Layer(kernel_sizes[i], input_sizes[i], filters[i], pooling_size[i], 3));
  };
}

vector<double> Model::forwardPropagate(vector<image> input, bool testing) {
  for (int i = 0; i < num_layers; i++) {
    for (int kernels = 0; kernels < layers[i].kernels.size(); kernels++) {
      if (i == 0) {
        layers[i].correlate(input, kernels);
      } else {
        layers[i].correlate(layers[i-1].pooled_data, kernels);
        layers[i].maxPool2d(testing, 0.0);
      }
    }
  }
  return fully_connected.forwardPass(layers[num_layers - 1].pooled_data,
                                     layers[num_layers - 1].pooled_size);
}
