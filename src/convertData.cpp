#include "convertData.h"

std::optional<std::pair<arma::dmat, arma::uvec>> readFile(std::string path) {
    std::ifstream file(path, ios::binary);
    arma::umat image_data;
    std::pair<arma::dmat, arma::uvec> res;
    if (file.is_open()) {
        res.first = imageToDmat(file);
        file.seekg(0, ios::beg);
        res.second = labelToUvec(file);
        return res;
    } else {
        std::cout << "File could not be read";
        return {};
    }
}

