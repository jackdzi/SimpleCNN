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
  vector<vector<double>> filter;
public:
  // make public for testing b/c none of your functions return the data
  vector<vector<double>> data;
  //Constructor
  Layer(int filter_param); 
  //Load data in for first layer
  void loadFromImages(const vector<double> input);

  void loadIntoLayer(const vector<vector<double>> input);
  //Add padding
  vector<vector<double>> padding(int n);
  // Convolution
  void convolve();
  // Pooling
  void pool();
};
