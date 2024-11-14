#include "./convertData.h"
#include "./classification.h"
#include "./layer.h"

class Model {
public:
  int num_layers;
  vector<Layer> layers;
  Classification fully_connected;

  Model(vector<int> kernel_sizes, vector<int> input_sizes, vector<int> filters, vector<int> cLayer_sizes, vector<int> filter_type, vector<int> pooling_size); // TODO: Change paramater to choose pooling type
  vector<double> forwardPropagate(vector<image> input, bool testing);
};
