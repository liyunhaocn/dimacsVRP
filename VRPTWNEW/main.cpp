// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <functional>
#include <numeric>

#include "Environment.h"
#include "Utility.h"
#include "Solver.h"
#include "Problem.h"
#include "Configuration.h"
#include "Flag.h"
#include "EAX.h"

#include<cstdlib>
#include<cstring>
#include<cmath>
#include<iostream>
#include<algorithm>
#include<queue>
#include<vector>
#include<map>

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {

	//globalEnv = new Environment("../Instances/Solomon/C101.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_3.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_1.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_4_2.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C2_10_6.txt");
	//globalEnv  = new Environment("../Instances/Homberger/RC1_8_1.txt");
	// 
	//75296223
	globalEnv = new Environment("../Instances/Homberger/RC2_6_4.txt");

	globalCfg = new hust::Configuration();
	//lyh::MyString ms;

	solveCommandLine(argc, argv);

	if (globalEnv->seed == -1) {
		globalEnv->seed = std::time(nullptr) + std::clock();
	}

	//globalEnv->seed = 1611589828;
	//globalEnv->seed = 1611589111;
	//globalEnv->seed = 1640620823;
	globalEnv->seed = 1640660545;//RC2_6_4

	globalEnv->show();
	globalCfg->show();

	myRand = new Random(globalEnv->seed);
	myRandX = new RandomX(globalEnv->seed);

	globalInput = new Input();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<LL> >
		(globalInput->custCnt + 1, Vec<LL>(globalInput->custCnt + 1, 0));
	//TODO[lyh][0]:
	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalEnv;
	delete globalInput;

	return true;
}

struct Goal {
	
	Vec<Vec<LL>> eaxYearTable;
	DisType lyhRec = DisInf;
	DisType lyhBestOneTurn = DisInf;
	Vec<Solver> pool;

	Goal() {
		eaxYearTable = Vec<Vec<LL>>
		(globalCfg->popSize, Vec<LL>(globalCfg->popSize));
		lyhRec = DisInf;
		lyhBestOneTurn = DisInf;
	}

	#if 0
	Vec<int> getpairOfPaPb() {

		int paIndex = myRand->pick(globalCfg->popSize);
		int pbIndex = (paIndex + 1) % globalCfg->popSize;
		int sameNum = 1;
		int retPbIndex = pbIndex;
		for (int i = 1; i + 1 < globalCfg->popSize; ++i) {
			int newPbIndex = (pbIndex + i) % globalCfg->popSize;

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
		return { paIndex,pbIndex };
	}
	#endif // 0

	bool updateBestSolRec(hust::Solver& pTemp) {

		lyhRec = std::min<DisType>(lyhRec, pTemp.RoutesCost);

		if (pTemp.RoutesCost < lyhBestOneTurn){

			lyhBestOneTurn = pTemp.RoutesCost;
			//pBest.saveToOutPut();
			//pBest.saveOutAsSintefFile();
			//hust::println("cost:", lyhRec);
			return true;
		}
		else {
			return false;
		}
		return false;
	};

	int doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

		int retState = 0; // 0 表示没有成功更新最优解，1表示更新了最优解 -1表示这两个解无法进行eax
		
		EAX eax(pa, pb);
		eax.generateCycles();

		if (eax.abCycleSet.size() == 0) {
			return -1;
		}

		Solver paBest = pa;

		//static int genSol = 0;
		//static int repSol = 0;

		int contiNotRepair = 1;

		for (int ch = 1; ch <= globalCfg->naEaxCh; ++ch) {

			Solver pc = pa;
			int eaxState = 0;
			if (kind == 0) {
				eaxState = eax.doNaEAX(pa, pb, pc);
			}
			else {
				eaxState = eax.doPrEAX(pa, pb, pc);
			}

			if (eaxState==-1) {
				eax.generateCycles();
				continue;
			}

			//++genSol;
			if (eax.repairSolNum == 0) {	
				++contiNotRepair;
				if (contiNotRepair >= 3) {
					eax.generateCycles();
				}
				continue;
			}
			else {
				contiNotRepair = 1;
			}

			//++repSol;
			auto newCus = EAX::getDiffCusofPb(pa,pc);
			//double rate = 100 * (pc.RoutesCost - pa.RoutesCost) / pa.RoutesCost;
			//println("kind:", kind,
			//	"eax.subCyNum:", eax.subCyNum, 
			//	"eax.subCyCusNum:", eax.subCyCusNum,
			//	"newSize:",newCus.size(),
			//	"rate:", rate
			//);

			//bool
			if (newCus.size() > 0) {
				pc.mRLLocalSearch(1,newCus);
				auto cus1 = EAX::getDiffCusofPb(pa, pc);
				if (cus1.size() == 0) {
					//debug("pa is same as pa");
				}
			}
			else {
				//debug("pa is same as pa");
			}
			
			if (pc.RoutesCost < paBest.RoutesCost) {
				paBest = pc;
				bool up = updateBestSolRec(pc);
				if (up) {
					ch = 1;
					retState = 1;
				}

				if (pc.RoutesCost == lyhRec) {
					println("cost:", lyhRec, "bestTurn:", lyhBestOneTurn, "eax local update, kind:", kind, "choosecyIndex:", eax.choosecyIndex, "chooseuId:", eax.unionIndex);
				}
			}

		}

		if (paBest.RoutesCost < pa.RoutesCost) {
			pa = paBest;
		}
		return retState;
	}

