// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

//#include "Utility.h"
#include "Solver.h"
//#include "Problem.h"
//#include "Configuration.h"
//#include "Flag.h"
#include "EAX.h"

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {


	globalCfg = new hust::Configuration();
	globalCfg->inputPath = "../Instances/Homberger/C2_8_7.txt";

	//globalEnv = new Environment("../Instances/Solomon/C101.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_3.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_1.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_4_2.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C2_10_6.txt");
	//globalEnv  = new Environment("../Instances/Homberger/RC1_8_5.txt");
	//globalEnv = new Environment("../Instances/Homberger/C2_8_7.txt");

	globalCfg->solveCommandLine(argc, argv);

	if (globalCfg->seed == -1) {
		globalCfg->seed = std::time(nullptr) + std::clock();
	}

	//globalEnv->seed = 1611589828;
	//globalEnv->seed = 1611589111;
	//globalEnv->seed = 1640620823;
	//globalEnv->seed = 1640660545;//RC2_6_4
	//globalEnv->seed = 1640858824;//RC2_6_4
	//globalEnv->seed = 1640880535;

	globalCfg->show();
	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<LL> >
		(globalInput->custCnt + 1, Vec<LL>(globalInput->custCnt + 1, 0));

	bks = new BKS();
	//TODO[lyh][0]:
	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalInput;
	delete bks;
	return true;
}

struct Goal {
	
	Vec<Vec<LL>> eaxYearTable;
	
	Vec<Solver> pool;

