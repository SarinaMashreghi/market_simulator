#ifndef TOOLS_HEADER_H
#define TOOLS_HEADER_H

#include <bits/stdc++.h>

using namespace std;

class tools {
public:
  double mean(vector<double> &values);
  double variance(vector<double> &values);
  pair<double, double> log_return_analysis(vector<double> &values, double dt);

  // void export_csv(unordered_map<string, vector<double>> &values);
};

#endif // TOOLS_HEADER_H
