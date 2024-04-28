#ifndef OPTION_PRICER_HEADER_H
#define OPTION_PRICER_HEADER_H

#include <bits/stdc++.h>

class option_pricer {
public:
  option_pricer(){};

  double european_payoff(double price, double strike, char option_type);
  double monte_carlo_european(int num_simulations, double strike_price,
                              double initial_price, double mu, double sigma,
                              int time_steps, double maturity_time,
                              char option_type, double discount_factor);
};

#endif // OPTION_PRICER_H
