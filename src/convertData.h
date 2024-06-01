#define BYTE unsigned char

#include <armadillo>
#include <iostream>
#include <optional>

using std::ios;

std::optional<std::pair<arma::dmat, arma::uvec>> readFile(std::string train, std::string test); // use auto [dmat, uvec]
arma::dmat imageToDmat(std::ifstream &file);
arma::uvec labelToUvec(std::ifstream &file);
void endianSwitch(int &num);