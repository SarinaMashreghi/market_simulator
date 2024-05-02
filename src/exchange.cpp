#include "../include/exchange.h"
#include <random>

Exchange::Exchange(double drift, double volatility, double interest) {
  /* Initialize market parameters */
  this->m_drift = drift;
  this->m_volatility = volatility;
  this->m_interest_rate = interest;
}

vector<double> Exchange::get_data(double end_time, int steps) {
  /* Get the price paths for an asset, simulated by GBM */
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distr(10, 100);

  double initial_price = distr(gen);

  return m_stochastic_gen.GBM(initial_price, this->m_drift, this->m_volatility,
                              steps, end_time);
}
