//
// pch.h
//

#pragma once

#include <gtest/gtest.h>
#include "../../Source/Flag.h"
#include "../../Source/Solver.h"
#include "../../Source/YearTable.h"
#include "../../Source/EAX.h"
#include "../../Source/Algorithm_Parameters.h"
#include "../../Source/Problem.h"
#include "../../Source/CommandLine.h"
#include "../../Source/Util_Logger.h"

namespace hust {
namespace lyhtest {
	
std::vector< std::vector< std::string> > getTestCommandLineArgv();

struct SolverGenerator {
	
public:
	CommandLine* commandline = nullptr;
	Timer* timer = nullptr;
	AlgorithmParameters* aps = nullptr;
	RandomTools* randomTools = nullptr;
	Input* input = nullptr;
	YearTable* yearTable = nullptr;
	BKS* bks = nullptr;
	Solver* solver = nullptr;
	
	SolverGenerator() {}

	hust::Solver* generateSolver(const std::vector< std::string>& argvStrings) {

		using namespace hust;

		int argc = argvStrings.size();

		char** argv = new char* [argc];
		for (int i = 0; i < argc; ++i) {
			int strSize = argvStrings[i].size();
			argv[i] = new char[strSize+1];
			strcpy( argv[i], argvStrings[i].c_str());
		}
		commandline = new CommandLine(argc, argv);
		timer = new Timer(commandline->runTimer);

		aps = &commandline->aps;
		randomTools = new RandomTools(commandline->seed);

		input = new Input(commandline, aps, randomTools, timer);
		YearTable* yearTable = new YearTable(input);
		BKS* bks = new BKS(input, yearTable, timer);

		solver = new Solver(input, yearTable, bks);
		return solver;
	}

	~SolverGenerator() {
		if (commandline != nullptr) delete commandline;
		if (timer != nullptr) delete timer;
		//if (aps != nullptr) delete aps;
		if (randomTools != nullptr) delete randomTools;
		if (input != nullptr) delete input;
		if (yearTable != nullptr) delete yearTable;
		if (bks != nullptr) delete bks;
		if (solver != nullptr) delete solver;
	}
};


}
}