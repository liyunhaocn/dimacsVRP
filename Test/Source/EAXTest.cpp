#include "pch.h"


TEST(EAXTest, generateCycles) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genatorPa;
		SolverGenerator genatorPb;

		auto pa = genatorPa.generateSolver(argvItem);
		pa->initSolution(0);
		Solver* pb = nullptr;
		try {
			pb = genatorPb.generateSolver(argvItem);
		}
		catch (const char* msg) {
			std::cerr << msg << std::endl;
		}

		
		pb->initSolution(1);
		
		int bigRtsNumber = std::max<int>(pa->rts.cnt, pb->rts.cnt);
		pa->adjustRouteNumber(bigRtsNumber);
		pb->adjustRouteNumber(bigRtsNumber);
		
		EAX eax(*pa, *pb);

		EXPECT_EQ(eax.pa->rts.cnt, eax.pb->rts.cnt);
		
		eax.generateCycles();

		EXPECT_TRUE(eax.GabEsize > 0);
		
		Solver pc = *pa;
		eax.doNaEAX(pc);

		pc.reCalRtsCostSumCost();
		int totalCustomerNumer = 0;
		for (int i = 0; i < pc.rts.cnt; ++i) {
			auto& r = pc.rts[i];
			int customerNumberInRour = pc.rGetCustomerNumber(r);
			totalCustomerNumer += customerNumberInRour;
			EXPECT_EQ(r.rCustCnt, customerNumberInRour);
		}

		// test if there is a subtour in solution
		EXPECT_EQ(totalCustomerNumer, genatorPa.input->customerNumer);
	}
}