	bool perturbThePop() {

		auto pertuOrder = Vec<int>(pool.size(), 0);
		std::iota(pertuOrder.begin(), pertuOrder.end(), 0);
		std::sort(pertuOrder.begin(), pertuOrder.end(), [&](int x, int y) {
			return pool[x].RoutesCost < pool[y].RoutesCost;
		});

		int best = pool[pertuOrder[0]].RoutesCost;
		if (best != lyhRec) {
			println("best sol not in pool");
		}

		for (int peri = 1; peri < pool.size(); ++peri) {
			int pi = pertuOrder[peri];
			auto& sol = pool[pi];
			auto before = sol.RoutesCost;
			Solver sclone = sol;

			#if 0
			sclone.resetSol();
			sclone.initSolution(myRand->pick(5));
			sclone.adjustRN();
			sclone.mRLLocalSearch(0, {});
			sol = sclone;
			#else		
			//sclone.saveOutAsSintefFile();
			for (int i = 0; i < 10; ++i) {
				//int kind = myRand->pick(3);
				sclone.perturbBaseRuin(3, 2*globalCfg->ruinC_);

				if (sclone.RoutesCost * 100 >= sol.RoutesCost * 110
					&& sclone.RoutesCost * 100 <= sol.RoutesCost * 120) {
					sol = sclone;
					//sclone.saveOutAsSintefFile("p");
					break;
				}
				else {
					sclone = sol;
				}
			}
			#endif // 0

			auto after = sol.RoutesCost;
			println("before:", before, "after:", after);
		}
		return true;
	}

	bool solverByEAX() {

		lyhRec = DisInf;
		lyhBestOneTurn = DisInf;

		Timer t1(globalCfg->runTimer);
		t1.setLenUseSecond(globalCfg->runTimer);
		t1.reStart();

		pool.reserve(globalCfg->popSize);

		for (int i = 0; i < globalCfg->popSize; ++i) {

			Solver st(*globalInput);
			int initKind = myRand->pick(5);
			st.initSolution(initKind);

			st.adjustRN();
			//saveSlnFile(input, pBest.output, globalCfg, globalEnv);
			st.mRLLocalSearch(0,{});
			st.ruinLocalSearch(2);
			updateBestSolRec(st);
			pool.push_back(st);
		}

		//LL MAiter = 0;

		Vec<int> solConti(globalCfg->popSize, 1);

		int contiNotDown = 1;
		int strategy = 0;

		#if DIMACSGO
		while (lyhRec > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (lyhRec > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			if (contiNotDown >= 50) {

				//debug(contiNotDown);
				//debug(strategy);
				++strategy;
				strategy %= 3;
				//strategy %= 2;
				contiNotDown = 1;
			}

			if (strategy == 2) {
				
				lyhBestOneTurn = DisInf;

				perturbThePop();

				contiNotDown = 100;
				continue;
			}

			int popSize = globalCfg->popSize;

			auto& papbOrder = myRandX->getMN(popSize, popSize);
			unsigned shuseed = (globalEnv->seed % hust::Mod) + ((hust::myRand->pick(10000007))) % hust::Mod;
			std::shuffle(papbOrder.begin(), papbOrder.end(), std::default_random_engine(shuseed));
			
			for (int i = 0; i < popSize; ++i) {

				int paIndex = papbOrder[i];
				int pbIndex = papbOrder[(i + 1) % popSize];
				Solver& pa = pool[paIndex];
				Solver& pb = pool[pbIndex];

				int isabState = doTwoKindEAX(pa, pb, strategy);
				//println("isabState:", isabState);
				if (isabState == 1) {
					contiNotDown = 1;
				}
				else {// 0 -1
					++contiNotDown;
				}
				
			}

		}

		//TODO[0][bestsSol]:要保证种群中最有解不会被扰动
		int minRli = 0;
		for (int i = 1; i < pool.size();++i) {
			if (pool[i].RoutesCost < pool[minRli].RoutesCost) {
				minRli = i;
			}
		}

		Output output = pool[minRli].saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(*globalInput, output);
		t1.disp();

		return true;
	}

	bool justLocalSearch() {

		Timer t1(globalCfg->runTimer);
		t1.setLenUseSecond(globalCfg->runTimer);
		t1.reStart();

		Solver st(*globalInput);
		//st.initSolution(myRand->pick(2));
		st.initSolution(0);

		st.adjustRN();
		//st.saveOutAsSintefFile("minR");
		st.mRLLocalSearch(0,{});

		//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
		auto pBest = st;

		int contiNotDown = 1;

		#if DIMACSGO
		while (lyhRec > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (lyhRec > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			st = pBest;
			int c_ = std::min<int>(globalCfg->ruinC_,contiNotDown);

			if (contiNotDown % 100 == 0) {
				st.patternAdjustment(30);
				st.mRLLocalSearch(0,{});
			}
			else {
				st.ruinLocalSearch(c_);
			}
			
			bool up2 = updateBestSolRec(st);
			if (up2) {
				println("cost:", lyhRec, "bestTurn:", lyhBestOneTurn);
			}
			if (st.RoutesCost < pBest.RoutesCost) {
				pBest = st;
				contiNotDown = 1;
			}
			else {
				++contiNotDown;
			}


		}

		Output output = pBest.saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(*globalInput, output);
		t1.disp();

		return true;
	}
};

}//namespace hust

int main(int argc, char* argv[])
{
	//hust::println(sizeof(std::mt19937));
	//hust::println(sizeof(hust::Solver));
	//hust::println(sizeof(hust::Timer));
	//hust::println(sizeof(hust::Solver::alpha));
	//hust::println(sizeof(hust::Solver::input));
	//hust::println(sizeof(hust::Solver::globalEnv));
	//hust::println(sizeof(hust::Input));

	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;

	//goal.justLocalSearch(); 
	goal.solverByEAX(); 
	hust::deallocGlobalMem();

	return 0;
}