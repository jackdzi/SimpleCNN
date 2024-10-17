#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>
#include <random>

using std::vector;
using std::cout;

#define FILTER_SIZE 3

class Layer {
private:
  vector<vector<double>> filter;
public:

  int size;
  int pooled_size; 
  // make public for testing b/c none of your functions return the data
  vector<vector<double>> data; // TODO : Add multiple filters and make data, pooled_data, filter multidimensional. Please modify constructor too
  // pooled data layer
  vector<vector<double>> pooled_data;
  //Constructor
  Layer(int filter_param, int input_size, int pooling_size); 
  //Load data in for first layer
  void loadFromImages(const vector<double> input);

  void loadIntoLayer(const vector<vector<double>> input);
  //Add padding
  vector<vector<double>> padding(int n);
  // Convolution
  void convolve(); //TODO: Implement some dropout
  // Pooling
  void maxPool2d(bool train_mode, double dropout_rate);
  // Dropout
  void applyDropout(double dropout_rate);
};
