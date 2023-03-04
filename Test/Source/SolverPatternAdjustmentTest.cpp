#include "pch.h"

TEST(SovlerPatternAdjustmentTest, SimplyTest) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);
		solver->perturb();

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}

}