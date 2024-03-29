#include "pch.h"

TEST(SovlerSimulatedannealingTest, kind0) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);
		solver->refinement(0, 100, 100.0, 10);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}

TEST(SovlerSimulatedannealingTest, kind1) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);
		solver->refinement(1, 100, 100.0, 10);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}