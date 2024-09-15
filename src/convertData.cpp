#include "../include/convertData.h"

std::optional<std::variant<std::vector<std::vector<double>>, std::vector<unsigned int>>> readFile(std::string path) {
  std::ifstream file(path, ios::binary);
  if (file.is_open()) {
    int file_type;
    file.read((char *)&file_type, 4 * sizeof(BYTE));
    endianSwitch(file_type);
    std::cout << file_type << std::endl;
    if (file_type == 2051)
      return imageToDmat(file);
    else if (file_type == 2049)
      return labelToUvec(file);
  }
  std::cout << "File could not be read";
  return {};
}

std::vector<std::vector<double>> imageToDmat(std::ifstream &file) {
  int images, rows, cols;
  file.read((char *)&images, 4 * sizeof(BYTE));
  file.read((char *)&rows, 4 * sizeof(BYTE));
  file.read((char *)&cols, 4 * sizeof(BYTE));
  endianSwitch(images);
  endianSwitch(rows);
  endianSwitch(cols);
  std::vector<std::vector<double>> matrix(images, std::vector<double>(rows * cols));

  for (int i = 0; i < images; i++) {
    for (int j = 0; j < rows * cols; j++) {
      BYTE temp;
      file.read((char *)&temp, sizeof(BYTE));
      matrix[i][j] = static_cast<double>(temp) / 256;
    }
  }
  return matrix;
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
