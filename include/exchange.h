#ifndef EXCHANGE_HEADER_H
#define EXCHANGE_HEADER_H

#include "stochastic.h"
#include "tools.h"
#include <random>

class Exchange {
public:
  Exchange(){};
  Exchange(double drift, double volatility, double interest);
  vector<double> get_data(double end_time, int steps);

private:
  stochastic m_stochastic_gen;
  tools m_tools;

  double m_drift;
  double m_volatility;
  double m_interest_rate;
};

#endif // EXCHANGE_HEADER_H
