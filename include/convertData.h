#define BYTE unsigned char
#ifdef _WIN32
#include <direct.h>  // For Windows
#define GetCurrentDir _getcwd
#else
#include <unistd.h> // For Unix-like systems
#define GetCurrentDir getcwd
#endif

#include <armadillo>
#include <iostream>
#include <optional>
#include <variant>
#include <functional>

using std::ios;

std::optional<std::variant<arma::dmat, arma::uvec>> readFile(std::string path); // use auto [dmat, uvec]
arma::dmat imageToDmat(std::ifstream &file);
arma::uvec labelToUvec(std::ifstream &file);
void endianSwitch(int &num); 
std::string directoryPrefix();