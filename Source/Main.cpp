
#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Util_Common.h"

#include "Util_Arr2D.h"
#include "CommandLine.h"
#include "YearTable.h"
#include "LowerBoundOfRouteNum.h"
#include "MCP/CliqueSolver.h"

namespace hust {

void dimacsRun(int argc, char* argv[]) {

	hust::CommandLine commandline(argc, argv);
	Timer timer(commandline.runTimer);

	AlgorithmParameters aps = commandline.aps;
	RandomTools randomTools(commandline.seed);

	Input input(&commandline, &aps, &randomTools, &timer);

	commandline.displayInfo();
	input.displayInfo();
	aps.displayInfo();

	// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
	aps.repairByCusCnt(input.custCnt);

	YearTable yearTable(&input);

	BKS bks(&input, &yearTable, &timer);

	hust::Goal goal(&input, &aps, &bks, &yearTable);

	//goal.callSimulatedannealing();
	goal.TwoAlgCombine();
	//goal.test();
	//goal.experOnMinRN();

}

void routeNumberMinimization(int argc, char* argv[]) {

	hust::CommandLine commandline(argc, argv);
	Timer timer(commandline.runTimer);

	AlgorithmParameters aps = commandline.aps;
	aps.ejectLSMaxIter = IntInf;

	RandomTools randomTools(commandline.seed);

	Input input(&commandline, &aps, &randomTools, &timer);

	commandline.displayInfo();
	input.displayInfo();
	aps.displayInfo();

	// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
	aps.repairByCusCnt(input.custCnt);

	YearTable yearTable(&input);

	BKS bks(&input, &yearTable, &timer);

	hust::Goal goal(&input, &aps, &bks, &yearTable);

	goal.experimentRouteNumberMinimization();

}
}
int main(int argc, char* argv[])
{
	
	hust::dimacsRun(argc,argv);
	//hust::routeNumberMinimization(argc,argv);

	//hust::getOneBound("C2_4_3");
	return 0;
}