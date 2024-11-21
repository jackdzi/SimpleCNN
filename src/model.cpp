#include "../include/model.h"
#include <algorithm>
#include <numeric>
#include <functional>
#include <cmath>

// Helper function to rotate a filter by 180 degrees
filter rotate180(const filter& kernel) {
    filter rotated_kernel = kernel;
    int h = kernel.fweights.size();
    int w = kernel.fweights[0].size();
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            rotated_kernel.fweights[i][j] = kernel.fweights[h - 1 - i][w - 1 - j];
        }
    }
    return rotated_kernel;
}

// Helper function to perform convolution between two matrices
Matrix convolve(const Matrix& input, const Matrix& kernel) {
    int H_in = input.size();
    int W_in = input[0].size();
    int H_k = kernel.size();
    int W_k = kernel[0].size();
    int H_out = H_in - H_k + 1;
    int W_out = W_in - W_k + 1;
    Matrix output(H_out, vector<double>(W_out, 0.0));

    for (int i = 0; i < H_out; i++) {
        for (int j = 0; j < W_out; j++) {
            double sum = 0.0;
            for (int m = 0; m < H_k; m++) {
                for (int n = 0; n < W_k; n++) {
                    sum += input[i + m][j + n] * kernel[m][n];
                }
            }
            output[i][j] = sum;
        }
    }
    return output;
}

// Helper function to compute the gradient of the kernel
Matrix computeGradient(const Matrix& input, const Matrix& delta) {
    int H_in = input.size();
    int W_in = input[0].size();
    int H_delta = delta.size();
    int W_delta = delta[0].size();
    int H_k = H_in - H_delta + 1;
    int W_k = W_in - W_delta + 1;
    Matrix grad(H_k, vector<double>(W_k, 0.0));

    for (int m = 0; m < H_k; m++) {
        for (int n = 0; n < W_k; n++) {
            double sum = 0.0;
            for (int i = 0; i < H_delta; i++) {
                for (int j = 0; j < W_delta; j++) {
                    sum += input[i + m][j + n] * delta[i][j];
                }
            }
            grad[m][n] = sum;
        }
    }
    return grad;
}

// Model constructor
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

// Select random indices for batching
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

// Forward propagation
vector<vector<double>> Model::forwardPropagate(vector<image> input,
                                               bool training) {
  vector<vector<double>> probabilities;
  for (int img = 0; img < input.size(); img++) {
    // Forward through convolutional layers
    for (int i = 0; i < num_layers; i++) {
      for (int kernel = 0; kernel < layers[i].kernels.size(); kernel++) {
        if (i == 0)
          layers[i].correlate({input[img]}, kernel);
        else
          layers[i].correlate(layers[i - 1].pooled_data, kernel);
      }
      layers[i].applyActivation(); // Apply activation function (ReLU)
      layers[i].maxPool2d(training, 0.01); // Perform max-pooling
    }
    // Flatten the output from the last convolutional layer
    vector<double> flattened_input;
    for (auto& pooled_image : layers.back().pooled_data) {
      for (auto& row : pooled_image.entry) {
        for (auto& val : row)
          flattened_input.push_back(val);
      }
    }
    // Forward through fully connected layers
    probabilities.push_back(fully_connected.forwardPass(flattened_input));
  }
  return probabilities;
}

