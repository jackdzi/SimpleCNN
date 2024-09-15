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

using std::ios;

std::optional<
    std::variant<std::vector<std::vector<double>>, std::vector<unsigned int>>>
readFile(std::string path); // use auto [dmat, uvec]i
std::vector<std::vector<double>> imageToDmat(std::ifstream &file);
std::vector<unsigned int> labelToUvec(std::ifstream &file);
void endianSwitch(int &num);
std::string directoryPrefix();
