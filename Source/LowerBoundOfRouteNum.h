#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Util_Common.h"

#include "Util_Arr2D.h"
#include "CommandLine.h"
#include "MCP/CliqueSolver.h"

namespace hust {

std::vector<int> getOneBound(std::string ex) {

	int argc = 3;

	char a[100];
	snprintf(a, 100, "../Instances/Homberger/%s.txt", ex.c_str());
	char _sol[] = "./DLLSMA";
	char _ins[] = "-ins";

	char* argv[] = { _sol,_ins,a };

	hust::CommandLine commandline(argc, argv);
	AlgorithmParameters aps = commandline.aps;
	RandomTools randomTools(commandline.seed);
	Timer timer(commandline.runTimer);

	Input input(&commandline,&aps,&randomTools,&timer);

	aps.displayInfo();

	// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
	aps.repairByCusCnt(input.custCnt);

	YearTable yearTable(&input);

	std::cout << ex << ", " << aps.sintefRecRN << std::endl;

	int n = input.custCnt;

	szx::tsm::AdjMat am(n, n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			am.at(i, j) = 0;
		}
	}

	BKS bks(&input,&yearTable,&timer);

	hust::Solver sol(&input,&yearTable,&bks);
	//sol.initSolution(4);

	hust::Route r1 = sol.rCreateRoute(0);
	sol.rts.push_back(r1);
	hust::Route r2 = sol.rCreateRoute(1);
	sol.rts.push_back(r2);

	for (int v = 1; v <= n; ++v) {
		for (int w = 1; w <= n; ++w) {

			if (v == w) {
				continue;
			}

			sol.rInsAtPosPre(r1, r1.tail, v);
			sol.rInsAtPosPre(r1, r1.tail, w);
			sol.rUpdateAvQFrom(r1, r1.head);
			sol.rUpdateZvQFrom(r1, r1.tail);
			//sol.rNextDisp(r1);

			bool is1 = r1.rPtw > 0;

			sol.rRemoveAtPosition(r1, v);
			sol.rRemoveAtPosition(r1, w);


			sol.rInsAtPosPre(r2, r2.tail, w);
			sol.rInsAtPosPre(r2, r2.tail, v);
			sol.rUpdateAvQFrom(r2, r2.head);
			sol.rUpdateZvQFrom(r2, r2.tail);
			//sol.rNextDisp(r2);


			sol.rRemoveAtPosition(r2, v);
			sol.rRemoveAtPosition(r2, w);

			bool is2 = r2.rPtw > 0;

			if (is1 && is2) {
				//Logger::INFO("r1.rPtw:",r1.rPtw,"v:",v,"w:",w);
				am.at(v - 1, w - 1) = 1;
				am.at(w - 1, v - 1) = 1;
			}
		}
	}

	int mqrn = -1;
	szx::Arr<szx::tsm::Weight> weights(n, 1);
	szx::tsm::Clique sln;

	if (szx::tsm::solveWeightedMaxClique(sln, am, weights)) {
		mqrn = static_cast<int>(sln.nodes.size());
	}

	int sinrn = aps.sintefRecRN;
	int qbrn = input.Qbound;

	return { mqrn,sinrn,qbrn };
}

void getAllInstancesBound() {

	std::vector<std::string> v1 = { "C1_","C2_", "R1_", "R2_","RC1_", "RC2_" };
	std::vector<std::string> v2 = { "2_","4_", "6_", "8_","10_" };
	std::vector<std::string> v3 = { "1","2", "3", "4","5","6","7", "8", "9","10" };

	std::vector<std::string> s;

	for (std::string x : v1) {
		for (std::string y : v2) {
			for (std::string z : v3) {
				std::cout << (x + y + z) << std::endl;
				auto ret = getOneBound(x + y + z);
				if (ret[0] == ret[1]) {
					s.push_back(x + y + z);
				}
			}
		}
	}

	for (auto& ss : s) {
		Logger::INFO(ss);
	}

	std::vector<std::string> instsReachBound = {
		"C1_2_1", "C1_4_1",
		"C1_6_1", "C1_8_1",
		"C1_8_2", "C1_10_1",
		"R1_2_1", "R1_4_1",
		"R1_6_1", "R1_8_1",
		"R1_10_1", "RC2_2_5" };

	for (std::string ex : instsReachBound) {
		auto rns = getOneBound(ex);
		std::cout << ex << ", " << rns[0] << ", " << rns[1] << ", " << rns[2] << std::endl;
		std::cout << "---------------" << std::endl;
	}
}
}