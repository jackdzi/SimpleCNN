#include "./classification.h"
#include "./layer.h"

class Model {
public:
  int num_layers;
  vector<Layer> layers;
  Classification fully_connected;

  Model(vector<int> kernel_sizes, vector<int> input_sizes, vector<int> filters,
        vector<int> cLayer_sizes, vector<int> pooling_size);
  vector<vector<double>> forwardPropagate(vector<image> input, bool training);
  vector<int> selectRandomIndices(int batch_size, int data_size);
  void backwardPropagate(vector<image> &training, vector<double> &labels,
                                double learn_rate, double momentum,
                                vector<vector<vector<filter>>> &kernel_velocity,
                                vector<vector<double>> &bias_velocity);
  void trainModel(double learn_rate, int batch_size, int epochs,
                  const vector<image> &training, const vector<double> &labels);
  void backpropConvLayer(int layer_index, vector<vector<image>> &layer_deltas,
                         double learn_rate, int batch_size,
                         const vector<image> &input_images);
};
