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

double tools::log_return_volatility(vector<double> &values, double dt) {
  /* returns the volatility of the log return of a series of values */

  vector<double> log_return(values.size() - 1);
  for (int i = 0; i < log_return.size(); i++)
    log_return[i] =
        log(values[i + 1] /
            values[i]); // calculate the logarithm of the return at each step
  double mean_return = mean(log_return);
  double sample_var = variance(log_return);
  double estimated_vol = sqrt(sample_var / dt);
  return estimated_vol;
}
