#include "include/option_pricer.h"
#include "include/stochastic.h"

int main() {
  /*
stochastic s = stochastic();

vector<vector<double>> bm = s.GBM(2, 10, 0.1, 0.1, 10, 1);

for (auto v : bm) {
for (auto val : v)
cout << val << endl;
} */
  option_pricer pricer = option_pricer();
  double tmp = pricer.monte_carlo_european(100000, 100, 100, 0.1, 0.1, 100, 1,
                                           'C', 0.99);
  cout << tmp << endl;
}
