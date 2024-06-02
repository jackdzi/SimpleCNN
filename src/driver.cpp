#include "convertData.h"

using std::cout;

int main(int argc, char **argv) {
    std::string prefix = directoryPrefix();
    if (prefix.substr(prefix.size()-6, 6) == "\\build")
        prefix = prefix.substr(0, prefix.size()-6); //because I can't figure out cmake
    std::string train_path = prefix + "\\train_images";
    cout << train_path << std::endl;
    cout << train_path + "\\train-labels.idx3-ubyte";
    auto matrix = readFile(train_path + "\\train-images.idx3-ubyte");
    if (matrix.has_value()) {
        if (std::holds_alternative<arma::dmat>(matrix.value())) {
            cout << std::endl;
            for (int i = 0; i < 28; i++) {
                for (int j = 0; j < 28; j++) {
                    if (std::get<arma::dmat>(matrix.value())(1, i*28+j) > 0.5)
                        cout << 1;
                    else
                        cout << 0;
                }
                cout << std::endl;
            }
        }
    }
    return 0;
}