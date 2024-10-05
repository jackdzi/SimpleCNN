#include "../include/convertData.h"
#include "../include/layer.h"

using std::cout;

int main(void) {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train_images";
  auto matrix = readFile(train_path + "/train-images.idx3-ubyte");
  auto labels = readFile(train_path + "/train-labels.idx1-ubyte");
  int number = 5;




  if (matrix.has_value()) {
    if (std::holds_alternative<std::vector<std::vector<double>>>(matrix.value())) {      
      for (int i = 0; i < 28 * 28; i++) {
        if (i % 28 == 0)
          cout << std::endl;
        if (std::get<std::vector<std::vector<double>>>(matrix.value())[number][i] > 0.5)
          cout << 1;
        else
          cout << 0;
      }
    }
  }
  cout << std::endl;
  cout << std::get<std::vector<unsigned int>>(labels.value())[number];

  Layer layer;

  vector<vector<double>> filter(2, vector<double>(2, 0));

  vector<double> output(26, 0);

  if (matrix.has_value()) {
    if (std::holds_alternative<std::vector<std::vector<double>>>(matrix.value())) {
      cout << "test";
      output = layer.convolve(matrix.value(), filter);
      cout << "test2"
    }
  }
 
  for (int i = 0 ; i < output.size() ; i++) {
    if (i % 26 == 0) {
      cout << "\n";
    }
    cout << output[i];

  }
  
  return 0;
}
