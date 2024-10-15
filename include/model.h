#include "./convertData.h"
#include "./layer.h"
#include "./classification.h"

class Model {
private:
  int num_layers;
  std::vector<Layer> layers;
  Classification fully_connected;
public:
  Model(vector<int> convolution_size, vector<int> cLayer_sizes, vector<int> filter_type, vector<int> pooling_size);
  vector<double> forwardPropagate(vector<double> input);
};
