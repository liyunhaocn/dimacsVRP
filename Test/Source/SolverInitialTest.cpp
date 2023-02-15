#include "pch.h"

TEST(SovlerInitialTest, InitKind0) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}

TEST(SovlerInitialTest, InitKind1) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(1);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}

TEST(SovlerInitialTest, InitKind2) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(2);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}

TEST(SovlerInitialTest, InitKind3) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(3);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}

TEST(SovlerInitialTest, InitKind4) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(4);

		EXPECT_EQ(solver->RoutesCost, solver->verify());
		EXPECT_EQ(solver->penalty, 0);
		EXPECT_EQ(solver->Ptw, 0);
		EXPECT_EQ(solver->Pc, 0);
		EXPECT_EQ(solver->EP.size(), 0);

	}
}
