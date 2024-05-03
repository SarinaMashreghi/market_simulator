#include "include/exchange.h"
#include "include/option_pricer.h"
#include "include/simulations.h"
#include "include/stochastic.h"

int main(int argc, char **argv) {

  simulation sim = simulation();

  if (strcmp(argv[1], "OPTION_CALCULATOR") == 0) {
    bool run = 1;
    while (run) {
      sim.option_price_calculator();
      cout << "Do you want to calculate the price of another option? (1 for "
              "yes, 0 for no) ";
      cin >> run;
    }
  } else if (strcmp(argv[1], "MARKET") == 0) {
    sim.market_simulation();
  }
}
