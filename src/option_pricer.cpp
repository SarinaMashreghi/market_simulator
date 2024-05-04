#include "../include/option_pricer.h"
#include <algorithm>
#include <stdexcept>

double option_pricer::european_payoff(double price, double strike,
                                      char option_type) {
  /* returns the value of the European option at maturity time based on option
   * type, strike price and stock price */

  if (option_type == 'C' || option_type == 'c')
    return max(
        price - strike,
        0.0); // call option: maximum of 0 and current price - strike price
  if (option_type == 'P' || option_type == 'p')
    return max(
        strike - price,
        0.0); // put option: maximum of 0 and strike price - current price

  throw invalid_argument("Invalid option type.");
}

double option_pricer::monte_carlo_european(
    int num_simulations, double strike_price, double initial_price, double mu,
    double sigma, int time_steps, double maturity_time, char option_type,
    double risk_free_interest) {

  /* Using Monte Carlo simulations for risk-neutral pricing by generating a
   * large number of GBM paths and evaluating the payoff for each. Accroding to
   * the risk-neutral pricing formula, the initial price of the option should be
   * equal to the expected value of these payoffs.
   *
   * Since generating the data for large numbers of paths is computationally
   * expensive, this method uses std::async to run stochastic::GBM
   * asynchronously.
   * */

  double dt = maturity_time / time_steps;
  double discount_factor = exp(-risk_free_interest * dt);
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
  // prices at maturity time
  vector<double> price(periods + 1);

#pragma omp parallel for
  for (int i = 0; i <= periods; i++) {
    price[i] =
        initial_price * pow(down_factor, i) * pow(up_factor, periods - i);
  }

  // enable parallel computation of the price using two vectors for storing
  // price values
  vector<double> option_value1(periods + 1);
  vector<double> option_value2(periods + 1);

  for (int i = 0; i <= periods; i++) {
    option_value1[i] = european_payoff(price[i], strike_price, option_type);
  }

  bool toggle = true; // determines which vector stores the current period (for
                      // read operations) and which stores the data for pervious
                      // period (for write operations)

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
    toggle = !toggle; // switch the other of the vectors
  }
  if (toggle)
    return option_value1[0];
  return option_value2[0];
}

// methods for choosing parameters in binomial model

vector<double> option_pricer::CRR(double sigma, double dt, double interest) {
  /* Cox, Ross and Rubinstein method for calculating the up and down factor and
   * risk-neutral probabilities */
  double up_factor = exp(sigma * sqrt(dt));
  double down_factor = 1 / up_factor;
  double p_risk_neutral =
      (exp(interest * dt) - down_factor) / (up_factor - down_factor);
  double q_risk_neutral = 1 - p_risk_neutral;

  return {up_factor, down_factor, p_risk_neutral, q_risk_neutral};
}

vector<double> option_pricer::JR(double sigma, double dt, double interest) {
  /* Jarrow and Rudd method for calculating the up and down factor and
   * risk-neutral probabilities */
  double q_risk_neutral = 0.5;
  double p_risk_neutral = 0.5;

  double nu = interest - sigma * sigma / 2;
  double up_factor = exp(nu * dt + sigma * sqrt(dt));
  double down_factor = exp(nu * dt - sigma * sqrt(dt));

  return {up_factor, down_factor, p_risk_neutral, q_risk_neutral};
}

// helper functions used in the Black-Scholes-Merton model
// source:
// (https://sites.google.com/view/vinegarhill-financelabs/black-scholes-merton)

// N(0,1) density

double f(double x) {

  double pi = 4.0 * atan(1.0);

  return exp(-x * x * 0.5) / sqrt(2 * pi);
}

double Boole(double StartPoint, double EndPoint, int n) {

  vector<double> X(n + 1, 0.0);
  vector<double> Y(n + 1, 0.0);

  double delta_x = (EndPoint - StartPoint) / double(n);

#pragma omp parallel for
  for (int i = 0; i <= n; i++) {
    X[i] = StartPoint + i * delta_x;
  }

  transform(X.begin(), X.end(), Y.begin(), f);

  double sum = 0;

  for (int t = 0; t <= (n - 1) / 4; t++) {

    int ind = 4 * t;

    sum += (1 / 45.0) *
           (14 * Y[ind] + 64 * Y[ind + 1] + 24 * Y[ind + 2] +

            64 * Y[ind + 3] + 14 * Y[ind + 4]) *
           delta_x;
  }

  return sum;
}

// N(0,1) cdf by Boole's Rule

double N(double x) { return Boole(-10.0, x, 240); }

double option_pricer::black_scholes_merton(double S, double K, double T,
                                           double r, double q, double v,
                                           char opt_type) {
  /* Black-Scholes-Merton formula for option pricing*/

  double d = (log(S / K) + T * (r - q + 0.5 * v * v)) / (v * sqrt(T));
  double call = S * exp(-q * T) * N(d) - exp(-r * T) * K * N(d - v * sqrt(T));

  if (opt_type == 'C' || opt_type == 'c')
    return call;

  if (opt_type == 'P' || opt_type == 'p')
    return call - S * exp(-q * T) + K * exp(-r * T);

  throw invalid_argument("Invalid option type.");
}
