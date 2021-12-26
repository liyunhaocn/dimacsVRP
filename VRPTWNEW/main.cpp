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
	//globalEnv  = new Environment("../Instances/Homberger/C1_6_6.txt");
	//globalEnv  = new Environment("../Instances/Homberger/RC1_8_1.txt");
	// 
	//75296223
	globalEnv = new Environment("../Instances/Homberger/RC2_6_4.txt");

	cfg = new hust::Configuration();
	//lyh::MyString ms;

	solveCommandLine(argc, argv, cfg, globalEnv);

	if (globalEnv->seed == -1) {
		globalEnv->seed = std::time(nullptr) + std::clock();
	}

	//globalEnv->seed = 1611589828;
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
	DisType lyhRec = DisInf;

	Goal() {
		eaxYearTable = Vec<Vec<LL>>
		(cfg->popSize, Vec<LL>(cfg->popSize));
		lyhRec = DisInf;
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

	bool updateBestSolRec(hust::Solver& pTemp) {
		if (pTemp.RoutesCost < lyhRec) {
			lyhRec = pTemp.RoutesCost;
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
			int eaxState = 0;
			if (kind == 0) {
				eaxState = eax.doNaEAX(pa, pb, pc);
			}
			else {
				eaxState = eax.doPrEAX(pa, pb, pc);
			}

			if (eaxState==-1) {
				//println("eaxState==-1,kind",kind);
				return -1;
			}
			if (eax.abCycleSet.size() <= 1) {
				//println("eax.abCycleSet.size() <= 1,kind", kind);
				return -1;
			}

			if (eax.repairSolNum == 0) {
				continue;
			}

			auto  newCus = EAX::getDiffCusofPb(pa,pc);

			pc.mRLLocalSearch(newCus);
			//pc.mRLLocalSearch({});
			bool up = updateBestSolRec(pc);
			if (up) {
				ch = 1;
				retState = 1;
				println("cost:", lyhRec, "eax local update,kind:", kind);
			}
			else {
				eax.generateCycles();
			}

			pc.ruinLocalSearch(cfg->ruinC_);
			up = updateBestSolRec(pc);
			if (up) {
				retState = 1;
				println("cost:", lyhRec, "eax ruin local update,kind:", kind);
			}

			if (pc.RoutesCost < paBest.RoutesCost) {
				paBest = pc;
			}
		}
		pa = paBest;
		return retState;
	}

	bool solverByEAX() {

		lyhRec = DisInf;

		Timer t1(cfg->runTimer);
		t1.setLenUseSecond(cfg->runTimer);
		t1.reStart();

		Vec<Solver> pool;
		pool.reserve(cfg->popSize);

		for (int i = 0; i < cfg->popSize; ++i) {
			Environment envt = *globalEnv;
			envt.seed = (globalEnv->seed % Mod) + ((i + 1) * (myRand->pick(10000007))) % Mod;
			Solver st(*globalInput);
			st.initSolution(i%2);

			st.adjustRN();
			//saveSlnFile(input, pBest.output, cfg, globalEnv);
			st.mRLLocalSearch({});
			st.ruinLocalSearch(1);
			updateBestSolRec(st);
			pool.push_back(st);
		}

		LL MAiter = 0;

		Vec<int> solConti(cfg->popSize, 1);

		int contiNotDown = 1;
		int strategy = 0;

		#if DIMACSGO
		while (lyhRec > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (lyhRec > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			++MAiter;

			auto papb = getpairOfPaPb();
			int paIndex = papb[0];
			int pbIndex = papb[1];
			
			eaxYearTable[paIndex][pbIndex] = MAiter;

			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];

			//println("paIndex:", paIndex);
			//println("pbIndex:", pbIndex);
			//bool ruina = pa.ruinLocalSearch(1);
			//bool ruinb = pb.ruinLocalSearch(1);
			//if (updateBestSolRec(pa)) {
			//	println("cost:", lyhRec, "ruin a update");
			//}
			//if (updateBestSolRec(pb)) {
			//	println("cost:", lyhRec, "ruin b update");
			//}

			int isUp = doTwoKindEAX(pa, pb, strategy);

			//debug(contiNotDown);
			if (isUp == -1 || contiNotDown == 41) {
				contiNotDown = 1;

				Solver& bad = pa.RoutesCost > pb.RoutesCost ? pa : pb;

				Solver badclone = bad;

				bool isabcygreater2 = false;

				for (int i = 0; i < 2; ++i) {
					bool isper = bad.perturbBaseRuin(3, cfg->ruinC_);
					if (isper) {
						int abcyNum = EAX::getabCyNum(bad, badclone);
						if (abcyNum >= 2) {
							debug(abcyNum);
							isabcygreater2 = true;
							break;
						}
					}
					else {
						bad = badclone;
					}
				}

				if (isabcygreater2 == false) {
					badclone = bad;
					for (int i = 0; i < 10; ++i) {
						bad.patternAdjustment(50);
						//bad.reCalRtsCostAndPen();
						int abcyNum = EAX::getabCyNum(bad, badclone);
						if (abcyNum >= 2) {
							debug(abcyNum);
							//debug(bad.RoutesCost);
							isabcygreater2 = true;
							break;
						}
					}
				}
				//bad.ruinLocalSearch(30);

				auto diffcuses = EAX::getDiffCusofPb(bad, badclone);
				bad.mRLLocalSearch(diffcuses);
				bad.ruinLocalSearch(1);
				if (updateBestSolRec(bad)) {
					contiNotDown = 1;
					println("cost:", lyhRec, "patternAdjustment bad update");
				}
			}

			if (isUp == 1) {
				contiNotDown = 1;
			}
			else {
				++contiNotDown;
			}

			if (contiNotDown == 20) {
				strategy = 1 - strategy;
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
		saveSlnFile(*globalInput, output, cfg, *globalEnv);
		t1.disp();

		return true;
	}

	bool justLocalSearch() {

		Timer t1(cfg->runTimer);
		t1.setLenUseSecond(cfg->runTimer);
		t1.reStart();

		Solver st(*globalInput);
		st.initSolution(myRand->pick(2));

		st.adjustRN();
		//st.saveOutAsSintefFile("minR");
		st.mRLLocalSearch({});

		//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
		auto pBest = st;

		int contiNotDown = 1;

		#if DIMACSGO
		while (lyhRec > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (lyhRec > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			st = pBest;
			int c_ = std::min<int>(cfg->ruinC_,contiNotDown);
			st.ruinLocalSearch(c_);
			bool up2 = updateBestSolRec(st);
			if (up2) {
				println("cost:", lyhRec);
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
	//hust::println(sizeof(hust::Solver::globalEnv));
	//hust::println(sizeof(hust::Input));

	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;
	//goal.justLocalSearch(); 
	goal.solverByEAX(); 
	hust::deallocGlobalMem();

	return 0;
}