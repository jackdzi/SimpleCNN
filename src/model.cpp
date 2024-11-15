#include "../include/model.h"
#include <algorithm>

Model::Model(vector<int> kernel_sizes, vector<int> input_sizes,
             vector<int> filters, vector<int> cLayer_sizes,
             vector<int> pooling_size)
    : fully_connected(Classification(
          cLayer_sizes, kernel_sizes[kernel_sizes.size() - 1] *
                            (pow((input_sizes[input_sizes.size() - 1] - 2) /
                                     pooling_size[pooling_size.size() - 1],
                                 2)))) {
  num_layers = kernel_sizes.size();
  for (int layer = 0; layer < num_layers; layer++) {
    layers.push_back(
        Layer(kernel_sizes[layer], input_sizes[layer], filters[layer], pooling_size[layer]));
  };
}

vector<int> Model::selectRandomIndices(int batch_size, int data_size) {
  vector<int> indices(data_size);
  for (int i = 0; i < data_size; i++)
    indices[i] = i;

  std::random_device rd;
  std::mt19937 gen(rd());
  std::shuffle(indices.begin(), indices.end(), gen);

  indices.resize(batch_size);
  return indices;
}

vector<vector<double>> Model::forwardPropagate(vector<image> input,
                                               bool training) {
  vector<vector<double>> probabilities;
  for (int img = 0; img < input.size(); img++) {
    for (int i = 0; i < num_layers; i++) {
      for (int kernel = 0; kernel < layers[i].kernels.size(); kernel++) {
        if (i == 0)
          layers[i].correlate(vec(input[img]), kernel);
        else
          layers[i].correlate(layers[i - 1].pooled_data, kernel);
      }
      layers[i].maxPool2d(training, 0.01);
    }
    probabilities.push_back(fully_connected.forwardPass(layers[layers.size() - 1].pooled_data));
  }
  return probabilities;
}

void Model::backwardPropagate(vector<image> training, double learn) {
  vector<double> final_errors(training.size());


}

void Model::trainModel(double learn, int batch_size, int epoches,
                       const vector<image> training) {
  for (int epoch = 0; epoch < epoches; epoch++) {
    if (epoch % 15 == 0 && epoch != 0)
      learn = learn * 0.9;
    vector<image> batch;
    auto indicies = selectRandomIndices(batch_size, training.size());
    for (int i = 0; i < batch_size; i++)
      batch.push_back(training[indicies[i]]);
    backwardPropagate(batch, learn);
  }
}