	Goal() {
		eaxYearTable = Vec<Vec<LL>>
		(globalCfg->popSize, Vec<LL>(globalCfg->popSize));
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
				bool isup = bks->updateBKS(pc, "eax local update, kind:" + std::to_string(kind));
				if (isup) {
					ch = 1;
					retState = 1;
					//println("bestSolFound cost:", bestSolFound.RoutesCost, ", kind, "choosecyIndex:", eax.choosecyIndex, "chooseuId:", eax.unionIndex);
				}
			}

		}

		if (paBest.RoutesCost < pa.RoutesCost) {
			if (paBest.RoutesCost < pb.RoutesCost) {
				auto diffwithPa = EAX::getDiffCusofPb(paBest,pa);
				auto diffwithPb = EAX::getDiffCusofPb(paBest,pb);
				if (diffwithPa.size() <= diffwithPb.size()) {
					pa = paBest;
				}
				else {
					println("replace with pb,kind:",kind);
					pb = paBest;
				}
			}
			else {
				pa = paBest;
			}
		}
		return retState;
	}

	bool perturbThePop() {

		auto pertuOrder = Vec<int>(pool.size(), 0);
		std::iota(pertuOrder.begin(), pertuOrder.end(), 0);
		std::sort(pertuOrder.begin(), pertuOrder.end(), [&](int x, int y) {return
			pool[x].RoutesCost < pool[y].RoutesCost;
		});
		//unsigned shuseed = myRand->pickRandSeed();
		//std::shuffle(pertuOrder.begin(), pertuOrder.end(), std::default_random_engine(shuseed));

		#if 0
		for (int peri = 0; peri < pool.size(); ++peri) {
			int pi = pertuOrder[peri];
			auto& sol = pool[pi];
			auto before = sol.RoutesCost;
			Solver sclone = sol;
			sclone.resetSol();
			int initKind = myRand->pick(5);
			sclone.initSolution(initKind);
			sclone.adjustRN();
			sclone.mRLLocalSearch(0, {});
			//sclone.ruinLocalSearch(1);
			sol = sclone;

			auto after = sol.RoutesCost;
			println("before:", before, "after:", after,"initKind:", initKind);
		}
		#else	

		for (int peri = 0; peri < pool.size(); ++peri) {
			int pi = pertuOrder[peri];
			auto& sol = pool[pi];
			auto before = sol.RoutesCost;
			Solver sclone = sol;
			//sclone.saveOutAsSintefFile();
			for (int i = 0; i < 10; ++i) {
				int kind = myRand->pick(4);
				if (kind < 3) {
					int clearEPkind = myRand->pick(6);
					sclone.perturbBaseRuin(kind, 4*globalCfg->ruinC_, clearEPkind);
				}
				else {

					int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt);
					sclone.patternAdjustment(step);
				}
				
				auto diff = EAX::getDiffCusofPb(sol, sclone);
				if (diff.size() > sol.input.custCnt*0.2) {
					sclone.mRLLocalSearch(1, diff);
					sol = sclone;
					break;
				}
				else {
					sclone = sol;
				}

			}
		}
		#endif // 0

		return true;
	}

	int naMA(Solver& pa,Solver& pb) { // 1 代表更新了最优解 0表示没有

		
		int strategy = 0;
		int popSize = globalCfg->popSize;

		int updateState = 0;
		
		for (int contiNotDown = 1; contiNotDown < 20; ++contiNotDown) {

			int isabState = doTwoKindEAX(pa, pb, strategy);
			if (isabState==1) {
				updateState = 1;
				contiNotDown = 1;
			}
			else {
				strategy = strategy + 1;
				if (strategy == 2) {
					break;
				}
			}
		}

		return updateState;
		//return updateState;
	}

	bool initPopulation() {

		//for (int initKind = 0; initKind < 5; ++initKind) {
		//
		//	Solver st;
		//	st.initSolution(initKind);
		//	println("initKind:", initKind);
		//	char a[20];
		//	snprintf(a, 20, "k%d", initKind);
		//	st.saveOutAsSintefFile(a);
		//}

		pool.reserve(globalCfg->popSize);

		//Vec<int> kset = { 1,4 };
		Vec<int> kset = { 0,1,2,3,4};
		myRand->shuffleVec(kset);
		
		int ourTarget = 0;

		#if DIMACSGO
		ourTarget = globalInput->dimacsRecRN;
		#else
		if (globalCfg->breakRecord) {
			ourTarget = globalInput->sintefRecRN - 1;
		}
		else {
			ourTarget = globalInput->sintefRecRN;
		}
		#endif // DIMACSGO

		Solver s0;
		s0.initSolution(kset[0]);
		s0.adjustRN(ourTarget);
		ourTarget = s0.rts.cnt;
		s0.mRLLocalSearch(0, {});
		s0.ruinLocalSearch(globalCfg->ruinC_);
		bks->updateBKS(s0);
		pool.push_back(s0);
		println("s0 inint kind:", kset[0]);

		int produceNum = globalCfg->popSize * 2;
		int i = 1;
		while (++i <= produceNum && pool.size() < globalCfg->popSize) {
			Solver st;
			st.initSolution(kset[i% kset.size()]);
			st.adjustRN(ourTarget);
			if (st.rts.cnt == ourTarget) {
				st.mRLLocalSearch(0, {});
				st.ruinLocalSearch(globalCfg->ruinC_);
				bks->updateBKS(st);
				println("s inint kind:", kset[i % kset.size()]);
				pool.push_back(st);
			}
		}

		if (pool.size() < globalCfg->popSize) {
			println("dont get enough population");
			int poolCurSize = pool.size();
			int i = 0;
			while (pool.size() < globalCfg->popSize) {
				Solver sclone = pool[i++];
				sclone.patternAdjustment(globalInput->custCnt);
				sclone.reCalRtsCostAndPen();
				pool.push_back(sclone);
			}
		}
		return true;
	}

	bool TwoAlgCombine() {

		initPopulation();
		Timer t1(globalCfg->runTimer);
		t1.setLenUseSecond(globalCfg->runTimer);
		t1.reStart();

		int contiNotDown = 1;
		//TODO[0]:先使用哪个
		int whichAlg = 0; // 0代表局部搜搜 1 代表交叉
		int popSize = globalCfg->popSize;

		#if DIMACSGO
		while (bks->bestSolFound.RoutesCost > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (bks->bestSolFound.RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			int paIndex = myRand->pick(popSize);
			int pbIndex = (paIndex + 1) % popSize;
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];

			int upState = 0;

			if (whichAlg == 0) {

				auto bestClone = bks->bestSolFound;
				//if (myRand->pick(10) == 0) {
				//	int step = myRand->pick(globalInput->custCnt * 0.2, globalInput->custCnt* 0.4);
				//	println("bestClone.patternAdjustment");
				//	bestClone.patternAdjustment(step);
				//}

				bestClone.ruinLocalSearch();
				if (bks->updateBKS(bestClone,"ruin bestSolFound")) {
					upState = 1;
				}
				for (auto& better:pool) {
					better.ruinLocalSearch();
					if (bks->updateBKS(better, "ruin localSearch")) {
						upState = 1;
					}
				}

				//bestClone.LSBasedRuinAndRuin();
				//if (bks->updateBKS(bestClone,"ruin bestSolFound")) {
				//	upState = 1;
				//}
				//for (auto& better:pool) {
				//	better.LSBasedRuinAndRuin();
				//	if (bks->updateBKS(better, "ruin localSearch")) {
				//		upState = 1;
				//	}
				//}

			}
			else {
				
				int before = EAX::getabCyNum(pa,pb);
				if (before <= 1) {
					perturbThePop();
					int after = EAX::getabCyNum(pa, pb);
					println("before:", before, "after:", after,"abCyNum <= 1");
				}
				
				upState = naMA(pa,pb);
			}

			if (upState == 1) {
				contiNotDown = 1;
			}
			else {
				++contiNotDown;
			}

			if (contiNotDown % 4 == 0) {
				++whichAlg;
				whichAlg %= 2;
			}

			if (contiNotDown == 100) {
				int before = EAX::getabCyNum(pa, pb);
				perturbThePop();
				int after = EAX::getabCyNum(pa, pb);
				println("before:", before, "after:", after,"contiNotDown:", contiNotDown);
				contiNotDown = 1;
			}
		}

		Output output = bks->bestSolFound.saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(output);
		t1.disp();

		return true;
	}

	bool justLocalSearch() {

		Timer t1(globalCfg->runTimer);
		t1.setLenUseSecond(globalCfg->runTimer);
		t1.reStart();

		Solver st;
		//st.initSolution(myRand->pick(2));
		st.initSolution(0);

		st.adjustRN(globalInput->dimacsRecRN);
		//st.saveOutAsSintefFile("minR");
		st.mRLLocalSearch(0,{});

		//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
		auto pBest = st;

		int contiNotDown = 1;

		Vec<int> runSize = { 1,globalCfg->ruinC_,2*globalCfg->ruinC_};
		int index = 0;

		//ProbControl pc(2*globalCfg->ruinC_);
		#if DIMACSGO
		while (bks->bestSolFound.RoutesCost > globalInput->dimacsRecRL && !t1.isTimeOut()) {
		#else
		while (bks->bestSolFound.RoutesCost > globalInput->sintefRecRL && !t1.isTimeOut()) {
		#endif // DIMACSGO

			//st.ruinLocalSearch(c_ + 1);
			//st.ruinLocalSearch(globalCfg->ruinC_);
			st.ruinLocalSearch(runSize[index]);
			
			bks->updateBKS(st);

			if (st.RoutesCost < pBest.RoutesCost) {
				//++pc.data[c_];
				pBest = st;
				contiNotDown = 1;
				index = 0;
			}
			else {
				st = pBest;
				++contiNotDown;
			}

			if (contiNotDown > 50) {
				++index;
				debug(index);
				if (index == runSize.size()) {
					st = pBest;
					if (myRand->pick(3) == 0) {
						int step = myRand->pick(st.input.custCnt*0.2, st.input.custCnt * 0.4);
						st.patternAdjustment(st.input.custCnt);
						int abcyNum = EAX::getabCyNum(st,pBest);
						println("st.patternAdjustment,cyNum",abcyNum);
					}
					
				}
				index %= runSize.size();
				contiNotDown = 1;
			}
		}

		Output output = pBest.saveToOutPut();
		output.runTime = t1.getRunTime();
		saveSlnFile(output);
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
	//hust::println(sizeof(hust::Solver::globalCfg));
	//hust::println(sizeof(hust::Input));

	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;

	//goal.justLocalSearch(); 
	goal.TwoAlgCombine();
	hust::deallocGlobalMem();

	return 0;
}