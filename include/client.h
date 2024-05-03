#ifndef CLIENT_HEADER_H
#define CLIENT_HEADER_H

#include "exchange.h"
#include "option_pricer.h"
#include <bits/stdc++.h>
class Client {

public:
  Client(const Exchange &exchange, int time_step);
  void start_listening();
  void stop_listening();
  void fetch_price();

  vector<double> get_price_history();

private:
  const Exchange &exchange;
  vector<double> m_price_history;
  thread client_thread;
  bool continue_listening;
  int m_time_step;
};

#endif // CLIENT_HEADER_H
