#define STB_IMAGE_IMPLEMENTATION
#include "./libs/stb_image.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

const string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                            "abcdefghijklmnopqrstuvwxyz"
                            "0123456789+/";

bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

vector<unsigned char> base64_decode(const string &encoded_string) {
  int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  vector<unsigned char> ret;

  while (in_len-- && (encoded_string[in_] != '=') &&
         is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_++];
    if (i == 4) {
      for (i = 0; i < 4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] =
          (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] =
          ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (i = 0; i < 3; i++)
        ret.push_back(char_array_3[i]);
      i = 0;
    }
  }

  if (i) {
    for (j = i; j < 4; j++)
      char_array_4[j] = 0;

    for (j = 0; j < 4; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] =
        ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
    char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

    for (j = 0; j < i - 1; j++)
      ret.push_back(char_array_3[j]);
  }

  return ret;
}

bool base64_to_png(const string &base64_string, const string &output_path) {
  try {
    string base64_data = base64_string;
    size_t pos = base64_data.find(",");
    if (pos != string::npos) {
      base64_data = base64_data.substr(pos + 1);
    }

    vector<unsigned char> png_data = base64_decode(base64_data);

    ofstream output_file(output_path, ios::binary);
    if (!output_file) {
      cerr << "Failed to open output file" << endl;
      return false;
    }

    output_file.write(reinterpret_cast<const char *>(png_data.data()),
                      png_data.size());
    output_file.close();

    return true;
  } catch (const exception &e) {
    cerr << "Error: " << e.what() << endl;
    return false;
  }
}

vector<vector<float>> readPNGToGrayscale(const string &filepath) {
  int width, height, channels;
  unsigned char *img =
      stbi_load(filepath.c_str(), &width, &height, &channels, 1);
  if (!img) {
    throw runtime_error("Failed to load image: " + filepath);
  }

  vector<vector<float>> resizedImage(28, vector<float>(28));

  float scaleY = static_cast<float>(height) / 28.0;
  float scaleX = static_cast<float>(width) / 28.0;

  for (int y = 0; y < 28; ++y) {
    for (int x = 0; x < 28; ++x) {
      int startY = static_cast<int>(y * scaleY);
      int endY = static_cast<int>((y + 1) * scaleY);
      int startX = static_cast<int>(x * scaleX);
      int endX = static_cast<int>((x + 1) * scaleX);

      float sum = 0.0;
      for (int yy = startY; yy < endY; ++yy) {
        for (int xx = startX; xx < endX; ++xx)
          sum += img[yy * width + xx];
      }
      resizedImage[y][x] = sum / (scaleY * scaleX);
    }
  }
  stbi_image_free(img);
  return resizedImage;
}

int main() {

  string input;
  getline(cin, input);
  if (base64_to_png(input, "data.png"))
    cout << "Succcessfully converted to PNG" << endl;
  else
    cout << "Failed to conver to PNG" << endl;

  try {
    auto data = readPNGToGrayscale("data.png");
    for (const auto &row : data) {
      for (auto it = row.begin(); it != row.end(); it++) {
        if (it != row.begin())
          cout << " ";
        cout << *it;
      }
      cout << endl;
    }
  } catch (const exception &e) {
    cerr << e.what() << endl;
  }

  return 0;
}
