#include "../include/convertData.h"
#include "../include/model.h"
#include <chrono>
#include <iomanip>
#include <iostream>

double computeAccuracy(Model &model, const vector<image> &data,
                       const vector<unsigned int> &labels,
                       int num_samples = -1) {
  if (num_samples == -1)
    num_samples = data.size();
  num_samples = std::min(num_samples, static_cast<int>(data.size()));

  int correct = 0;
  for (int i = 0; i < num_samples; i++) {
    auto probs = model.forwardPropagate(vec(data[i]), false);
    int predicted = 0;
    double max_prob = probs[0][0];
    for (int j = 1; j < 10; j++) {
      if (probs[0][j] > max_prob) {
        max_prob = probs[0][j];
        predicted = j;
      }
    }
    if (predicted == static_cast<int>(labels[i])) {
      correct++;
    }
  }
  return static_cast<double>(correct) / num_samples;
}

int main() {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train";

  auto matrix_opt = readFile(train_path + "/train-images.idx3-ubyte");
  auto labels_opt = readFile(train_path + "/train-labels.idx1-ubyte");

  if (!matrix_opt.has_value() || !labels_opt.has_value()) {
    std::cerr << "Failed to load dataset" << std::endl;
    return 1;
  }

  const vector<image> &train_images =
      std::get<vector<image>>(matrix_opt.value());
  const vector<unsigned int> &label_uints =
      std::get<vector<unsigned int>>(labels_opt.value());

  vector<double> train_labels;
  train_labels.reserve(label_uints.size());
  for (const auto &label : label_uints) {
    train_labels.push_back(static_cast<double>(label));
  }

  // Sample
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      if (train_images[1053].entry[i][j] > 0)
        cout << 1;
      else
        cout << 0;
    }
    cout << std::endl;
  }

  Model model =
      Model(vec(32, 64), vec(28, 13), vec(1, 32), vec(100, 10), vec(2, 2));

  cout << "\nProbabilities for image 1053 (label " << label_uints[1053]
       << "):" << std::endl;
  vector<vector<double>> probs1 =
      model.forwardPropagate(vec(train_images[1053]), false);
  for (auto prob : probs1[0]) {
    cout << std::fixed << std::setprecision(6) << prob << std::endl;
  }

  cout << "\nProbabilities for image 1054 (label " << label_uints[1054]
       << "):" << std::endl;
  vector<vector<double>> probs2 =
      model.forwardPropagate(vec(train_images[1054]), false);
  for (auto prob : probs2[0]) {
    cout << std::fixed << std::setprecision(6) << prob << std::endl;
  }

  double initial_accuracy =
      computeAccuracy(model, train_images, label_uints, 1000);
  cout << "\nInitial accuracy on first 1000 samples: " << std::fixed
       << std::setprecision(2) << (initial_accuracy * 100) << "%" << std::endl;

  /*
   *
   * Training
   *
   */
  const int epochs = 1;
  const int batch_size = 32;
  double learning_rate = 0.05;

  cout << "\nStarting training..." << std::endl;
  auto start_time = std::chrono::high_resolution_clock::now();

  model.trainModel(learning_rate, batch_size, epochs, train_images,
                   train_labels);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

  double final_accuracy =
      computeAccuracy(model, train_images, label_uints, 1000);
  cout << "\nFinal accuracy on first 1000 samples: " << std::fixed
       << std::setprecision(4) << (final_accuracy * 100) << "%" << std::endl;
  cout << "Training took " << duration.count() << " seconds" << std::endl;




  cout << "\nFinal probabilities for test images:" << std::endl;

  probs1 = model.forwardPropagate(vec(train_images[1053]), false);
  cout << "Image 1053 (label " << label_uints[1053] << "):" << std::endl;
  for (auto prob : probs1[0]) {
    cout << std::fixed << std::setprecision(6) << prob << std::endl;
  }

  probs2 = model.forwardPropagate(vec(train_images[1054]), false);
  cout << "Image 1054 (label " << label_uints[1054] << "):" << std::endl;
  for (auto prob : probs2[0]) {
    cout << std::fixed << std::setprecision(6) << prob << std::endl;
  }

  return 0;
}
