#include "../include/cLayer.h"

class Classification {
public:
  vector<cLayer> connected;
  int size;

  Classification(vector<int> layer_sizes, int input_size);
  vector<double> forwardPass(vector<vector<double>> input, int input_dim);
};

