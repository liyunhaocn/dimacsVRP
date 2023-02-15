
#include "pch.h"


TEST(SolverEjectionTest, DoEject) {
	
	using namespace hust;
	using namespace lyhtest;
	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);
		
		auto& r = solver->rts[0];
		DisType rQStart = r.rQ;
		DisType PtwStart = r.rPtw;
		DisType PcStart = r.rPc;
		DisType depot0TW_XStart = solver->customers[r.head].TWX_;
		DisType depotN1TW_XStart = solver->customers[r.tail].TW_X;

		solver->ejectOneRouteOnlyP(r, 2, 4);

		EXPECT_EQ(r.rQ, rQStart);
		EXPECT_EQ(PcStart, r.rPc);
		EXPECT_EQ(PtwStart, r.rPtw);
		EXPECT_EQ(PtwStart, solver->customers[r.head].TWX_);
		EXPECT_EQ(PtwStart, solver->customers[r.tail].TW_X);
		EXPECT_EQ(solver->customers[r.head].QX_, rQStart);
		EXPECT_EQ(solver->customers[r.tail].Q_X, rQStart);

		Vec<int> v1 = solver->rPutCustomersInVector(r);
		Vec<int> v2;

		for (int pt = solver->customers[r.tail].pre; pt <= genator.input->custCnt; pt = solver->customers[pt].pre) {
			v2.push_back(pt);
		}
		EXPECT_EQ(v1.size(), v2.size());

	}



}