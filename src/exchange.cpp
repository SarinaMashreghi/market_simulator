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
  this->is_running = false;
}

void Exchange::update_price() {
  thread([this]() {
    random_device rd;
    mt19937 gen(rd());
    normal_distribution<double> d(0, sqrt(m_dt));
    auto random_val = [&d, &gen] { return d(gen); };

    while (this->is_running) {
      current_price =
          current_price * exp((m_drift - pow(m_volatility, 2) / 2) * m_dt +
                              m_volatility * random_val());
      this_thread::sleep_for(chrono::milliseconds(1000));
    }
  }).detach();
}

double Exchange::get_price() const { return current_price.load(); }

void Exchange::start() {
  this->is_running = true;
  exchange_thread = thread(&Exchange::update_price, this);
}

void Exchange::stop() {
  this->is_running = false;
  exchange_thread.join();
}
