#include "../include/option_pricer.h"
#include "../include/stochastic.h"
#include <stdexcept>

double option_pricer::european_payoff(double price, double strike, char option_type){
  /* returns the value of the European option at maturity time based on option type, strike price and stock price */

  if(option_type == 'C') return price > strike ? price - strike : 0;  // call option: maximum of 0 and current price - strike price 
  if(option_type == 'P') return strike > price ? strike - price : 0;  // put option: maximum of 0 and strike price - current price 

  throw invalid_argument("Invalid option type.");
}

double option_pricer::monte_carlo_european(int num_simulations, double strike_price, 
                                           double initial_price, double mu, double sigma, 
                                           int time_steps, double maturity_time, 
                                           char option_type, double discount_factor) {

  time_t start, end;
  time(&start);


  vector<future<vector<double>>> futures;
  futures.reserve(num_simulations);
  stochastic s = stochastic();

  for (int i = 0; i < num_simulations; i++) {
    // generate GBM samples asynchronously
    futures.push_back(async(&stochastic::GBM, &s, initial_price, mu, sigma,
                            time_steps, maturity_time));
  }

  vector<double> discounted_payoffs;
  discounted_payoffs.reserve(num_simulations);

  for (auto &f : futures) {
    double final_price = f.get().back(); // retrieve the final price for each path
    double payoff = european_payoff(final_price, strike_price, option_type);
    discounted_payoffs.push_back(payoff*discount_factor);

  }
  
  double option_price = accumulate(discounted_payoffs.begin(), discounted_payoffs.end(), 0.0) / num_simulations;

  time(&end);
  cout << "time: " << end-start << endl;

  return option_price;
}
