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
#include <variant>
#include <vector>
#include <random>

using std::vector;
using std::cout;
using std::ios;

#define BYTE unsigned char

struct filter {
    vector<vector<double>> fweights;
};
struct image {
    vector<vector<double>> entry;

    image(size_t rows, size_t cols) : entry(rows, vector<double>(cols, 0.0)) {}
};
