#include "./headers.h"

std::optional<std::variant<vector<image>, vector<unsigned int>>>
readFile(std::string path); // use auto [dmat, uvec]i
vector<image> imageToDmat(std::ifstream &file);
std::vector<unsigned int> labelToUvec(std::ifstream &file);
void endianSwitch(int &num);
std::string directoryPrefix();
