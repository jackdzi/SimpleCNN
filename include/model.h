#include "./convertData.h"
#include "./layer.h"
#include "./classification.h"

class Model {
public:
  int num_layers;
  std::vector<Layer> layers;
  Classification fully_connected;

  Model(vector<int> convolution_size, vector<int> cLayer_sizes, vector<int> filter_type, vector<int> pooling_size); // TODO: Change paramater to choose pooling type
  vector<double> forwardPropagate(vector<double> input);
};
