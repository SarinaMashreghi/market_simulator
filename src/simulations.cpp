#include "../include/simulations.h"
#include <ostream>

void simulation::market_simulation() {
  /* Market simulation */
  tools tool;
  int duration;
  cout << "Simulation duration (seconds): ";
  cin >> duration;
  // set exchange parameters
  double initial_price, drift, volatility, time_steps;
  cout << "\nSet market parameters: \nStarting price ($): ";
  cin >> initial_price;
  cout << "Annual drift/risk-free interest rate (%): ";
  cin >> drift;
  cout << "Annual volatility (%): ";
  cin >> volatility;
  cout << "Time step length (seconds): ";
  cin >> time_steps;

  drift /= 100.0 * 365 * 24 * 3600; // from percentage to (0,1) range
  volatility /= 100.0 * 365 * 24 * 3600;

  Exchange exchange(initial_price, drift, volatility, time_steps);
  Client client(exchange, int(time_steps * 1000)); // time_step in milliseconds

  exchange.start();
  client.start_listening();
  cout << "Simulating ... " << endl;

  this_thread::sleep_for(chrono::seconds(duration));

  client.stop_listening();
  exchange.stop();

  vector<double> price_path = client.get_price_history();
  pair<double, double> log_return_factors =
      tool.log_return_analysis(price_path, time_steps);

  cout << "\nEstimated drift: " << setw(15) << setprecision(3)
       << log_return_factors.first * 100 * 3600 * 24 * 365 << "%" << endl
       << "Estimated volatility: " << setw(10) << setprecision(3)
       << log_return_factors.second * 100 * 3600 * 24 * 365 << "%" << endl;
}

void simulation::option_price_calculator() {
  /* Option price calculation */
  double initial_price;
  double strike_price;
  double risk_free_interest;
  double volatility;
  double expiry_time;
  char option_type;

  cout << "\nCurrent price of underlying asset ($): ";
  cin >> initial_price;
  cout << "Strike price ($): ";
  cin >> strike_price;
  cout << "Time to maturity (years): ";
  cin >> expiry_time;
  cout << "Option type ('C' for call, 'P' for put): ";
  cin >> option_type;
  cout << "Annual risk-free interest rate (%): ";
  cin >> risk_free_interest;
  cout << "Annual volatility (%): ";
  cin >> volatility;

  // from percentage to (0,1) range
  risk_free_interest /= 100.0;
  volatility /= 100.;

  int num_steps_bin;
  int num_steps_mc;
  int num_simulations;

  cout << "\nNumber of periods in the binomial model: ";
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
      volatility, num_steps_mc, expiry_time, option_type, risk_free_interest);

  // if the value of the option is less than 1 cent, round it to 0
  bsm = bsm < 0.01 ? 0 : bsm;
  binomial_crr = binomial_crr < 0.01 ? 0 : binomial_crr;
  binomial_jr = binomial_jr < 0.01 ? 0 : binomial_jr;
  mc = mc < 0.01 ? 0 : mc;

  cout << "\n----------------------------------------\n"
       << "Black-Scholes-Merton Model: " << setw(11) << setprecision(3) << bsm
       << "$" << endl
       << "Binomial Model (CRR method): " << setw(10) << setprecision(3)
       << binomial_crr << "$" << endl
       << "Binomial Model (JR method): " << setw(11) << setprecision(3)
       << binomial_jr << "$" << endl
       << "Monte Carlo Model: " << setw(20) << setprecision(3) << mc << "$"
       << endl
       << "----------------------------------------\n"
       << endl;
}
