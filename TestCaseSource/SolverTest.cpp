

#include "pch.h"


TEST(SovlerTest, minimizeRouteNumber) {

	using namespace hust;
	
	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {
		
		int argc = argvItem.size();

		char** argv = new char*[argc];
		for (int i = 0; i < argc; ++i) {
			argv[i] = const_cast<char*>(argvItem[i].c_str());
		}
		CommandLine commandline(argc, argv);
		Timer timer(commandline.runTimer);

		AlgorithmParameters aps = commandline.aps;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);
		YearTable yearTable(&input);
		BKS bks(&input, &yearTable, &timer);
		
		Solver solver(&input, &yearTable, &bks);
		solver.initSolution(0);
		
		while (solver.rts.cnt > 2) {

			EXPECT_EQ(solver.RoutesCost, solver.verify());
			EXPECT_EQ(solver.penalty, 0);
			EXPECT_EQ(solver.Ptw, 0);
			EXPECT_EQ(solver.Pc, 0);
			EXPECT_EQ(solver.EP.size(), 0);

			if (solver.minimizeRouteNumber(solver.rts.cnt - 1) == false) {
				break;
			}
		}
	}
	//hust::Input input;
	//hust::Solver s(,);
	//s.minimizeRN(0);
	//auto ret = hust::getOneBound("C1_6_6");
	//auto ret = std::vector<int>{1,3};
	//EXPECT_TRUE(true);

}

TEST(SolverTest, Neighborhood) {

	using namespace hust;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		int argc = argvItem.size();

		char** argv = new char* [argc];
		for (int i = 0; i < argc; ++i) {
			argv[i] = const_cast<char*>(argvItem[i].c_str());
		}
		CommandLine commandline(argc, argv);
		Timer timer(commandline.runTimer);

		AlgorithmParameters aps = commandline.aps;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);
		YearTable yearTable(&input);
		BKS bks(&input, &yearTable, &timer);

		Solver solver(&input, &yearTable, &bks);
		solver.initSolution(0);

		for (int v = 1; v <= input.custCnt; ++v) {
			for (int w = v + 1; w <= input.custCnt; ++w) {
				for (int kind = 0; kind <= 15; ++kind) {

					auto deltPen = solver.estimatevw(kind,v, w, 1);
					TwoNodeMove m(v,w,kind, deltPen);
					if (deltPen.PcOnly != DisInf && deltPen.deltPtw != DisInf) {
						DisType penaltyOld = solver.penalty;
						DisType ptwNoWeiOld = solver.PtwNoWei;
						DisType pcOld = solver.Pc;
						DisType ptwOld = solver.Ptw;
						solver.doMoves(m);
						solver.reCalRtsCostAndPen();
						DisType penaltyNew = solver.penalty;
						DisType ptwNew = solver.Ptw;
						DisType ptwNoWeiNew = solver.PtwNoWei;
						DisType pcNew = solver.Pc;

						EXPECT_EQ(penaltyOld + deltPen.deltPc + deltPen.deltPtw, penaltyNew) << "kind: " << kind << std::endl;
						EXPECT_EQ(ptwOld + deltPen.deltPtw, ptwNew) << "kind: " << kind << std::endl;
						EXPECT_EQ(ptwNoWeiOld + deltPen.PtwOnly, ptwNoWeiNew) << "kind: "<< kind << std::endl;
						EXPECT_EQ(pcOld + deltPen.deltPc, pcNew) << "kind: " << kind << std::endl;
					}
				}
			}
		}
	}
}

