#include "pch.h"

TEST(SovlerMinimizeTest, MinimizeRouteNumber) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);

		while (solver->rts.cnt > 2) {

			EXPECT_EQ(solver->RoutesCost, solver->verify());
			EXPECT_EQ(solver->penalty, 0);
			EXPECT_EQ(solver->Ptw, 0);
			EXPECT_EQ(solver->Pc, 0);
			EXPECT_EQ(solver->EP.size(), 0);

			if (solver->minimizeRouteNumber(solver->rts.cnt - 1) == false) {
				break;
			}
		}
	}

}