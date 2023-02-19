
#include<iostream>
#include<vector>

#include "Adapter.h"

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

	hust::Input::DataModel dm{
		num_vehicles,
		num_customers,
		vehicles_capacity,
		time_matrix,
		time_windows,
		coordinates,
		demands,
		service_times
	};
	
	using SolverGoal = hust::CommandLine::SolverGoal;
	SolverGoal solverGoal = SolverGoal::StaticRoutesDistance;
	//SolverGoal solverGoal = SolverGoal::StaticRoutesNumber;
	//SolverGoal solverGoal = SolverGoal::StaticRoutesNumberBound;
	//SolverGoal solverGoal = SolverGoal::DynamicRoutesDistance;

	auto routes = hust::callSolverWithCustomizedInstance(dm, solverGoal);

	if (solverGoal == SolverGoal::StaticRoutesNumberBound) {
		std::cout << "the route number bound get with mcp is " 
				  << routes[0][0] << std::endl;
	}
	else {
		std::cout << "routes:" << std::endl;
		for (auto& i : routes) {
			for (auto& j : i) {
				std::cout << j << " ";
			}
			std::cout << std::endl;
		}
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