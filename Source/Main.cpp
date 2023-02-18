
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
#include "Util_Logger.h"
#include "MCP/CliqueSolver.h"

namespace hust {

std::vector<std::vector<int>> dynamicRun(hust::CommandLine& commandline) {

	Timer timer(commandline.runTimer);

	AlgorithmParameters& aps = commandline.aps;
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

	return bks.bestSolFound.saveToOutPut().rts;
}

std::vector<std::vector<int>> dimacsRun(hust::CommandLine& commandline) {

	Timer timer(commandline.runTimer);

	AlgorithmParameters& aps = commandline.aps;
	RandomTools randomTools(commandline.seed);

	Input input(&commandline, &aps, &randomTools, &timer);

	commandline.displayInfo();
	input.displayInfo();
	aps.displayInfo();

	// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
	aps.repairByCusCnt(input.custCnt);

	YearTable yearTable(&input);

	BKS bks(&input, &yearTable, input.timer);

	hust::Goal goal(&input, &aps, &bks, &yearTable);

	goal.TwoAlgCombine();
	return bks.bestSolFound.saveToOutPut().rts;
}

std::vector<std::vector<int>> routeNumberMinimization(hust::CommandLine& commandline) {

	Timer timer(commandline.runTimer);

	AlgorithmParameters& aps = commandline.aps;
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

	return bks.bestSolFound.saveToOutPut().rts;
}

std::vector<std::vector<int>> callSolverWithCustomizedInstance(
	const Input::DataModel& dm,
	CommandLine::SolverGoal solverGoal
) {

	using namespace hust;

	static std::vector< std::string>  argvStrings = {
			"./DLLSMA","-instancePath", "customizedInstance",
	};
	int argc = argvStrings.size();

	char** argv = new char* [argc];
	for (int i = 0; i < argc; ++i) {
		int strSize = argvStrings[i].size();
		argv[i] = new char[strSize + 1];
		strcpy(argv[i], argvStrings[i].c_str());
	}

	hust::CommandLine commandline(argc, argv);

	commandline.solverGoal = solverGoal;

	Timer timer(commandline.runTimer);
	AlgorithmParameters& aps = commandline.aps;
	aps.ejectLSMaxIter = IntInf;
	RandomTools randomTools(commandline.seed);

	Input input(&commandline, &aps, &randomTools, &timer, dm);

	commandline.displayInfo();
	input.displayInfo();
	aps.displayInfo();

	// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
	aps.repairByCusCnt(input.custCnt);

	YearTable yearTable(&input);

	BKS bks(&input, &yearTable, &timer);

	hust::Goal goal(&input, &aps, &bks, &yearTable);

	goal.experimentRouteNumberMinimization();

	if (commandline.solverGoal == hust::CommandLine::SolverGoal::DynamicRoutesDistance) {
		return hust::dynamicRun(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesDistance) {
		return hust::dimacsRun(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumber) {
		return hust::routeNumberMinimization(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumberBound) {
		return { hust::getOneBound(commandline) };
	}
	else {
		hust::Logger::ERROR("unknow solver goal of running");
	}
	return {};
}

}

void exampleUse() {

	int64_t num_vehicles = 2;
	int64_t num_customers = 1;
	int64_t vehicles_capacity = 100;

	std::vector<std::vector<int64_t>> time_matrix = {
		{0,5},
		{5,0}
	};

	std::vector<std::pair<int64_t, int64_t>> time_windows = {
		{0,20},
		{10,40}
	};

	std::vector<std::pair<int64_t, int64_t>> coordinates = {
		{0,0},
		{3,4}
	};

	std::vector<int64_t> demands = { 0,20 };
	std::vector<int64_t> service_times = { 0,10 };

	hust::Input::DataModel dm{ num_vehicles,num_customers,vehicles_capacity,time_matrix,time_windows,coordinates,demands,service_times };
	auto ret = callSolverWithCustomizedInstance(dm, hust::CommandLine::SolverGoal::StaticRoutesDistance);

	for (auto& i : ret) {
		for (auto& j : i) {
			std::cout << j << " ";
		}
		std::cout << std::endl;
	}
}

int main(int argc, char* argv[])
{
	hust::CommandLine commandline(argc, argv);
	if (commandline.solverGoal == hust::CommandLine::SolverGoal::DynamicRoutesDistance) {
		hust::dynamicRun(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesDistance) {
		hust::dimacsRun(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumber) {
		hust::routeNumberMinimization(commandline);
	}
	else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumberBound) {
		hust::getOneBound(commandline);
	}
	else {
		hust::Logger::ERROR("unknow solver goal of running");
		return 0;
	}
	
	return 0;
}