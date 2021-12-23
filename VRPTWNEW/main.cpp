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

bool allocGlobalMem(int argc, char* argv[]) {

	//globalEnv = new Environment("../Instances/Solomon/C101.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_3.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_1.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_4_2.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_6_6.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_8_2.txt");
	// 
	//75296223
	globalEnv = new Environment("../Instances/Homberger/RC2_6_4.txt");

	cfg = new hust::Configuration();
	//lyh::MyString ms;

	solveCommandLine(argc, argv, cfg, globalEnv);

	if (globalEnv->seed == -1) {
		globalEnv->seed = std::time(nullptr) + std::clock();
	}

	globalEnv->seed = 1611589828;
	//globalEnv->seed = 1611589111;
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
	return true;
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

struct Goal {
	
	Vec<Vec<LL>> eaxYearTable;
	Solver* pBest = nullptr;

	Goal() {
		eaxYearTable = Vec<Vec<LL>>
		(cfg->popSize, Vec<LL>(cfg->popSize));
		pBest = new Solver(*globalInput, *globalEnv);
		pBest->RoutesCost = DisInf;
	}

	Vec<int> getpairOfPaPb() {

		int paIndex = myRand->pick(cfg->popSize);
		int pbIndex = (paIndex + 1) % cfg->popSize;
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
		return { paIndex,pbIndex };
	}

	bool updateBestSol(hust::Solver& pTemp) {
		if (pTemp.RoutesCost < pBest->RoutesCost) {
			*pBest = pTemp;
			//pBest.saveToOutPut();
			//pBest.saveOutAsSintefFile();
			//hust::println("cost:", pBest->RoutesCost);
			return true;
		}
		else {
			return false;
		}
		return false;
	};

	int doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

		int repairNum = 0;

		int retState = 0; // 0 表示没有成功更新最优解，1表示更新了最优解 -1表示这两个解无法进行eax
		int Nch = 10;

		EAX eax(pa, pb);
		eax.generateCycles();
		Solver paBest = pa;

		for (int ch = 1; ch <= Nch; ++ch) {

			Solver pc = pa;
			if (kind == 0) {
				eax.doNaEAX(pa, pb, pc);
			}
			else {
				eax.doPrEAX(pa, pb, pc);
			}

			if (eax.abCycleSet.size() <= 1) {
				println("eax.abCycleSet.size() <= 1");
				return -1;
			}

			if (eax.repairSolNum == 0) {
				if (eax.abCycleSet.size() == 0) {
					println("eax.abCycleSet.size() == 0");
					println("kind:", kind);
					return -1;
				}
				continue;
			}

			EAX eaxGetdiff(pa,pc);
			auto newCus = eaxGetdiff.getDiffCusofPb();

			pc.mRLLocalSearch(newCus);
			//pc.mRLLocalSearch({});
			bool up = updateBestSol(pc);
			if (up) {
				retState = 1;
				println("cost:", pBest->RoutesCost, "eax local update,kind:", kind);
			}

			for (int i = 0; i < 20; ++i) {
				int ruinCusNum =  i/5 + 1;
				//int ruinCusNum = i + 1;
				bool ruin = pc.ruinLocalSearch(1, ruinCusNum);
				if (ruin) {
					i = 0;
				}
				bool up = updateBestSol(pc);
				if (up) {
					retState = 1;
					println("cost:", pBest->RoutesCost, "eax ruin local update,kind:", kind);
					i = 0;
				}
			}

			if (pc.RoutesCost < paBest.RoutesCost) {
				paBest = pc;
			}
		}
		pa = paBest;
		return retState;
	}

	bool solverByEAX() {

		pBest->RoutesCost = DisInf;

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

			if (st.RoutesCost < pBest->RoutesCost) {
				*pBest = st;
			}
			pool.push_back(st);
		}

		LL MAiter = 0;

		Vec<int> solConti(cfg->popSize, 1);

		int contiNotDown = 1;
		int strategy = 0;

		while (pBest->RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {
			++MAiter;

			auto papb = getpairOfPaPb();
			int paIndex = papb[0];
			int pbIndex = papb[1];
			
			eaxYearTable[paIndex][pbIndex] = MAiter;

			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];

			//println("paIndex:", paIndex);
			//println("pbIndex:", pbIndex);

			for (int i = 0; i < 20; ++i) {
				int ruinCusNum = i / 5 + 1;
				bool ruina = pa.ruinLocalSearch(1, ruinCusNum);
				bool ruinb = pb.ruinLocalSearch(1, ruinCusNum);
				if (ruina || ruinb) {
					i = 0;
				}
				if (updateBestSol(pa)) {
					println("cost:", pBest->RoutesCost, "ruin a or b update");
					i = 0;
				}
			}

			int isUp = doTwoKindEAX(pa, pb, strategy);
			debug(contiNotDown);
			if (isUp == -1) {
				Solver& bad = pa.RoutesCost > pb.RoutesCost ? pa : pb;
				Solver badclone = pa.RoutesCost > pb.RoutesCost ? pa : pb;

				DisType olddis = bad.RoutesCost;
				bad.patternAdjustment(globalInput->custCnt * 0.1);
				EAX eaxt(bad, badclone);
				auto diffcuses = eaxt.getDiffCusofPb();
				bad.mRLLocalSearch(diffcuses);
				if (updateBestSol(bad)) {
					contiNotDown = 1;
					println("cost:", pBest->RoutesCost, "patternAdjustment bad update");
				}
			}

			
			if (isUp == 1) {
				contiNotDown = 1;
			}
			else {
				++contiNotDown;
			}
			if (contiNotDown >= 20) {
				strategy = 1 - strategy;
				contiNotDown = 1;
			}

			#if 0
			if(contiNotDown==29) {

				Solver& bad = pa.RoutesCost > pb.RoutesCost ? pa : pb;
				Solver badclone = pa.RoutesCost > pb.RoutesCost ? pa : pb;

				DisType olddis = bad.RoutesCost;
				bad.patternAdjustment(globalInput->custCnt*0.1);
				EAX eaxt(bad, badclone);
				auto diffcuses = eaxt.getDiffCusofPb();
				bad.mRLLocalSearch(diffcuses);
				if (updateBestSol(bad)) {
					contiNotDown = 1;
					println("cost:", pBest->RoutesCost, "patternAdjustment bad update");
				}
				//for (int i = 0; i < 200; ++i) {
				//	int ruinCusNum = i / 5 + 1;
				//	bool ruin = bad.ruinLocalSearch(1, ruinCusNum);
				//	if (ruin) {
				//		i = 0;
				//	}
				//	if (updateBestSol(bad)) {
				//		contiNotDown = 1;
				//		println("cost:",pBest->RoutesCost,"ruin bad update");
				//		i = 0;
				//	}
				//}
				contiNotDown = 1;
			}
			#endif // 0

			

		}

		Output output = pBest->saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(*globalInput, output, cfg, *globalEnv);
		t1.disp();

		return true;
	}

	bool justLocalSearch() {

		Timer t1(cfg->runTimer);
		t1.setLenUseSecond(cfg->runTimer);
		t1.reStart();

		Solver st(*globalInput, *globalEnv);
		st.minimizeRN();
		//st.saveOutAsSintefFile("minR");
		st.mRLLocalSearch({});

		int ruinNum = 0;
		int parNum = 0;

		Vec<int> cusOrder(globalInput->custCnt, 1);
		std::iota(cusOrder.begin(), cusOrder.end(), 1);

		int avg = globalInput->custCnt / st.rts.cnt;
		int contiNoDown = 1;

		Vec<Vec<int>> table(3, Vec<int>(50, 0));

		int kind = 1;
		int rnum = 1;

		std::fill(st.P.begin(), st.P.end(), 1);

		*pBest = st;

		while (pBest->RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {

			st = *pBest;

			#if 1
			if (contiNoDown >= 300) {

				rnum = (rnum + 3) % 40 + 1;
				//std::min<int>(rnum + 1, 20);
				contiNoDown = 1;
			}

			bool isRuin = st.ruinLocalSearch(kind, rnum);
			bool up1 = updateBestSol(st);
			if (up1) {
				//println("cost:", pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
				println("cost:", pBest->RoutesCost, " rnum:", rnum);
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
			bool up2 = updateBestSol(st);
			if (up2) {
				++parNum;
				println("cost:", pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
			}
			#endif // 0
		}

		Output output = pBest->saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(*globalInput, output, cfg, *globalEnv);
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
	//hust::println(sizeof(hust::Solver::env));
	//hust::println(sizeof(hust::Input));

	/*for (;;) {
		hust::myRand = new hust::Random(0);
		hust::myRandX = new hust::RandomX(0);
		delete hust::myRand;
		delete hust::myRandX;
	}
	return 0;*/
	//hust::solverByEAX(argc, argv);return 0;
	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;
	goal.justLocalSearch(); 
	//goal.solverByEAX(); 
	hust::deallocGlobalMem();

	return 0;
}