// Backward propagation
void Model::backwardPropagate(vector<image> training, vector<double> labels,
                              double learn) {
  vector<double> final_errors(10, 0.0);
  auto probabilities = forwardPropagate(training, true);

  // Compute the error at the output layer
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

  // Backpropagate through fully connected layers
  for (int clayer = fully_connected.size - 2; clayer >= 0; clayer--) {
    int current_layer_size = fully_connected.connected[clayer].size;
    int next_layer_size = fully_connected.connected[clayer + 1].size;
    vector<double> node_deltas(current_layer_size, 0.0);

    for (int node = 0; node < current_layer_size; node++) {
      for (int prev_node = 0; prev_node < next_layer_size; prev_node++) {
        node_deltas[node] +=
            deltas[0][prev_node] *
            fully_connected.connected[clayer + 1].weights[prev_node][node];
      }
      int activation_derivative =
          fully_connected.connected[clayer].data[node] > 0 ? 1 : 0;
      node_deltas[node] *= activation_derivative;
    }
    deltas.insert(deltas.begin(), node_deltas);
  }

  // Update weights and biases in fully connected layers
  for (int clayer = 0; clayer < fully_connected.size; clayer++) {
    const vector<double>& current_deltas = deltas[clayer];
    vector<double> prev_activations;

    if (clayer == 0) {
      // Flattened pooled data from the last convolutional layer
      for (auto& pooled_image : layers.back().pooled_data) {
        for (auto& row : pooled_image.entry) {
          for (auto& val : row)
            prev_activations.push_back(val);
        }
      }
    } else {
      prev_activations = fully_connected.connected[clayer - 1].activation;
    }

    int layer_size = fully_connected.connected[clayer].size;
    for (int node = 0; node < layer_size; node++) {
      for (int prev_node = 0; prev_node < prev_activations.size(); prev_node++) {
        double grad = current_deltas[node] * prev_activations[prev_node];
        fully_connected.connected[clayer].weights[node][prev_node] -= learn * grad;
      }
      // Update bias
      double bias_grad = current_deltas[node];
      fully_connected.connected[clayer].bias[node] -= learn * bias_grad;
    }
  }

  // Backpropagate to the input of the first fully connected layer
  vector<double> delta_input_fc(fully_connected.connected[0].input_size, 0.0);
  for (int node = 0; node < fully_connected.connected[0].size; node++) {
    for (int prev_node = 0; prev_node < fully_connected.connected[0].input_size; prev_node++) {
      delta_input_fc[prev_node] += deltas[0][node] * fully_connected.connected[0].weights[node][prev_node];
    }
  }
  // Apply activation derivative (ReLU)
  for (int prev_node = 0; prev_node < fully_connected.connected[0].input_size; prev_node++) {
    if (fully_connected.connected[0].data[prev_node] > 0) {
      delta_input_fc[prev_node] *= 1;
    } else {
      delta_input_fc[prev_node] *= 0;
    }
  }

  // Reshape delta_input_fc to match the shape of the pooled data
  int num_filters = layers.back().pooled_data.size();
  int pooled_height = layers.back().pooled_data[0].entry.size();
  int pooled_width = layers.back().pooled_data[0].entry[0].size();

  vector<image> delta_pooled(num_filters);
  int idx = 0;
  for (int f = 0; f < num_filters; f++) {
    delta_pooled[f].entry.resize(pooled_height, vector<double>(pooled_width, 0.0));
    for (int i = 0; i < pooled_height; i++) {
      for (int j = 0; j < pooled_width; j++) {
        delta_pooled[f].entry[i][j] = delta_input_fc[idx++];
      }
    }
  }

  // Backpropagate through pooling layers
  vector<image> delta_unpooled(num_filters);
  for (int f = 0; f < num_filters; f++) {
    int conv_height = layers.back().data[f].entry.size();
    int conv_width = layers.back().data[f].entry[0].size();
    delta_unpooled[f].entry.resize(conv_height, vector<double>(conv_width, 0.0));

    for (int i = 0; i < pooled_height; i++) {
      for (int j = 0; j < pooled_width; j++) {
        // Get the indices of the max value from the mask
        int max_i = layers.back().mask[f][i][j].first;
        int max_j = layers.back().mask[f][i][j].second;
        // Assign the delta to the position of the max value
        delta_unpooled[f].entry[max_i][max_j] = delta_pooled[f].entry[i][j];
      }
    }

    // Apply activation derivative (ReLU)
    for (int i = 0; i < conv_height; i++) {
      for (int j = 0; j < conv_width; j++) {
        if (layers.back().data[f].entry[i][j] <= 0) {
          delta_unpooled[f].entry[i][j] = 0;
        }
      }
    }
  }

  // Backpropagate through convolutional layers
  vector<image> delta_current = delta_unpooled;
  for (int layer_idx = num_layers - 1; layer_idx >= 0; layer_idx--) {
    Layer& layer = layers[layer_idx];
    vector<image> delta_prev;

    // For each kernel (output feature map)
    for (int k = 0; k < layer.kernels.size(); k++) {
      // For each input channel
      for (int c = 0; c < layer.kernels[k].size(); c++) {
        // Get the input to the current layer
        image input_image;
        if (layer_idx == 0) {
          // Input is the original image
          input_image = training[0]; // Assuming batch size is 1
        } else {
          input_image = layers[layer_idx - 1].pooled_data[c];
        }

        // Compute gradient for the kernel
        Matrix grad_kernel = computeGradient(input_image.entry, delta_current[k].entry);

        // Update kernel weights
        for (int i = 0; i < layer.kernels[k][c].fweights.size(); i++) {
          for (int j = 0; j < layer.kernels[k][c].fweights[0].size(); j++) {
            layer.kernels[k][c].fweights[i][j] -= learn * grad_kernel[i][j];
          }
        }
      }

      // Update bias
      double bias_grad = 0.0;
      for (int i = 0; i < delta_current[k].entry.size(); i++) {
        for (int j = 0; j < delta_current[k].entry[0].size(); j++) {
          bias_grad += delta_current[k].entry[i][j];
        }
      }
      for (int i = 0; i < layer.bias[k].fweights.size(); i++) {
        for (int j = 0; j < layer.bias[k].fweights[0].size(); j++) {
          layer.bias[k].fweights[i][j] -= learn * bias_grad;
        }
      }
    }

    // Compute delta_prev if not the first layer
    if (layer_idx > 0) {
      int num_input_channels = layer.kernels[0].size();
      delta_prev.resize(num_input_channels);
      for (int c = 0; c < num_input_channels; c++) {
        int delta_height = delta_current[0].entry.size() + layer.kernels[0][0].fweights.size() - 1;
        int delta_width = delta_current[0].entry[0].size() + layer.kernels[0][0].fweights[0].size() - 1;
        delta_prev[c].entry.resize(delta_height, vector<double>(delta_width, 0.0));

        for (int k = 0; k < layer.kernels.size(); k++) {
          // Rotate kernel by 180 degrees
          filter rotated_kernel = rotate180(layer.kernels[k][c]);
          // Convolve delta_current[k] with rotated_kernel
          Matrix delta_conv = convolve(delta_current[k].entry, rotated_kernel.fweights);
          // Add to delta_prev[c]
          for (int i = 0; i < delta_conv.size(); i++) {
            for (int j = 0; j < delta_conv[0].size(); j++) {
              delta_prev[c].entry[i][j] += delta_conv[i][j];
            }
          }
        }

        // Apply activation derivative (ReLU)
        for (int i = 0; i < delta_prev[c].entry.size(); i++) {
          for (int j = 0; j < delta_prev[c].entry[0].size(); j++) {
            if (layers[layer_idx - 1].data[c].entry[i][j] <= 0) {
              delta_prev[c].entry[i][j] = 0;
            }
          }
        }
      }
      delta_current = delta_prev;
    }
  }
}

// Training the model
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