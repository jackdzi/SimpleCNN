#include "../include/convertData.h"
#include "../include/layer.h"
using std::cout;

int main() {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train";
  auto matrix = readFile(train_path + "/train-images.idx3-ubyte");
  auto labels = readFile(train_path + "/train-labels.idx1-ubyte");
  int number = 5;

  if (matrix.has_value()) {
    if (std::holds_alternative<std::vector<std::vector<double>>>(
            matrix.value())) {
      for (int i = 0; i < 28 * 28; i++) {
        if (i % 28 == 0)
          cout << std::endl;
        if (std::get<std::vector<std::vector<double>>>(
                matrix.value())[number][i] > 0)
          cout << static_cast<int>((std::get<std::vector<std::vector<double>>>(
                matrix.value())[number][i]));
        else
          cout << 0;
      }
    }
  }
  cout << std::endl;
  cout << std::get<std::vector<unsigned int>>(labels.value())[number];

  Layer layer = Layer(FILTER_SIZE);

  layer.loadFromImages(std::get<std::vector<std::vector<double>>>(
                matrix.value())[5]); // load in image 5 which is a single vector, easier conversions
  layer.convolve(); // use filter value of 1 to find sum of all nearby pixels
  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      cout << static_cast<int>(layer.data[i][j]) << " ";
    }
    cout << std::endl;
  }
  
  layer.maxPool2d();
  for (int i = 0; i < 14; i++) {
    for (int j = 0; j < 14; j++) {
      cout << static_cast<int>(layer.pooledData[i][j]) << " ";
    }
    cout << std::endl;
  }
  return 0;
}
