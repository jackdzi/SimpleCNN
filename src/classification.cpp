#include "../include/classification.h"
#include <iostream>
using std::cout;


Classification::Classification(vector<int> layer_sizes, int input_size) {
  size = layer_sizes.size();
  if (layer_sizes[size-1] != 10)
    cout << "ERROR: LAST LAYER MUST HAVE 10 NODES" << std::endl;
  connected.push_back(cLayer(layer_sizes[0], input_size));
  for (int i = 1; i < layer_sizes.size()-1; i++) {
    connected.push_back(cLayer(layer_sizes[i], layer_sizes[i-1]));
  }
}

vector<double> Classification::forwardPass(vector<vector<double>> input, int input_dim) {
  for (int i = 0; i < size; i++) {
    if (i == 0)
      connected[i].loadFromLayer(input, input_dim);
    else
      connected[i].loadFromPrevious(connected[i-1]);
    int activation = i != size - 1 ? 0 : 1;
    connected[i].applyActivation(activation);
  }
  return connected[size-1].activation;
}

