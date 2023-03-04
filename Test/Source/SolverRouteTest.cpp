
#include "pch.h"

TEST(SolverRouteTest, CustomersTest) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);

		auto& rts = solver->rts;

		for (int i = 0; i < rts.cnt; ++i) {

			Route& r = rts[i];
			Vector<int> cus1;
			Vector<int> cus2;

			int pt = r.head;
			while (pt != -1) {
				EXPECT_EQ(solver->customers[pt].routeId, r.routeId) << "pt:" << pt << " r.routeId: " << r.routeId << std::endl;
				cus1.push_back(pt);
				pt = solver->customers[pt].next;
			}

			pt = r.tail;
			while (pt != -1) {
				cus2.push_back(pt);
				pt = solver->customers[pt].prev;
			}

			EXPECT_EQ(cus1.size(), cus2.size()) << "cus1.size():" << cus1.size() << " cus2.size():" << cus2.size() << std::endl;

			for (int i = 0; i < cus1.size(); ++i) {
				EXPECT_EQ(cus1[i], cus2[cus1.size() - 1 - i]);
			}
			EXPECT_EQ((r.rCustCnt + 2), cus1.size()) << "r.rCustCnt:" << r.rCustCnt << " cus1.size():" << cus1.size() << std::endl;
		}
	}

}