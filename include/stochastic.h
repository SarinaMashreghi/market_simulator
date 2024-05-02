#ifndef STOCHASTIC_HEADER_H
#define STOCHASTIC_HEADER_H

#include <bits/stdc++.h>
#include <cmath>
#include <random>

using namespace std;

class stochastic {
public:
  vector<double> symmetric_random_walk(int time_steps);
  vector<vector<double>> symmetric_random_walk(int num_simulations,
                                               int time_steps);

  vector<double> brownian_motion(int steps, double time);
  vector<vector<double>> brownian_motion(int num_simulations, int steps,
                                         double time);
  vector<double> GBM(double initial_value, double mu, double sigma, int steps,
                     double time);
  // vector<vector<double>> GBM(int num_simulations, double initial_value,
  //                            double mu, double sigma, int steps, double
  //                            time);
};

#endif // STOCHASTIC_HEADER_H
