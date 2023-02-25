
#include<iostream>
#include<vector>


#include "Goal.h"

//../Instances/Homberger/C1_8_2.txt ../SolverParameters.json ../Results

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
	commandLine.displayInfo();
	input.displayInfo();
	commandLine.aps.displayInfo();

	commandLine.aps.check(input.custCnt);

	Goal goal(&input);

	goal.run();

	return 0;
}