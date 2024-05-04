#include "../include/client.h"

Client::Client(const Exchange &exchange, int time_step)
    : exchange(exchange), m_time_step(time_step) {
  /* create a client instance */
  this->continue_listening = false;
}

void Client::fetch_price() {
  /* Continuously listens to the price given by the exchange thread */
  double current_price;

  while (this->continue_listening) {
    current_price = this->exchange.get_price();
    // cout << "price " << current_price << endl;
    m_price_history.push_back(current_price);

    this_thread::sleep_for(chrono::milliseconds(m_time_step));
  }
}

void Client::start_listening() {
  /* start the price listening process */
  continue_listening = true;
  client_thread = thread(&Client::fetch_price, this);
}

void Client::stop_listening() {
  /* stop the price listening process */
  continue_listening = false;
  client_thread.join();
}

vector<double> Client::get_price_history() {
  /* get the price history */
  return m_price_history;
}
