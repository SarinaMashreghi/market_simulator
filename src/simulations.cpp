#include "../include/simulations.h"
#include <thread>

simulation::simulation() {}

void simulation::market_simulation() {
  Exchange exchange(100, 0.1, 0.1, 0.06, 0.1);
  Client client(exchange, 1000);

  exchange.start();
  client.start_listening();

  this_thread::sleep_for(chrono::seconds(10));

  client.stop_listening();
  exchange.stop();

  vector<double> price = client.get_price_history();

  for (auto p : price)
    cout << p << endl;
}
