#include "../include/convertData.h"
#include "../include/model.h"

#define vec(...) std::vector{__VA_ARGS__}

int main() {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train";
  auto matrix = readFile(train_path + "/train-images.idx3-ubyte");
  auto labels = readFile(train_path + "/train-labels.idx1-ubyte");

  if (matrix.has_value()) {
    if (std::holds_alternative<vector<image>>(matrix.value())) {
      for (int i = 0; i < 28; i++) {
        for (int j = 0; j < 28; j++) {
          if (std::get<vector<image>>(matrix.value())[1053].entry[i][j] > 0)
            cout << 1;
          else
            cout << 0;
        }
        cout << std::endl;
      }
    }
  }

  // Model(Kernel sizes, image size before convolution, number of filters in each kernel {should match kernel size[i-1]}, hidden layer sizes of fully connected layer, pooling sizes)
  Model model = Model(vec(32,64), vec(28, 13), vec(1, 32), vec(54, 10), vec(2, 2));

  vector<double> probs = model.forwardPropagate(vec(std::get<vector<image>>
  (matrix.value())[1053]), false);
  for (auto prob: probs) {
    cout << prob << std::endl;
  }
}
