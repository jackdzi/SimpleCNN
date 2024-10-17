#include "../include/convertData.h"
#include "../include/model.h"
using std::cout;

#define CONV_OUTPUT_DIM 7

template <typename T, typename... Args>
std::vector<T> vec(T first, Args... args) {
    return std::vector<T>{first, args...};
}

int main() {
  std::string prefix = directoryPrefix();
  if (prefix.substr(prefix.size() - 6, 6) == "/build")
    prefix = prefix.substr(0, prefix.size() - 6);
  std::string train_path = prefix + "/train";
  auto matrix = readFile(train_path + "/train-images.idx3-ubyte");
  auto labels = readFile(train_path + "/train-labels.idx1-ubyte");
  int number = 1;

  if (matrix.has_value()) {
    if (std::holds_alternative<std::vector<std::vector<double>>>(
            matrix.value())) {
      for (int i = 0; i < 28 * 28; i++) {
        if (i % 28 == 0)
          cout << std::endl;
        if (std::get<std::vector<std::vector<double>>>(
                matrix.value())[number][i] > 0)
          cout << 1;
        else
          cout << 0;
      }
    }
  }
  cout << std::endl;
  cout << std::get<std::vector<unsigned int>>(labels.value())[number];

  Layer layer = Layer(FILTER_SIZE, 28, 2);

  layer.loadFromImages(std::get<std::vector<std::vector<double>>>(
                matrix.value())[1]); // load in image 5 which is a single vector, easier conversions
  layer.convolve(); // use filter value of 1 to find sum of all nearby pixels
  for (int i = 0; i < layer.size; i++) {
    for (int j = 0; j < layer.size; j++) {
      cout << static_cast<int>(layer.data[i][j]);
    }
    cout << std::endl;
  }
  // true means training is on, false means testing runs
  layer.maxPool2d(true, 0.0);
  for (int i = 0; i < layer.pooled_size; i++) {
    for (int j = 0; j < layer.pooled_size; j++) {
      cout << static_cast<int>(layer.pooled_data[i][j]);
    }
    cout << std::endl;
  }

  Layer layer1 = Layer(FILTER_SIZE, 14, 2);
  layer1.loadIntoLayer(layer.pooled_data);
  layer1.convolve();
  layer1.maxPool2d(true, 0.0);
  for (int i = 0; i < layer1.pooled_size; i++) {
    for (int j = 0; j < layer1.pooled_size; j++) {
      cout << static_cast<int>(layer1.pooled_data[i][j] / 10);
    }
    cout << std::endl;
  }
  Model new_model = Model(vec(28), vec(50,50,10), vec(3), vec(2)); // Model(Dim of convolution layers, dim of fully_connected layers, dim of filters, dim of pooling)
  cout << new_model.fully_connected.connected[0].weights[0][0];
  auto probs = new_model.forwardPropagate(std::get<std::vector<std::vector<double>>>(
                matrix.value())[1]);
  for (int i = 0; i < 10; i++)
    cout << round(probs[i]*1000) /1000.0<< std::endl;
  return 0;
  // Extreme probability values due to weights just being initialized and due to filter giving very high values
}
