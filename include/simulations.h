#ifndef SIMULATIONS_HEADER_H
#define SIMULATIONS_HEADER_H

#include "client.h"
#include "exchange.h"
#include "option_pricer.h"
#include <bits/stdc++.h>

class simulation {
public:
  void market_simulation();
  void option_price_calculator();
};

#endif // SIMULATIONS_HEADER_H
