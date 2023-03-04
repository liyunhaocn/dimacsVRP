

#include "pch.h"

// test the all Neighorhood will take too much time, you can test them use the follow second line code.
//TEST(SolverTest, DISABLED_AllNeighborhood) {
TEST(SolverNeighborhoodTest, AllNeighborhood) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);

		for (int v = 1; v <= genator.input->customerNumer; ++v) {
			for (int w = v + 1; w <= genator.input->customerNumer; ++w) {
				for (int kind = 0; kind <= 15; ++kind) {

					auto deltPen = solver->estimatevw(kind,v, w, 1);
					TwoNodeMove m(v,w,kind, deltPen);
					if (deltPen.PcOnly != DisInf && deltPen.deltPtw != DisInf) {
						DisType penaltyOld = solver->penalty;
						DisType ptwNoWeiOld = solver->PtwNoWei;
						DisType pcOld = solver->Pc;
						DisType ptwOld = solver->Ptw;
						solver->doMoves(m);
						solver->reCalRtsCostAndPen();
						DisType penaltyNew = solver->penalty;
						DisType ptwNew = solver->Ptw;
						DisType ptwNoWeiNew = solver->PtwNoWei;
						DisType pcNew = solver->Pc;

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

TEST(SolverNeighborhoodTest, SimplyTest) {

	using namespace hust;
	using namespace lyhtest;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		SolverGenerator genator;

		auto solver = genator.generateSolver(argvItem);
		solver->initSolution(0);

		int v = (rand() % genator.input->customerNumer) + 1;
		int w = (rand() % genator.input->customerNumer) + 1;
		while (w == v) {
			w = (rand() % genator.input->customerNumer) + 1;
		}
		int kind = rand() % 15;

		auto deltPen = solver->estimatevw(kind, v, w, 1);
		TwoNodeMove m(v, w, kind, deltPen);
		if (deltPen.PcOnly != DisInf && deltPen.deltPtw != DisInf) {
			DisType penaltyOld = solver->penalty;
			DisType ptwNoWeiOld = solver->PtwNoWei;
			DisType pcOld = solver->Pc;
			DisType ptwOld = solver->Ptw;
			solver->doMoves(m);
			solver->reCalRtsCostAndPen();
			DisType penaltyNew = solver->penalty;
			DisType ptwNew = solver->Ptw;
			DisType ptwNoWeiNew = solver->PtwNoWei;
			DisType pcNew = solver->Pc;

			EXPECT_EQ(penaltyOld + deltPen.deltPc + deltPen.deltPtw, penaltyNew) << "kind: " << kind << std::endl;
			EXPECT_EQ(ptwOld + deltPen.deltPtw, ptwNew) << "kind: " << kind << std::endl;
			EXPECT_EQ(ptwNoWeiOld + deltPen.PtwOnly, ptwNoWeiNew) << "kind: " << kind << std::endl;
			EXPECT_EQ(pcOld + deltPen.deltPc, pcNew) << "kind: " << kind << std::endl;
		}
	}
}

