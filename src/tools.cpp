#include "../include/tools.h"
#include <numeric>
#include <stdexcept>

// statistical analysis tools used in other classes

double tools::mean(vector<double> &values) {
  /* returns the mean of a 1D array of values */

  return double(accumulate(values.begin(), values.end(), 0.0)) /
         (values.size() - 1);
}

double tools::variance(vector<double> &values) {
  /* returns the variance of a 1D array of values*/

  double m = mean(values);
  double var = 0;

  for (auto val : values)
    var += pow(val - m, 2);
  return var / (values.size() - 1);
}

pair<double, double> tools::log_return_analysis(vector<double> &values,
                                                double dt) {
  /* returns the drift and the volatility of a GBM using the log return of the
   * values */
  if (values.size() < 2)
    return {0, 0};

  vector<double> log_return(values.size() - 1);
  for (int i = 0; i < log_return.size(); i++) {
    // calculate the logarithm of the return at each step
    log_return[i] = log(values[i + 1] / values[i]);
  }

  double mean_return = mean(log_return);
  double sample_var = variance(log_return);

  double estimated_vol = sqrt(sample_var / dt);
  double estimated_drift = (mean_return + pow(estimated_vol, 2) / 2) / dt;
  return {estimated_drift, estimated_vol};
}
