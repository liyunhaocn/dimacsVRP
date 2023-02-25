//
// pch.h
//

#pragma once

#include <gtest/gtest.h>
#include "../../Source/Common.h"
#include "../../Source/Solver.h"
#include "../../Source/YearTable.h"
#include "../../Source/EAX.h"
#include "../../Source/AlgorithmParameters.h"
#include "../../Source/Input.h"
#include "../../Source/CommandLine.h"

namespace hust {
namespace lyhtest {
	
std::vector< std::vector< std::string> > getTestCommandLineArgv();

struct SolverGenerator {
	
public:
	CommandLine* commandline = nullptr;
	Timer* timer = nullptr;
	AlgorithmParameters* aps = nullptr;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
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
		random = new Random(commandline->seed);
		randomx = new RandomX(commandline->seed);

		input = new Input(commandline);
		YearTable* yearTable = new YearTable(input);
		BKS* bks = new BKS(input);

		solver = new Solver(input, yearTable, bks);
		return solver;
	}

	~SolverGenerator() {
		if (commandline != nullptr) delete commandline;
		if (timer != nullptr) delete timer;
		if (random != nullptr) delete random;
		if (randomx != nullptr) delete randomx;
		if (input != nullptr) delete input;
		if (yearTable != nullptr) delete yearTable;
		if (bks != nullptr) delete bks;
		if (solver != nullptr) delete solver;

	}
};


}
}