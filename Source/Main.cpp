
#include<iostream>
#include<vector>
#include "Goal.h"

//../Instances/Homberger/C1_8_2.txt ../SolverParameters.json ../Results 
// bks 24974.1

int main(int argc, char* argv[])
{
	using namespace hust;

	CommandLine commandLine(argc, argv);

	Json parameters;
	std::ifstream ifs(commandLine.parametersPath);
	if (ifs.fail()) {
		Logger::INFO("can't open parameters file,use the default parameters");
	}
	else {
		Json allJson;
		ifs >> allJson;
		Json apsJson = allJson["AlgorithmParameters"];
		// initial commandLine with Josn
		commandLine.from_json(allJson, commandLine);
	}

	Input input(&commandLine);

	commandLine.check();
	commandLine.aps.check(input.customerNumer);

	commandLine.displayInfo();
	input.displayInfo();
	commandLine.aps.displayInfo();

	Goal goal(&input);

	goal.run();

	return 0;
}