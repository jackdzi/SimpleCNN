#include "convertData.h"

std::optional<arma::dmat> readFile(std::string path, std::function<arma::dmat(std::ifstream &)> &func) {
    std::ifstream file(path, ios::binary);
    arma::dmat res;
    if (file.is_open()) {
        return func(file);
    } else {
        std::cout << "File could not be read";
        return {};
    }
}

std::optional<arma::uvec> readFile(std::string path, std::function<arma::uvec(std::ifstream &)> &func) {
    std::ifstream file(path, ios::binary);
    arma::uvec res;
    if (file.is_open()) {
        return func(file);
    } else {
        std::cout << "File could not be read";
        return {};
    }
}

arma::dmat imageToDmat(std::ifstream &file) {

}

void endianSwitch(int &num) {
    BYTE b1 = num & 255;
    BYTE b2 = (num >> 8) & 255;
    BYTE b3 = (num >> 16) & 255;
    BYTE b4 = (num >> 24) & 255;
    num = (b1 << 24) & (b2 << 16) & (b3 << 8) & b4;
    return;
}