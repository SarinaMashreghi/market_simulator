#include "include/stochastic.h"

int main() {
  stochastic s = stochastic();

  vector<vector<double>> bm = s.GBM(2, 10, 0.1, 0.1, 10, 1);

  for (auto v : bm) {
    for (auto val : v)
      cout << val << endl;
  }
}
