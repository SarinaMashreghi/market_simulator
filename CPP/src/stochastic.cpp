#include "../include/stochastic.h"
#include <random>

vector<double> stochastic::symmetric_random_walk(int time_steps) {
  /* returns a sample of a random walk */
  vector<double> result(time_steps, 0);

  // using random library to create a Bernoulli distribution
  random_device rd;
  mt19937 gen(rd());
  bernoulli_distribution d(
      0.5); // in a symmetric random walk, there is 50% chance of an upward
            // movement and 50% change of a downward movement
  auto trial = [&d, &gen] {
    return d(gen);
  }; // each trial generates a random variable from the distribution

  for (int i = 1; i < time_steps; i++) {
    // increment or decrement the previous value in the series based on the
    // outcome of the trial
    if (trial())
      result[i] = result[i - 1] + 1;
    else
      result[i] = result[i - 1] - 1;
  }

  return result;
}

vector<vector<double>> stochastic::symmetric_random_walk(int num_simulations,
                                                         int time_steps) {
  /* returns multiple sample paths of symmetric random walks in a 2D vector */

  vector<vector<double>> result(num_simulations);

  for (int i = 0; i < num_simulations; i++) {
    result[i] = symmetric_random_walk(time_steps);
  }

  return result;
}

vector<double> stochastic::brownian_motion(int steps, double time) {
  /* returns a sinlge sample series of Brownian motion */

  double dt = time / steps; // length of each time step
  vector<double> result(steps);
  result[0] = 0; // standard Brownian motion starts at 0

  // using random library to generate random variables from a normal
  // distribution
  random_device rd;
  mt19937 gen(rd());

  normal_distribution<double> d(
      0, sqrt(dt)); // increments are iid random variables from a normal
                    // distribution with mean 0 and variance dt
  auto random_val = [&d, &gen] { return d(gen); };

  for (int i = 1; i < steps; i++) {
    result[i] = result[i - 1] + random_val();
  }

  return result;
}

vector<vector<double>> stochastic::brownian_motion(int num_simulations,
                                                   int steps, double time) {
  /* returns multiple sample paths of Brownian motion in a 2D vector */

  vector<vector<double>> result(num_simulations);

  for (int i = 0; i < num_simulations; i++) {
    result[i] = brownian_motion(steps, time);
  }

  return result;
}

vector<double> stochastic::GBM(double initial_value, double mu, double sigma,
                               int steps, double time) {
  /* returns a sinlge sample series of Geometric Brownian Motion (GBM)
   * mu = drift factor
   * sigma = volatility
   */
  vector<double> bm = brownian_motion(steps, time); // a sample Brownian motion

  vector<double> result(steps);
  double dt = time / steps;
  int t = 0; // current time value
  for (int i = 0; i < steps; i++) {
    result[i] =
        initial_value * exp((mu - sigma * sigma / 2) * t + sigma * bm[i]);
    t += dt;
  }

  return result;
}

vector<vector<double>> stochastic::GBM(int num_simulations,
                                       double initial_value, double mu,
                                       double sigma, int steps, double time) {
  /* returns multiple sample paths of GBM in a 2D vector */

  vector<vector<double>> result(num_simulations);

  for (int i = 0; i < num_simulations; i++) {
    result[i] = GBM(initial_value, mu, sigma, steps, time);
  }

  return result;
}
