#include "../include/option_pricer.h"
#include <stdexcept>

double option_pricer::european_payoff(double price, double strike,
                                      char option_type) {
  /* returns the value of the European option at maturity time based on option
   * type, strike price and stock price */

  if (option_type == 'C')
    return max(
        price - strike,
        0.0); // call option: maximum of 0 and current price - strike price
  if (option_type == 'P')
    return max(
        strike - price,
        0.0); // put option: maximum of 0 and strike price - current price

  throw invalid_argument("Invalid option type.");
}

double option_pricer::monte_carlo_european(
    int num_simulations, double strike_price, double initial_price, double mu,
    double sigma, int time_steps, double maturity_time, char option_type,
    double discount_factor) {

  /* Using Monte Carlo simulations for risk-neutral pricing by generating a
   * large number of GBM paths and evaluating the payoff for each. Accroding to
   * the risk-neutral pricing formula, the initial price of the option should be
   * equal to the expected value of these payoffs.
   *
   * Since generating the data for large numbers of paths is computationally
   * expensive, this method uses std::async to run stochastic::GBM
   * asynchronously.
   * */

  // std::async runs tasks asynchronously and returns a std::future object that
  // eventually holds the result
  vector<future<vector<double>>> futures;
  futures.reserve(num_simulations);

  for (int i = 0; i < num_simulations; i++) {
    // generate GBM samples asynchronously
    futures.push_back(async(&stochastic::GBM, &this->m_stochastic_gen,
                            initial_price, mu, sigma, time_steps,
                            maturity_time));
  }

  vector<double>
      discounted_payoffs; // stores the discounted payoff for each simulation
  discounted_payoffs.reserve(num_simulations);

  for (auto &f : futures) {
    double final_price = f.get().back(); // retrieve the final price for each
                                         // path from the std::future object
    double payoff = european_payoff(
        final_price, strike_price,
        option_type); // calculate the final payoff for each path
    discounted_payoffs.push_back(payoff * discount_factor);
  }

  // the initial price should be the expected value of the payoffs at expiry,
  // estimated by cacluating the sample mean of discounted payoffs of the
  // simulations
  double option_price = this->m_tools.mean(discounted_payoffs);

  return option_price;
}

double option_pricer::binomial_model_european(double strike_price,
                                              double initial_price,
                                              double interest, double sigma,
                                              int periods, double maturity_time,
                                              char option_type, string method) {

  /* Computes option pirce through binomial asset pricing model.
   * The method uses backward recursion to compute the value at time step n
   * based on values at time step n+1. The values at the last time step are
   * calculated based by the option payoff formula. Uses multi-threading to
   * improve performance
   * */

  double dt = maturity_time / periods; // lenght of each time step
  double discount_factor = exp(-1 * dt * interest);

  vector<double> variables;
  if (method == "CRR")
    variables = CRR(sigma, dt, interest);
  else if (method == "JR")
    variables = JR(sigma, dt, interest);
  else
    throw invalid_argument("Invalid method for binomial model.");

  double up_factor = variables[0], down_factor = variables[1],
         p_risk_neutral = variables[2], q_risk_neutral = variables[3];
  cout << "u " << up_factor << " d " << down_factor << endl;
  cout << "p " << p_risk_neutral << " q " << q_risk_neutral << endl;
  // prices at maturity time
  vector<double> price(periods + 1);

#pragma omp parallel for
  for (int i = 0; i <= periods; i++) {
    price[i] =
        initial_price * pow(down_factor, i) * pow(up_factor, periods - i);
  }

  vector<double> option_value1(periods + 1);
  vector<double> option_value2(periods + 1);

  for (int i = 0; i <= periods; i++) {
    option_value1[i] = european_payoff(price[i], strike_price, option_type);
  }

  bool toggle = true;

  for (int i = periods - 1; i >= 0; i--) {
#pragma omp parallel for
    for (int j = 0; j < i; j++) {
      if (toggle) {
        option_value2[j] =
            discount_factor * (p_risk_neutral * option_value1[j] +
                               q_risk_neutral * option_value1[j + 1]);
      } else {
        option_value1[j] =
            discount_factor * (p_risk_neutral * option_value2[j] +
                               q_risk_neutral * option_value2[j + 1]);
      }
    }
    toggle = !toggle;
  }
  cout << "1 " << option_value1[0] << " 2 " << option_value2[0] << endl;
  if (toggle)
    return option_value1[0];
  return option_value2[0];
}

// methods for choosing parameters in binomial model

vector<double> option_pricer::CRR(double sigma, double dt, double interest) {
  // Cox, Ross and Rubinstein method
  double up_factor = exp(sigma * sqrt(dt));
  double down_factor = 1 / up_factor;
  double p_risk_neutral =
      (exp(interest * dt) - down_factor) / (up_factor - down_factor);
  double q_risk_neutral = 1 - p_risk_neutral;

  return {up_factor, down_factor, p_risk_neutral, q_risk_neutral};
}

vector<double> option_pricer::JR(double sigma, double dt, double interest) {
  // Jarrow and Rudd method
  double q_risk_neutral = 0.5;
  double p_risk_neutral = 0.5;

  double nu = interest - sigma * sigma / 2;
  double up_factor = exp(nu * dt + sigma * sqrt(dt));
  double down_factor = exp(nu * dt - sigma * sqrt(dt));

  return {up_factor, down_factor, p_risk_neutral, q_risk_neutral};
}
