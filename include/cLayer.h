#include "headers.h"

class cLayer {
public:
  int size;
  vector<double> data;
  vector<vector<double>> weights;
  vector<double> activation;

  cLayer(int num_nodes, int prev_nodes);
  void loadFromPrevious(cLayer input);
  void loadFromLayer(vector<image> pooled);
  int applyActivation(int application_type);
};
