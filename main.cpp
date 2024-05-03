#include "include/exchange.h"
#include "include/option_pricer.h"
#include "include/simulations.h"
#include "include/stochastic.h"
#include <chrono>

int main() {
  /*
stochastic s = stochastic();

vector<vector<double>> bm = s.GBM(2, 10, 0.1, 0.1, 10, 1);

for (auto v : bm) {
for (auto val : v)
cout << val << endl;
} */
  simulation sim = simulation();
  sim.market_simulation();
  /*
  Exchange exc(100, 0.1, 0.1, 0.06, 1);
  exc.update_price();
  while (true) {
    cout << exc.get_price() << endl;
    this_thread::sleep_for(chrono::seconds(1));
  } */
  // vector<double> tmp = exc.get_data(1, 100);
  // for (auto n : tmp)
  //  cout << n << endl;
  option_pricer pricer = option_pricer();
  double bsm = pricer.black_scholes_merton(100, 100, 1, 0.06, 0, 0.3, 'C');
  cout << "bsm " << bsm << endl;
  double mc = pricer.monte_carlo_european(100000, 100, 100, 0.06, 0.3, 100, 1,
                                          'C', 0.94);
  cout << mc << endl;
  double bin =
      pricer.binomial_model_european(100, 100, 0.06, 0.3, 10000, 1, 'C', "CRR");
  cout << bin << endl;
}
