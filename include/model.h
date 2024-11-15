#include "./classification.h"
#include "./layer.h"

class Model {
public:
  int num_layers;
  vector<Layer> layers;
  Classification fully_connected;

  Model(vector<int> kernel_sizes, vector<int> input_sizes, vector<int> filters, vector<int> cLayer_sizes, vector<int> pooling_size);
  vector<vector<double>> forwardPropagate(vector<image> input, bool training);
  vector<int> selectRandomIndices(int batch_size, int data_size);
  void backwardPropagate(vector<image> training, vector<double> labels, double learn);
  void trainModel(double learn, int batch_size, int epoches, const vector<image> training);
};
