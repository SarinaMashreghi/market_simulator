#include "../include/stochastic.h"
#include <random>

vector<vector<double>> stochastic::random_walk_symmetric(int num_simulations,
                                                         int time) {
  /* returns sample paths of symmetric random walks in a 2D vector */
  vector<vector<double>> res(num_simulations, vector<double>(time));

  random_device rd;
  mt19937 gen(rd());
  bernoulli_distribution d(0.5);
  auto trial = [&d, &gen] { return d(gen); };

  for (int i = 0; i < num_simulations; i++) {
    res[i][0] = 0;
    for (int j = 1; j < time; j++) {
      if (trial())
        res[i][j] = res[i][j - 1] + 1;
      else
        res[i][j] = res[i][j - 1] - 1;
    }
  }
  return res;
}

vector<vector<double>> stochastic::GBM(int num_simulations, double initial_val,
                                       double drift, double var, int n_steps,
                                       int time) {
  /* returns sample paths of Geometric Brownian Motion in a 2D vector */

  double dt = double(time) / n_steps;

  random_device rd{};
  mt19937 gen{rd()};

  normal_distribution<double> d(0, sqrt(dt));
  auto random_val = [&d, &gen] { return d(gen); };

  vector<vector<double>> res(num_simulations,
                             vector<double>(n_steps, initial_val));

  for (int i = 0; i < num_simulations; i++) {
    for (int j = 1; j < n_steps; j++) {
      res[i][j] = res[i][j - 1] *
                  exp((drift - pow(var, 2) / 2) * dt + var * random_val());
    }
  }

  return res;
}
