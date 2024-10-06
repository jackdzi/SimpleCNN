#include "./convertData.h"
#include "./layer.h"

class Model {
private:
  std::vector<Layer> layers;
  Connected connected; //TODO: Implement fully connected layer, also implement returnProbabiilities method
public:
  Model();
  void forwardPropagate(vector<vector<double>> input);
  vector<double> returnProbabiilities();
};
