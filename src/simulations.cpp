#include "../include/simulations.h"
#include <ostream>

void simulation::market_simulation(int duration) {
  Exchange exchange(100, 0.1, 0.1, 0.06, 0.1);
  Client client(exchange, 1000);

  exchange.start();
  client.start_listening();

  this_thread::sleep_for(chrono::seconds(duration));

  client.stop_listening();
  exchange.stop();

  vector<double> price = client.get_price_history();

  for (auto p : price)
    cout << p << endl;
}

void simulation::option_price_calculator() {
  double initial_price;
  double strike_price;
  double risk_free_interest;
  double volatility;
  double expiry_time;
  char option_type;

  cout << "Initial price: ";
  cin >> initial_price;
  cout << "Strike price: ";
  cin >> strike_price;
  cout << "Time to maturity (years): ";
  cin >> expiry_time;
  cout << "Annual risk-free interest rate (%): ";
  cin >> risk_free_interest;
  cout << "Annualized volatility (%): ";
  cin >> volatility;
  cout << "Option type ('C' for call, 'P' for put): ";
  cin >> option_type;

  // from percentage to (0,1) range
  risk_free_interest /= 100.0;
  volatility /= 100.;

  int num_steps_bin;
  int num_steps_mc;
  int num_simulations;

  cout << "Number of time steps in the binomial model: ";
  cin >> num_steps_bin;
  cout << "Number of time steps in the Monte Carlo model: ";
  cin >> num_steps_mc;
  cout << "Number of simulations in the Monte Carlo model: ";
  cin >> num_simulations;

  option_pricer pricer = option_pricer();

  double bsm = pricer.black_scholes_merton(initial_price, strike_price,
                                           expiry_time, risk_free_interest, 0,
                                           volatility, option_type);

  double binomial_crr = pricer.binomial_model_european(
      strike_price, initial_price, risk_free_interest, volatility,
      num_steps_bin, expiry_time, option_type, "CRR");
  double binomial_jr = pricer.binomial_model_european(
      strike_price, initial_price, risk_free_interest, volatility,
      num_steps_bin, expiry_time, option_type, "JR");

  double mc = pricer.monte_carlo_european(
      num_simulations, strike_price, initial_price, risk_free_interest,
      volatility, num_steps_mc, expiry_time, option_type,
      1 / (1 + risk_free_interest));

  cout << "\nBlack-Scholes-Merton Model: " << bsm << endl
       << "Binomial Model (CRR method): " << binomial_crr << endl
       << "Binomial Model (JR method): " << binomial_jr << endl
       << "Monte Carlo Model: " << mc << endl;
}
