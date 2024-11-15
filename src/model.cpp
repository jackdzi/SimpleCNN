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
    layers.push_back(Layer(kernel_sizes[layer], input_sizes[layer],
                           filters[layer], pooling_size[layer]));
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
    probabilities.push_back(
        fully_connected.forwardPass(layers[layers.size() - 1].pooled_data));
  }
  return probabilities;
}

void Model::backwardPropagate(vector<image> training, vector<double> labels,
                              double learn) {
  vector<double> final_errors(10, 0.0);
  auto probabilities = forwardPropagate(training, true);
  for (int img = 0; img < training.size(); img++) {
    for (int prob = 0; prob < 10; prob++) {
      int target = prob == labels[img] ? 1 : 0;
      final_errors[prob] += probabilities[img][prob] - target;
    }
  }

  for (int prob = 0; prob < 10; prob++)
    final_errors[prob] /= training.size();

  vector<vector<double>> deltas;
  deltas.push_back(final_errors);
  
  for (int clayer = fully_connected.size - 2; clayer >= 0; clayer--) {

    int current_layer_size = fully_connected.connected[clayer].size;
    int next_layer_size = fully_connected.connected[clayer + 1].size;
    vector<double> node_deltas(current_layer_size, 0.0);
    
    for (int node = 0; node < current_layer_size; node++) {
      for (int prev_node = 0;
           prev_node < next_layer_size;
           prev_node++) {
        node_deltas[node] +=
            deltas[0][prev_node] *
            fully_connected.connected[clayer + 1].weights[node][prev_node];
      }
      int activation_derivative =
          fully_connected.connected[clayer].data[node] > 0 ? 1 : 0;
      node_deltas[node] *= activation_derivative;
    }
    deltas.insert(deltas.begin(), node_deltas);
  }
  // For each layer, go over every batch in image
  for (int clayer = 0; clayer < fully_connected.size; clayer++) {
    for (int img = 0; img < training.size(); img++) {

      const vector<double> current_deltas = deltas[clayer + 1];
      vector<double> prev_activations;

      // If first layer, use pooled data from final convo layer and load into prev_activations
      if (clayer == 0) {
        for (auto& pooled_image : layers[clayer].pooled_data) {
          for (auto& row : pooled_image.entry) {
            for (auto& val : row) {
              prev_activations.push_back(val);
            }
          }
        }
      // Otherwise, just load the data from the previous layer
      } else {
        vector<double> prev_layer_data = fully_connected.connected[clayer - 1].data;
        prev_activations = prev_layer_data;
      }
      // Now update the weights and biases for this layer
      int layer_size = fully_connected.connected[clayer].size;
      // For every node in this layer, iterate through every previoud node and adjust 
      // the weights based on activations and deltas
      for (int node = 0 ; node < layer_size; node++) {
        for (int prev_node = 0; prev_node < prev_activations.size(); prev_node++) {
          double grad = current_deltas[node] * prev_activations[prev_node];
          fully_connected.connected[clayer].weights[node][prev_node] -= learn * grad;
        }

      // Do the same for the bias
        double bias_grad = current_deltas[node];
        fully_connected.connected[clayer].bias[node] -= learn * bias_grad;
      }

      
    }
  }
}

void Model::trainModel(double learn, int batch_size, int epoches,
                       const vector<image> training, vector<double> labels) {
  for (int epoch = 0; epoch < epoches; epoch++) {
    if (epoch % 15 == 0 && epoch != 0)
      learn = learn * 0.9;
    vector<image> batch;
    vector<double> batch_labels;
    auto indicies = selectRandomIndices(batch_size, training.size());
    for (int i = 0; i < batch_size; i++) {
      batch.push_back(training[indicies[i]]);
      batch_labels.push_back(labels[indicies[i]]);
    }
    backwardPropagate(batch, batch_labels, learn);
  }
}
