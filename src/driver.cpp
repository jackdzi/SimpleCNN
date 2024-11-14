

#define BYTE unsigned char
#ifdef _WIN32
#include <direct.h> // For Windows
#define GetCurrentDir _getcwd
#else
#include <unistd.h> // For Unix-like systems
#define GetCurrentDir getcwd
#endif

#include <fstream>
#include <functional>
#include <iostream>
#include <optional>
#include <random>
#include <variant>
#include <vector>

template <typename T, typename... Args>
std::vector<T> vec(T first, Args... args) {
  return std::vector<T>{first, args...};
}

using std::cout;
using std::ios;
using std::vector;

#define BYTE unsigned char

struct filter {
  vector<vector<double>> fweights;

  filter(int dim, std::mt19937 gen, std::normal_distribution<> d) {

    for (auto &row : fweights) {
      for (auto &weight : row) {
        weight = d(gen);
      }
    }
  }
};
struct image {
  vector<vector<double>> entry;

  image(size_t rows, size_t cols) : entry(rows, vector<double>(cols, 0.0)) {}
};

using std::ios;

std::optional<std::variant<vector<image>, vector<unsigned int>>>
readFile(std::string path); // use auto [dmat, uvec]i
vector<image> imageToDmat(std::ifstream &file);
std::vector<unsigned int> labelToUvec(std::ifstream &file);
void endianSwitch(int &num);
std::string directoryPrefix();

std::optional<std::variant<vector<image>, vector<unsigned int>>>
readFile(std::string path) {
  std::ifstream file(path, ios::binary);
  if (file.is_open()) {
    int file_type;
    file.read((char *)&file_type, 4 * sizeof(BYTE));
    endianSwitch(file_type);
    if (file_type == 2051)
      return imageToDmat(file);
    else if (file_type == 2049)
      return labelToUvec(file);
  }
  std::cout << "File could not be read";
  return {};
}

vector<image> imageToDmat(std::ifstream &file) {
  int images, rows, cols;
  file.read((char *)&images, 4 * sizeof(BYTE));
  file.read((char *)&rows, 4 * sizeof(BYTE));
  file.read((char *)&cols, 4 * sizeof(BYTE));
  endianSwitch(images);
  endianSwitch(rows);
  endianSwitch(cols);

  vector<image> result(images, image(rows, cols));
  for (int i = 0; i < images; i++) {
    for (int j = 0; j < rows; j++) {
      for (int k = 0; k < cols; k++) {
        BYTE temp;
        file.read((char *)&temp, sizeof(BYTE));
        result[i].entry[j][k] = static_cast<double>(temp) / 256;
      }
    }
  }

  return result;
}

std::vector<unsigned int> labelToUvec(std::ifstream &file) {
  int labels;
  file.read((char *)&labels, 4 * sizeof(BYTE));
  endianSwitch(labels);
  std::vector<unsigned int> matrix(labels);

  for (int i = 0; i < labels; i++) {
    BYTE temp;
    file.read((char *)&temp, sizeof(BYTE));
    matrix[i] = static_cast<unsigned int>(temp);
  }
  return matrix;
}

void endianSwitch(int &num) {
  BYTE b1 = num & 255;
  BYTE b2 = (num >> 8) & 255;
  BYTE b3 = (num >> 16) & 255;
  BYTE b4 = (num >> 24) & 255;
  num = (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
  return;
}

std::string directoryPrefix() {
  char buffer[256];
  GetCurrentDir(buffer, sizeof(buffer));
  std::string str(buffer);
  return str;
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
}
