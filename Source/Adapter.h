#ifndef CN_HUST_LYH_ADAPTER_H
#define CN_HUST_LYH_ADAPTER_H

#include "Goal.h"
#include "Util_Common.h"

#include "Util_Arr2D.h"
#include "CommandLine.h"
#include "YearTable.h"
#include "LowerBoundOfRouteNum.h"
#include "Util_Logger.h"
#include "MCP/CliqueSolver.h"

namespace hust {
	
	std::vector<std::vector<int>> dynamicRun(hust::CommandLine& commandline);

	std::vector<std::vector<int>> dimacsRun(hust::CommandLine& commandline);

	std::vector<std::vector<int>> routeNumberMinimization(hust::CommandLine& commandline);

	std::vector<std::vector<int>> callSolverWithCustomizedInstance(
		const Input::DataModel& dm,
		CommandLine::SolverGoal solverGoal
	);

}

#endif // CN_HUST_LYH_ADAPTER_H