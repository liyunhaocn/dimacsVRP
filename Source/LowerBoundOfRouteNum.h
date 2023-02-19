
#ifndef CN_HUST_LYH_LOWERBOUND_OF_ROUTE_NUM_H
#define CN_HUST_LYH_LOWERBOUND_OF_ROUTE_NUM_H

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Util_Common.h"

#include "Util_Arr2D.h"
#include "CommandLine.h"
#include "MCP/CliqueSolver.h"

namespace hust {

	std::vector<int> getOneBound(hust::CommandLine& commandline);

	void getAllInstancesBound();

}

#endif // CN_HUST_LYH_LOWERBOUND_OF_ROUTE_NUM_H