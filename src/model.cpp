#include "../include/model.h"

Model::Model(vector<int> convolution_size, vector<int> cLayer_sizes, vector<int> filter_type, vector<int> pooling_size) :
  fully_connected(Classification(cLayer_sizes, std::pow(convolution_size[convolution_size.size()-1] / pooling_size[pooling_size.size()-1], 2))) 
  {
  num_layers = convolution_size.size();
  for (int i = 0; i < convolution_size.size(); i++) {
    layers.push_back(Layer(filter_type[i], convolution_size[i], pooling_size[i]));
  };
}

vector<double> Model::forwardPropagate(vector<double> input) {
  for (int i = 0; i < num_layers; i++) {
    if (i == 0)
      layers[i].loadFromImages(input);
    else
      layers[i].loadIntoLayer(layers[i-1].pooled_data);
    layers[i].convolve();
    layers[i].maxPool2d(true, 0.0);
  }
  return fully_connected.forwardPass(layers[num_layers-1].pooled_data, layers[num_layers-1].pooled_size);
}
