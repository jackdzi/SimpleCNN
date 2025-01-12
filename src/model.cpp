#include "../include/model.h"
#include <algorithm>

Model::Model(vector<int> kernel_sizes, vector<int> input_sizes,
             vector<int> filters, vector<int> cLayer_sizes,
             vector<int> pooling_size)
    : fully_connected(Classification(
          cLayer_sizes, kernel_sizes[kernel_sizes.size() - 1] *
                            (pow(double((input_sizes[input_sizes.size() - 1] -
                                         2)) / // TODO: Check if this is correct
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

void Model::trainModel(double learn_rate, int batch_size, int epochs,
                       const vector<image> &training,
                       const vector<double> &labels) {
  if (training.empty() || labels.empty() || training.size() != labels.size()) {
    throw std::invalid_argument("Invalid training data or labels");
  }

  const int num_samples = training.size();
  const int steps_per_epoch = (num_samples + batch_size - 1) / batch_size;

  double current_learn_rate = learn_rate;
  const double decay_rate = 0.9;
  const int decay_steps = 15;

  for (int epoch = 0; epoch < epochs; epoch++) {
    if (epoch > 0 && epoch % decay_steps == 0) {
      current_learn_rate *= decay_rate;
    }

    // Shuffle batch
    vector<int> shuffled_indices(num_samples);
    for (int i = 0; i < num_samples; i++) {
      shuffled_indices[i] = i;
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffled_indices.begin(), shuffled_indices.end(), gen);

    double epoch_loss = 0.0;
    int correct_predictions = 0;

    // For each batch
    for (int step = 0; step < steps_per_epoch; step++) {
      int start_idx = step * batch_size;
      int end_idx = std::min(start_idx + batch_size, num_samples);
      int current_batch_size = end_idx - start_idx;

      // Allocate memory for batch
      vector<image> batch_images;
      vector<double> batch_labels;
      batch_images.reserve(current_batch_size);
      batch_labels.reserve(current_batch_size);
      std::cout << "a" << std::endl;

      for (int i = start_idx; i < end_idx; i++) {
        batch_images.push_back(training[shuffled_indices[i]]);
        batch_labels.push_back(labels[shuffled_indices[i]]);
      }

      auto predictions = forwardPropagate(batch_images, true);
      std::cout << "b" << std::endl;

      // Compute batch loss and accuracy
      double batch_loss = 0.0;
      for (int i = 0; i < current_batch_size; i++) {
        int true_label = static_cast<int>(batch_labels[i]);
        batch_loss -= std::log(predictions[i][true_label] + 1e-15);

        // Find predicted class
        int predicted_class = 0;
        double max_prob = predictions[i][0];
        for (int j = 1; j < 10; j++) {
          if (predictions[i][j] > max_prob) {
            max_prob = predictions[i][j];
            predicted_class = j;
          }
        }
        if (predicted_class == true_label) {
          correct_predictions++;
        }
      }
      epoch_loss += batch_loss;

      // Backward pass
      backwardPropagate(batch_images, batch_labels, current_learn_rate);

      if (step % 10 == 0) {
        std::cout << "Epoch " << epoch + 1 << "/" << epochs << ", Batch "
                  << step + 1 << "/" << steps_per_epoch
                  << ", Batch Loss: " << batch_loss / current_batch_size
                  << std::endl;
      }
      if (step % 10 == 3) {
        break;
      }
    }

    // Statistics

    epoch_loss /= num_samples;
    double epoch_accuracy =
        static_cast<double>(correct_predictions) / num_samples;

    std::cout << "Epoch " << epoch + 1 << "/" << epochs
              << ", Loss: " << epoch_loss
              << ", Accuracy: " << epoch_accuracy * 100 << "%"
              << ", Learning Rate: " << current_learn_rate << std::endl;
  }
}

void Model::backwardPropagate(vector<image> training, vector<double> labels,
                              double learn) {
  int batch_size = training.size();

  auto probabilities = forwardPropagate(training, true);

  // Cross-entropy loss gradients
  std::cout << "c" << std::endl;

  vector<double> final_errors(10, 0.0);
  for (int img = 0; img < batch_size; img++) {
    for (int prob = 0; prob < 10; prob++) {
      // Cross-entropy gradient: p_i - y_i
      int target = (prob == static_cast<int>(labels[img])) ? 1 : 0;
      final_errors[prob] += (probabilities[img][prob] - target) / batch_size;
    }
  }

  // Now backpropagate
  vector<vector<double>> deltas;
  deltas.push_back(final_errors);

  for (int clayer = fully_connected.size - 2; clayer >= 0; clayer--) {
    int current_layer_size = fully_connected.connected[clayer].size;
    int next_layer_size = fully_connected.connected[clayer + 1].size;
    vector<double> node_deltas(current_layer_size, 0.0);

    // Compute error gradients
    for (int node = 0; node < current_layer_size; node++) {
      for (int next_node = 0; next_node < next_layer_size; next_node++) {
        node_deltas[node] +=
            deltas[0][next_node] *
            fully_connected.connected[clayer + 1].weights[node][next_node];
      }
      // ReLU derivative
      double activation_derivative =
          fully_connected.connected[clayer].data[node] > 0 ? 1.0 : 0.0;
      node_deltas[node] *= activation_derivative;
    }
    deltas.insert(deltas.begin(), node_deltas);
  }

  std::cout << "d" << std::endl;

  // Update fully connected layer weights
  std::cout << "FC layer update starting" << std::endl;
  std::cout << "FC size: " << fully_connected.size << std::endl;

  // Update FC layer weights
  for (int clayer = 0; clayer < fully_connected.size; clayer++) {
    std::cout << "Updating FC layer: " << clayer << std::endl;
    const vector<double> &current_deltas = deltas[clayer];

    if (clayer == 0) {
      // First layer
      int total_inputs = layers[num_layers - 1].kernels.size() *
                         layers[num_layers - 1].pooled_size *
                         layers[num_layers - 1].pooled_size;

      std::cout << "First FC layer total inputs: " << total_inputs << std::endl;
      std::cout << "First FC layer nodes: "
                << fully_connected.connected[clayer].size << std::endl;

      for (int node = 0; node < fully_connected.connected[clayer].size;
           node++) {
        for (int img = 0; img < batch_size; img++) {
          for (int i = 0; i < layers[num_layers - 1].pooled_size; i++) {
            for (int j = 0; j < layers[num_layers - 1].pooled_size; j++) {
              for (int k = 0; k < layers[num_layers - 1].kernels.size(); k++) {
                // Each image contributes pooled_size * pooled_size features for
                // each kernel
                int prev_node_index = k * (layers[num_layers - 1].pooled_size *
                                           layers[num_layers - 1].pooled_size) +
                                      (i * layers[num_layers - 1].pooled_size) +
                                      j;

                if (prev_node_index >=
                    fully_connected.connected[clayer].weights.size()) {
                  std::cout << "Index out of bounds!" << std::endl;
                  std::cout << "prev_node_index: " << prev_node_index
                            << std::endl;
                  std::cout << "weights size: "
                            << fully_connected.connected[clayer].weights.size()
                            << std::endl;
                  throw std::runtime_error(
                      "Index out of bounds in FC layer update");
                }

                double grad = current_deltas[node] *
                              layers[num_layers - 1].pooled_data[k].entry[i][j];
                fully_connected.connected[clayer]
                    .weights[prev_node_index][node] -=
                    learn * grad / batch_size;
              }
            }
          }
        }
        fully_connected.connected[clayer].bias[node] -=
            learn * current_deltas[node] / batch_size;
      }
    } else {
      // Other layers...
      int layer_size = fully_connected.connected[clayer].size;
      cLayer *prev_layer = &fully_connected.connected[clayer - 1];

      for (int node = 0; node < layer_size; node++) {
        for (int prev_node = 0; prev_node < prev_layer->activation.size();
             prev_node++) {
          double grad =
              current_deltas[node] * prev_layer->activation[prev_node];
          fully_connected.connected[clayer].weights[prev_node][node] -=
              learn * grad / batch_size;
        }
        fully_connected.connected[clayer].bias[node] -=
            learn * current_deltas[node] / batch_size;
      }
    }
  }

  std::cout << "e" << std::endl;
  // Initialize conv layer deltas
  vector<vector<vector<image>>> conv_deltas(num_layers);
  for (int l = 0; l < num_layers; l++) {
    conv_deltas[l] = vector<vector<image>>(
        layers[l].kernels.size(),
        vector<image>(layers[l].kernels[0].size(), image(3, 3)));
  }

  std::cout << "f" << std::endl;

  for (int conv_layer = num_layers - 1; conv_layer >= 0; conv_layer--) {
    backpropConvLayer(conv_layer, conv_deltas[conv_layer], learn, batch_size,
                      training);
  }
}

void Model::backpropConvLayer(int layer_index,
                              vector<vector<image>> &layer_deltas,
                              double learn_rate, int batch_size,
                              const vector<image> &input_images) {
  Layer &current_layer = layers[layer_index];

  // Initialize unpooled gradients to match data size
  vector<image> unpooled_gradients(
      current_layer.kernels.size(),
      image(current_layer.data[0].entry.size(),
            current_layer.data[0].entry[0].size()));

  std::cout << "=== Starting Backprop ===\n";
  std::cout << "Unpooled gradients size: " << unpooled_gradients.size() << " x "
            << unpooled_gradients[0].entry.size() << " x "
            << unpooled_gradients[0].entry[0].size() << std::endl;

  // Step 1: Zero initialize gradients
  for (int k = 0; k < current_layer.kernels.size(); k++) {
    for (int i = 0; i < unpooled_gradients[k].entry.size(); i++) {
      for (int j = 0; j < unpooled_gradients[k].entry[0].size(); j++) {
        unpooled_gradients[k].entry[i][j] = 0.0;
      }
    }
  }

  // Step 2: Compute gradients for each kernel
  for (int k = 0; k < current_layer.kernels.size(); k++) {
    // For each pooling window
    for (int i = 0; i < current_layer.pooled_size; i++) {
      for (int j = 0; j < current_layer.pooled_size; j++) {
        if (current_layer.psize == 1) {
          // No pooling case - gradient flows directly
          unpooled_gradients[k].entry[i][j] = 1.0; // Simple gradient for now
        } else {
          // Find max in pooling window
          double max_val = -std::numeric_limits<double>::infinity();
          int max_pi = 0, max_pj = 0;

          // Find max position
          for (int pi = 0; pi < current_layer.psize; pi++) {
            for (int pj = 0; pj < current_layer.psize; pj++) {
              int orig_i = i * current_layer.psize + pi;
              int orig_j = j * current_layer.psize + pj;

              if (orig_i < current_layer.data[k].entry.size() &&
                  orig_j < current_layer.data[k].entry[0].size()) {
                double val = current_layer.data[k].entry[orig_i][orig_j];
                if (val > max_val) {
                  max_val = val;
                  max_pi = pi;
                  max_pj = pj;
                }
              }
            }
          }

          // Route gradient through max position
          int orig_i = i * current_layer.psize + max_pi;
          int orig_j = j * current_layer.psize + max_pj;

          if (orig_i < unpooled_gradients[k].entry.size() &&
              orig_j < unpooled_gradients[k].entry[0].size()) {
            double gradient = 1.0; // Will be computed from layer_deltas
            for (int c = 0; c < current_layer.kernels[k].size(); c++) {
              gradient +=
                  layer_deltas[k][c].entry[1][1]; // Use center of gradient
            }
            unpooled_gradients[k].entry[orig_i][orig_j] = gradient;
          }
        }
      }
    }
  }

  // Step 3: Apply ReLU gradient
  for (int k = 0; k < current_layer.kernels.size(); k++) {
    for (int i = 0; i < unpooled_gradients[k].entry.size(); i++) {
      for (int j = 0; j < unpooled_gradients[k].entry[0].size(); j++) {
        if (current_layer.data[k].entry[i][j] <= 0) {
          unpooled_gradients[k].entry[i][j] = 0;
        }
      }
    }
  }

  // Step 4: Compute kernel gradients
  for (int k = 0; k < current_layer.kernels.size(); k++) {
    for (int c = 0; c < current_layer.kernels[k].size(); c++) {
      // For each weight in the 3x3 kernel
      for (int fi = 0; fi < 3; fi++) {
        for (int fj = 0; fj < 3; fj++) {
          double kernel_gradient = 0.0;

          for (int i = 1; i < current_layer.size - 1; i++) {
            for (int j = 1; j < current_layer.size - 1; j++) {
              if (layer_index > 0) {
                kernel_gradient += unpooled_gradients[k].entry[i - 1][j - 1] *
                                   layers[layer_index - 1]
                                       .pooled_data[c]
                                       .entry[i + (fi - 1)][j + (fj - 1)];
              } else {
                kernel_gradient +=
                    unpooled_gradients[k].entry[i - 1][j - 1] *
                    input_images[c].entry[i + (fi - 1)][j + (fj - 1)];
              }
            }
          }

          kernel_gradient /= batch_size;
          current_layer.kernels[k][c].fweights[fi][fj] -=
              learn_rate * kernel_gradient;
        }
      }
    }

    // Update bias
    double bias_gradient = 0.0;
    for (int i = 0; i < unpooled_gradients[k].entry.size(); i++) {
      for (int j = 0; j < unpooled_gradients[k].entry[0].size(); j++) {
        bias_gradient += unpooled_gradients[k].entry[i][j];
      }
    }
    bias_gradient /= batch_size;
    current_layer.bias[k] -= learn_rate * bias_gradient;
  }

  // Step 5: Compute gradients for previous layer if needed
  if (layer_index > 0) {
    Layer &prev_layer = layers[layer_index - 1];
    vector<image> prev_layer_gradients(prev_layer.kernels.size(),
                                       image(prev_layer.size, prev_layer.size));

    for (int k = 0; k < current_layer.kernels.size(); k++) {
      for (int c = 0; c < current_layer.kernels[k].size(); c++) {
        for (int i = 1; i < prev_layer.size - 1; i++) {
          for (int j = 1; j < prev_layer.size - 1; j++) {
            double grad = 0.0;
            for (int fi = -1; fi < 2; fi++) {
              for (int fj = -1; fj < 2; fj++) {
                if (i + fi >= 1 && i + fi < current_layer.size - 1 &&
                    j + fj >= 1 && j + fj < current_layer.size - 1) {
                  grad += unpooled_gradients[k].entry[i + fi - 1][j + fj - 1] *
                          current_layer.kernels[k][c].fweights[fi + 1][fj + 1];
                }
              }
            }
            prev_layer_gradients[c].entry[i][j] += grad;
          }
        }
      }
    }
    layer_deltas[layer_index - 1] = prev_layer_gradients;
  }
}
