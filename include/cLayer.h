#include <vector>
#include <cmath>
#include <functional>
#include <numeric>
#include <random>
using std::vector;

class cLayer {
private:
  int size;
public:
  vector<double> data;
  vector<vector<double>> weights;
  vector<double> activation;

  cLayer(int num_nodes, int prev_nodes);
  void loadFromLayer(vector<vector<double>> input, int dim_prev);
  void loadFromPrevious(cLayer input);
  int applyActivation(int application_type);
};
