#include "../include/classification.h"

Classification::Classification(vector<int> layer_sizes, int input_size) {
  size = layer_sizes.size();
  if (layer_sizes[size - 1] != 10)
    cout << "Warning: Last layer must have 10 nodes" << std::endl;
  connected.push_back(cLayer(layer_sizes[0], input_size));
  for (int i = 1; i < layer_sizes.size(); i++) {
    connected.push_back(cLayer(layer_sizes[i], layer_sizes[i - 1]));
  }
}

vector<double> Classification::forwardPass(vector<image> input) {
  for (int i = 0; i < size; i++) {
    if (i == 0)
      connected[i].loadFromLayer(input);
    else
      connected[i].loadFromPrevious(connected[i - 1]);
    int activation = i != size - 1 ? 0 : 1;
    connected[i].applyActivation(activation);
  }
  return connected[size - 1].activation;
}

// This is wrong
// void Classification::backwardPass(vector<vector<image>> testing,
//                                   double learn_rate) {
//   vector<double> error = vector<double>(10, 0);
//   for (int image = 0; image < testing.size(); image++) {
//     forwardPass(testing[image])
//     for (int i = 0; i < 10; i++)
//       error[image] = pow(targets[image][i] - connected[size - 1].activation[i], 2) / 2;
//   }
//   vector<vector<double>> delta;
// }
