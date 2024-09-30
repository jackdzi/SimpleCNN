#include "../include/convertData.h"
using std::cout;

int main() {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train_images";
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
                matrix.value())[number][i] > 0.5)
          cout << 1;
        else
          cout << 0;
      }
    }
  }
  cout << std::endl;
  cout << std::get<std::vector<unsigned int>>(labels.value())[number];
  return 0;
}
