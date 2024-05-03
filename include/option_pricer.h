#ifndef OPTION_PRICER_HEADER_H
#define OPTION_PRICER_HEADER_H

#include "stochastic.h"
#include "tools.h"
#include <bits/stdc++.h>

class option_pricer {
public:
  option_pricer(){};

  double european_payoff(double price, double strike, char option_type);
  double monte_carlo_european(int num_simulations, double strike_price,
                              double initial_price, double mu, double sigma,
                              int time_steps, double maturity_time,
                              char option_type, double risk_free_interest);

  double binomial_model_european(double strike_price, double initial_price,
                                 double interest, double sigma, int periods,
                                 double maturity_time, char option_type,
                                 string method);

  vector<double> CRR(double sigma, double dt, double disc);
  vector<double> JR(double sigma, double dt, double interest);
  double black_scholes_merton(double S, double K, double T, double r, double q,
                              double v, char opt_type);

private:
  stochastic m_stochastic_gen; // to generate stochastic processes
  tools m_tools;               // providing statistical analysis functions
};

#endif // OPTION_PRICER_H
