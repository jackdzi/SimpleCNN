#include "convertData.h"

using std::cout;

int main(int argc, char **argv) {
    std::string train_path = directoryPrefix();
    cout << train_path + "Hello" << std::endl;
    return 0;
}