// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Utility.h"

#include "Util/Arr2D.h"
#include "MCP/CliqueSolver.h"

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {
//bool allocGlobalMem(std::string inpath) {

	// ../Instances/Homberger/R2_8_9.txt
	// ../Instances/Homberger/RC1_10_1.txt
	// ../Instances/Homberger/RC1_10_5.txt
	// ../Instances/Solomon/R103.txt
	// ../Instances/Solomon/R210.txt
	// ../Instances/Homberger/RC2_6_6.txt
	// ../Instances/Homberger/R1_6_8.txt
	// ../Instances/Homberger/R1_2_2.txt
	// ../Instances/Homberger/RC2_6_4.txt
	// ../Instances/Homberger/RC2_10_5.txt
	//../Instances/Solomon/R204.txt
	//../Instances/Solomon/R210.txt
	//../Instances/Homberger/R2_2_4.txt 
	//../Instances/Homberger/RC1_8_1.txt 
	//../Instances/Homberger/C1_10_6.txt 
	//../Instances/Homberger/C2_10_6.txt 
	//../Instances/Homberger/RC1_8_5.txt
	//../Instances/Homberger/RC2_10_1.txt

	globalCfg = new hust::Configuration();

	globalCfg->solveCommandLine(argc, argv);

	if (globalCfg->seed == -1) {
		globalCfg->seed = std::time(0) + std::clock();
	}
	
	//globalCfg->seed = 1645192521;
	//globalCfg->seed = 1645199481;

	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	ERROR("globalCfg->seed:", globalCfg->seed, " ins:", globalInput->example);
	INFO("argc:", argc);

	globalCfg->addHelpInfo();

	globalCfg->show();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new hust::util::Array2D<int> (globalInput->custCnt + 1, globalInput->custCnt + 1, 0);

	bks = new BKS();
	gloalTimer = new Timer(globalCfg->runTimer);

#if DIMACSGO
#else
	globalInput->printHelpInfo();
#endif // DIMACSGO

	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalInput;
	delete bks;
	delete gloalTimer;

	return true;
}

}//namespace hust

void testArray2() {

	hust::util::Array2D<int> a(2, 4, 0);
	for (int i = 0; i < a.size2(); ++i) {
		a[0][i] = 9 - i;
		a[1][i] = 5 - i;
	}

	std::sort(a[1], a[1] + a.size2());

	for (int i = 0; i < a.size1(); ++i) {
		for (int j = 0; j < a.size2(); ++j) {
			std::cout << a[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

void printSln(const szx::tsm::Clique& sln) {
	std::cout << "weight=" << sln.weight << std::endl;
	std::cout << "clique=";
	for (auto n = sln.nodes.begin(); n != sln.nodes.end(); ++n) {
		std::cout << *n << " ";
	}
	std::cout << std::endl;
}

int getBound(int n ,szx::tsm::AdjMat am) {

	szx::Arr<szx::tsm::Weight> weights(n, 1);
	szx::tsm::Clique sln;
	
	if (szx::tsm::solveWeightedMaxClique(sln, am, weights)) {
		return sln.nodes.size();
		//printSln(sln);
	}
}

int getOneBound(std::string ex) {

	int argc = 3;

	char a[100];
	snprintf(a, 100, "../Instances/Homberger/%s.txt", ex.c_str());
	char _sol[] = "./DLLSMA";
	char _ins[] = "-ins";

	char* argv[] = { _sol,_ins,a };
	
	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;

	
	int n = hust::globalInput->custCnt;

	szx::tsm::AdjMat am(n, n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			am.at(i, j) = 0;
		}
	}

	hust::Solver sol;
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
			sol.rUpdateAvfrom(r1, r1.head);
			sol.rUpdateZvfrom(r1, r1.tail);
			//sol.rNextDisp(r1);
			
			bool is1 = r1.rPtw > 0;

			sol.rRemoveAtPos(r1, v);
			sol.rRemoveAtPos(r1, w);


			sol.rInsAtPosPre(r2, r2.tail,w);
			sol.rInsAtPosPre(r2, r2.tail, v);
			sol.rUpdateAvfrom(r2, r2.head);
			sol.rUpdateZvfrom(r2, r2.tail);
			//sol.rNextDisp(r2);


			sol.rRemoveAtPos(r2, v);
			sol.rRemoveAtPos(r2, w);

			bool is2 = r2.rPtw > 0;

			if (is1 && is2 ) {
				//INFO("r1.rPtw:",r1.rPtw,"v:",v,"w:",w);
				am.at(v - 1, w - 1) = 1;
				am.at(w - 1, v - 1) = 1;
			}


		}
	}

	int ret = getBound(n,am);
	INFO("ret:{}",ret);

	ret = ret == hust::globalCfg->sintefRecRN;
	hust::deallocGlobalMem();
	return ret;
}

int main(int argc, char* argv[])
{
	//hust::INFO(sizeof(std::mt19937));
	//hust::INFO(sizeof(hust::Solver));
	//hust::INFO(sizeof(hust::Timer));
	//hust::INFO(sizeof(hust::Solver::alpha));
	//hust::INFO(sizeof(hust::Solver::input));
	//hust::INFO(sizeof(hust::Solver::globalCfg));
	//hust::INFO(sizeof(hust::Input));
	std::vector<std::string> v1 = {"C1_","C2_", "R1_", "R2_","RC1_", "RC2_" };
	std::vector<std::string> v2 = {"2_","4_", "6_", "8_","10_" };
	std::vector<std::string> v3 = {"1","2", "3", "4","5","6","7", "8", "9","10" };

	std::vector<std::string> s;

	for (std::string x : v1) {
		for (std::string y : v2) {
			for (std::string z : v3) {
				std::cout << (x + y + z) << std::endl;
				int ret = getOneBound(x + y + z);
				if (ret == 1) {
					s.push_back(x+y+z);
				}
			}
		}
	}
	for (auto& ss : s) {
		INFO(ss);
	}
	//  [INFO] : C1_2_1
	//	[INFO] : C1_4_1
	//	[INFO] : C1_6_1
	//	[INFO] : C1_8_1
	//	[INFO] : C1_8_2
	//	[INFO] : C1_10_1
	//	[INFO] : R1_2_1
	//	[INFO] : R1_4_1
	//	[INFO] : R1_6_1
	//	[INFO] : R1_8_1
	//	[INFO] : R1_10_1
	//	[INFO] : RC2_2_5
	return 0;

	hust::allocGlobalMem(argc,argv);
	hust::Goal goal;
	
	//goal.callSimulatedannealing();
	//goal.TwoAlgCombine();
	
	//goal.test();
	goal.experOnMinRN();

	hust::deallocGlobalMem();

	return 0;
}