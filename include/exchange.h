#ifndef EXCHANGE_HEADER_H
#define EXCHANGE_HEADER_H

#include "stochastic.h"
#include "tools.h"
#include <atomic>
#include <chrono>
#include <mutex>
#include <random>
#include <thread>

class Exchange {
public:
  Exchange(double initial_price, double drift, double volatility,
           double interest, double dt);

  void update_price();
  double get_price() const;

  void start();
  void stop();

private:
  stochastic m_stochastic_gen;
  tools m_tools;

  double m_drift;
  double m_volatility;
  double m_interest_rate;
  double m_dt;
  atomic<double> current_price;
  atomic<bool> is_running;
  thread exchange_thread;
};

#endif // EXCHANGE_HEADER_H
