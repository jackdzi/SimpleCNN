#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

using std::vector;
using std::cout;

#define FILTER_SIZE 3

class Layer {
private:
  vector<vector<double>> data;
  vector<vector<double>> filter;
public:
  //Constructor
  Layer(int filter_param); 
  //Load data in for first layer
  void load(const vector<vector<double>> input);
  //Add padding
  vector<vector<double>> padding(int n);
  // Convolution
  void convolve();
  // Pooling
  void pool();
};
