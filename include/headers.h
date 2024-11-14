#pragma once
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

using std::cout;
using std::ios;
using std::vector;

#define BYTE unsigned char

struct filter {
  vector<vector<double>> fweights;

  filter(int size) {
    fweights = vector<vector<double>>(size, vector<double>(size, 0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> d(0, sqrt(2.0 / size));
    for (auto &row: fweights) {
      for (auto &col: row) {
        col = d(gen);
      }
    }
  }
};
struct image {
  vector<vector<double>> entry;

  image(size_t rows, size_t cols) : entry(rows, vector<double>(cols, 0.0)) {}
};
