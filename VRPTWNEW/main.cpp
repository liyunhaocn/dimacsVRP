// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <functional>
#include <numeric>
//#include "./VRPTW/Environment.h"
//#include "./VRPTW/Utility.h"
//#include "./VRPTW/Solver.h"
//#include "./VRPTW/Problem.h"
//#include "./VRPTW/Configuration.h"

#include "./Environment.h"
#include "./Utility.h"
#include "./Solver.h"
#include "./Problem.h"
#include "./Configuration.h"
#include "./Flag.h"
#include "./EAX.h"

#include<cstdlib>
#include<cstring>
#include<cmath>
#include<iostream>
#include<algorithm>
#include<queue>
#include<vector>
#include<map>

namespace hust {

bool updateBestSol(hust::Solver& pBest, hust::Solver& pTemp) {
	if (pTemp.RoutesCost < pBest.RoutesCost) {
		pBest = pTemp;
		//pBest.saveToOutPut();
		//pBest.saveOutAsSintefFile();
		hust::println("cost:", pBest.RoutesCost);
		return true;
	}
	else {
		return false;
	}
	return false;
};

bool allocGlobalMem(int argc, char* argv[]) {

	//Environment env("../Instances/Solomon/C101.txt");
	//Environment env("../Instances/Homberger/RC1_8_3.txt");
	globalEnv = new Environment("../Instances/Homberger/RC1_8_1.txt");
	//Environment env("../Instances/Homberger/C1_2_8.txt");

	cfg = new hust::Configuration();
	//lyh::MyString ms;

	solveCommandLine(argc, argv, cfg, globalEnv);

	if (globalEnv->seed == -1) {
		globalEnv->seed = std::time(nullptr) + std::clock();
	}

	globalEnv->seed = 1611589828;
	globalEnv->show();
	cfg->show();

	myRand = new Random(globalEnv->seed);
	myRandX = new RandomX(globalEnv->seed);

	globalInput = new Input(*globalEnv);

	// TODO[lyh][0]:一定要记得cfg用cusCnt合法化一下
	cfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<LL> >
		(globalInput->custCnt + 1, Vec<LL>(globalInput->custCnt + 1, 0));
	//TODO[lyh][0]:
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete cfg;
	delete globalEnv;
	delete globalInput;

	return true;
}

bool naEAX(Solver& pBest,Solver& pa, Solver& pb) {

	int paUpNum = 0;
	int paNotUpNum = 0;

	int wtihSubcyNum = 0;
	int wtihSubcyRepair = 0;
	int wtihoutSubcyNum = 0;
	int wtihoutSubcyRepair = 0;

	int repairNum = 0;

	

	for (int ch = 1; ch <= 10; ++ch) {

		EAX eax(pa, pb);

		Solver pc = pa;
		//unsigned eaxSeed = (env.seed % Mod) + ((myRand->pick(10000007))) % Mod;
		Vec<int> newCus;
		// newCus = eax.doNaEAX(pa, pb, pc);
		if (ch <= 9) {
			newCus = eax.doNaEAX(pa, pb, pc);
		}
		else if (ch == 10) {
			newCus = eax.doPrEAX(pa, pb, pc);
		}

		//if (eax.subCyNum > 0) {
		//	++wtihSubcyNum;
		//	if (eax.repairSolNum > 0) {
		//		++wtihSubcyRepair;
		//	}
		//}
		//else {
		//	++wtihoutSubcyNum;
		//	if(eax.repairSolNum > 0) {
		//		++wtihoutSubcyRepair;
		//	}
		//}
		//println("eax.subCusNum:",eax.subCyCusNum, " wtihSb:", wtihSubcyNum, " wtihSbRe:", wtihSubcyRepair, " wtihoutSb:", wtihoutSubcyNum, " wtihoutSbRe:", wtihoutSubcyRepair);

		if (eax.repairSolNum == 0) {
			if (eax.abCycleSet.size() == 0) {
				println("eax.abCycleSet.size() == 0");
				return false;
			}
			continue;
		}

		++repairNum;

		pc.mRLLocalSearch(newCus);
		//pc.mRLLocalSearch({});
		bool up = updateBestSol(pBest, pc);
		if (up) {
			//println("MAiter:", MAiter, " rep:", repairNum, " paUp:", paUpNum, " paNUp:", paNotUpNum, " pa:", paIndex, " pb:", pbIndex, " cost", pBest.RoutesCost, "bSolNUp", bestSolContiNotUp);
			println("eax update");
			//ch = 1;
		}

		for (int i = 0; i < 20; ++i) {
			bool ruin = pc.ruinLocalSearch(1, myRand->pick(2) + 1);
			if (ruin) {
				i = 0;
			}
			bool up = updateBestSol(pBest, pc);
			if (up) {
				//println("MAiter:", MAiter, " rep:", repairNum, " paUp:", paUpNum, " paNUp:", paNotUpNum, " pa:", paIndex, " pb:", pbIndex, " cost", pBest.RoutesCost, "bSolNUp", bestSolContiNotUp);
				println("eax ruin local update");
				i = 0;
				//ch = 1;
			}
		}

		if (pc.RoutesCost < pa.RoutesCost) {
			println("pc.RoutesCost < pa.RoutesCost");
			pa = pc;
			//ch = 1;
			++paUpNum;
		}
		else {
			++paNotUpNum;
		}
	}
	return true;
}

bool solverByEAX(int argc, char* argv[]) {

	allocGlobalMem(argc,argv);

	Solver pBest(*globalInput, *globalEnv);
	pBest.RoutesCost = DisInf;

	Timer t1(cfg->runTimer);
	t1.setLenUseSecond(cfg->runTimer);
	t1.reStart();

	Vec<Solver> pool;
	pool.reserve(cfg->popSize);

	for (int i = 0; i < cfg->popSize; ++i) {
		Environment envt = *globalEnv;
		envt.seed = (globalEnv->seed % Mod) + ((i + 1) * (myRand->pick(10000007))) % Mod;
		Solver st(*globalInput, envt);
		st.minimizeRN();
		//saveSlnFile(input, pBest.output, cfg, env);
		st.mRLLocalSearch({});

		//for (int i = 0; i < 20; ++i) {
		//	st.ruinLocalSearch(1, myRand->pick(2) + 1);
		//	bool up = updateBestSol(pBest, st);
		//	if (up) {
		//		//println("MAiter:", MAiter, " rep:", repairNum, " paUp:", paUpNum, " paNUp:", paNotUpNum, " pa:", paIndex, " pb:", pbIndex, " cost", pBest.RoutesCost, "bSolNUp", bestSolContiNotUp);
		//		println("init ruin local update");
		//		i = 0;
		//	}
		//}

		if (st.RoutesCost < pBest.RoutesCost) {
			pBest = st;
		}
		pool.push_back(st);
	}

	int paIndex =  myRand->pick( cfg->popSize);
	int pbIndex = (paIndex + myRand->pick(cfg->popSize - 1) + 1) % cfg->popSize;

	Vec<Vec<LL>> eaxYearTable
	(cfg->popSize, Vec<LL>(cfg->popSize));
	LL MAiter = 0;

	Vec<int> solConti(cfg->popSize, 1);


	while (pBest.RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {
		++MAiter;

		#pragma region ChoosePaPb
		eaxYearTable[paIndex][pbIndex] = MAiter;
		paIndex = myRand->pick(cfg->popSize);
		pbIndex = (paIndex + 1) % cfg->popSize;
		int sameNum = 1;
		int retPbIndex = pbIndex;
		for (int i = 1; i + 1 < cfg->popSize; ++i) {
			int newPbIndex = (pbIndex + i) % cfg->popSize;

			if (eaxYearTable[paIndex][newPbIndex]
				< eaxYearTable[paIndex][pbIndex]) {
				retPbIndex = newPbIndex;
				sameNum = 1;
			}
			if (eaxYearTable[paIndex][newPbIndex]
				== eaxYearTable[paIndex][pbIndex]) {
				++sameNum;
				if (myRand->pick(sameNum) == 0) {
					retPbIndex = newPbIndex;
				}
			}
		}
		pbIndex = retPbIndex;
		#pragma endregion

		Solver& pa = pool[paIndex];
		Solver& pb = pool[pbIndex];

		for (int i = 0; i < 20; ++i) {
			int ruinCusNum = myRand->pick(2) + 1;
			bool ruin = pa.ruinLocalSearch(1, ruinCusNum);
			if (ruin) {
				i = 0;
			}
			if (updateBestSol(pBest, pa)) {
				println("ruin a update");
				i = 0;
			}
		}
		
		for (int i = 0; i < 20; ++i) {
			int ruinCusNum = myRand->pick(2) + 1;
			bool ruin = pb.ruinLocalSearch(1, ruinCusNum);
			if (ruin) {
				i = 0;
			}
			if (updateBestSol(pBest, pb)) {
				println("ruin b update");
				i = 0;
			}
		}

		naEAX(pBest, pa, pb);

	}

	Output output = pBest.saveToOutPut();
	output.runTime = t1.getRunTime();
	saveSlnFile(*globalInput, output, cfg, *globalEnv);
	t1.disp();

	deallocGlobalMem();

	return true;
}

bool justLocalSearch(int argc, char* argv[]) {

	allocGlobalMem(argc, argv);

	squIter = 1;

	Timer t1(cfg->runTimer);
	t1.setLenUseSecond(cfg->runTimer);
	t1.reStart();

	Solver st(*globalInput, *globalEnv);
	st.minimizeRN();
	//st.saveOutAsSintefFile("minR");
	st.mRLLocalSearch({});
	//st.saveOutAsSintefFile("minL");
	//TODO[lyh][0]:
	Solver pBest = st;

	//updateBestSol(st);

	int ruinNum = 0;
	int parNum = 0;

	Vec<int> cusOrder(globalInput->custCnt,1);
	std::iota(cusOrder.begin(), cusOrder.end(),1);

	int avg = globalInput->custCnt / st.rts.cnt;
	int contiNoDown = 1;

	Vec<Vec<int>> table(3, Vec<int>(50, 0));

	//int kind = myRand->pick(3);
	int kind = 1;
	//int rnum = std::min<int>(contiNoDown/10,20) + 1;
	int rnum = 1;

	while (pBest.RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {

		st = pBest;

		#if 1
		if (contiNoDown >= 500) {
			rnum = std::min<int>(rnum+1,10);
			contiNoDown = 1;
		}

		bool isRuin = st.ruinLocalSearch(kind, rnum);
		bool up1 = updateBestSol(pBest, st);
		if (up1) {
			//println("cost:", pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
			println("-cost:", pBest.RoutesCost, " rnum:", rnum);
			contiNoDown = 1;
			rnum = 1;
			//++table[kind][rnum];
			//println("-----------");
			//for (auto& i : table) {
			//	printve(i);
			//}
			++ruinNum;
		}
		else {
			++contiNoDown;
			//println("+cost:", pBest.RoutesCost, " rnum:", rnum);
		}
		#else
		Vec<int> cus = st.patternAdjustment(200);
		st.mRLLocalSearch(cus);
		bool up2 = updateBestSol(pBest, st);
		if (up2) {
			++parNum;
			println("cost:", pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
		}
		#endif // 0
	}

	Output output = pBest.saveToOutPut();
	output.runTime = t1.getRunTime();
	saveSlnFile(*globalInput, output, cfg, *globalEnv);
	t1.disp();

	deallocGlobalMem();

	return true;
}

}

int main(int argc, char* argv[])
{
	hust::println(sizeof(std::mt19937));
	hust::println(sizeof(hust::Solver));
	hust::println(sizeof(hust::Timer));
	hust::println(sizeof(hust::Solver::alpha));
	hust::println(sizeof(hust::Solver::input));
	hust::println(sizeof(hust::Solver::env));
	hust::println(sizeof(hust::Input));

	/*for (;;) {
		hust::myRand = new hust::Random(0);
		hust::myRandX = new hust::RandomX(0);
		delete hust::myRand;
		delete hust::myRandX;
	}
	return 0;*/
	hust::solverByEAX(argc, argv);return 0;
	//hust::justLocalSearch(argc, argv); return 0;
	//run(argc, argv);

	return 0;
}