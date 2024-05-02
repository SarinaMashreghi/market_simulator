#include "../include/exchange.h"
#include <chrono>
#include <cmath>
#include <mutex>
#include <random>

Exchange::Exchange(double initial_price, double drift, double volatility,
                   double interest, double dt) {
  /* Initialize market parameters */
  this->current_price = initial_price;
  this->m_drift = drift;
  this->m_volatility = volatility;
  this->m_interest_rate = interest;
  this->m_dt = dt;
}

void Exchange::update_price() {
  thread([this]() {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> d(0, sqrt(m_dt));
    auto random_val = [&d, &gen] { return d(gen); };

    while (true) {
      lock_guard<mutex> guard(mtx);
      current_price =
          current_price * exp((m_drift - pow(m_volatility, 2) / 2) * m_dt +
                              m_volatility * random_val());
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
  }).detach();
}

double Exchange::get_price() {
  // lock_guard<mutex> guard(mtx);

  return current_price;
}
