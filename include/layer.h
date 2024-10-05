#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <variant>
#include <vector>

using std::vector;
using std::cout;

class Layer {
public:
  // Convolution
  //
  // Pooling
  vector<double> convolve(const vector<vector<double>> &input, const vector<vector<double>> &filter, int stride = 1) {
    int tester = 0;
    cout << "anyonehome";
    int input_size = input.size();
    cout << tester++;
    int filter_size = filter.size();
    cout << tester++;
    int output_size = (input_size - filter_size) / stride + 1;
    cout << tester++;
    vector<double> output(output_size, 0);
    int count = 0;
    cout << tester++;
    for (int filter_row = 0 ; filter_row < input_size - filter_size + 1; filter_row++) {
      for (int filter_col  = 0 ; filter_col < input_size - filter_size + 1; filter_col ++) {
        double sum = 0.0;

        for (int z = 0; z < filter_size ; z++) {
          for (int x = 0 ; x < filter_size ; x++) {
            sum += filter[z][x] * input[filter_row + z][filter_col + x];
          }
        }
        output[count++] = sum / (filter_size * filter_size);
      }
    }
    cout << tester++;
    return output;
  }

};
