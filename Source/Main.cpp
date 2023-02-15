
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

void dynamicRun(hust::CommandLine& commandline) {

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

	//hust::Goal goal(&input, &aps, &bks, &yearTable);

	hust::Solver solver(&input,&yearTable,&bks);

	solver.dynamicRuin(aps.ruinC_);

	//goal.callSimulatedannealing();
	//goal.TwoAlgCombine();
	//goal.test();
	//goal.experOnMinRN();
}

void dimacsRun(hust::CommandLine& commandline) {

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

void routeNumberMinimization(hust::CommandLine& commandline) {

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
	hust::CommandLine commandline(argc, argv);
	if (commandline.isDynamicRun) {
		hust::dynamicRun(commandline);
	}
	else if (commandline.isDimacsRun) {
		commandline.instancePath = "readstdin";
		hust::dimacsRun(commandline);
	}
	else if (commandline.isMinimizeRouteNumberRun) {
		hust::routeNumberMinimization(commandline);
	}
	else if (commandline.isRouteNumberBoundRun) {
		hust::getOneBound(commandline);
	}
	else {
		hust::dimacsRun(commandline);
	}
	
	return 0;
}