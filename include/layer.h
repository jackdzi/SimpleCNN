#include "./headers.h"

#define FILTER_SIZE 3

class Layer {
private:
  vector<vector<filter>> kernels;
  vector<filter> bias;
public:
  int input_size;
  int pooled_size;
  int num_filters;
  int size;
  vector<image> data;


  vector<image> pooled_data;
  //Constructor
  Layer(int num_kernels, int input_size, int input_depth, int pooling_size,
             int filter_size);
  //Load data in for first layer
  void loadFromImages(const vector<double> input);

  void loadIntoLayer(const vector<vector<double>> input);

  // Convolution
  void correlate(const vector<image> img, int kernel);

  void convolution(const vector<image> img, int kernel);

  // Pooling
  void maxPool2d(bool train_mode, double dropout_rate);
  // Dropout
  void applyDropout(double dropout_rate);
  // Change convolve to cross correlate - Convolve is cross correlate but the filter is flipped by 180 degrees
};
