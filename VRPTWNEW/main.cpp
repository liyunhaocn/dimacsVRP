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
		//hust::println("cost:", pBest.RoutesCost);
		return true;
	}
	else {
		return false;
	}
	return false;
};

bool solverByEAX(int argc, char* argv[]) {

	//Environment env("../Instances/Solomon/C101.txt");
	//Environment env("../Instances/Homberger/RC1_8_3.txt");
	Environment env("../Instances/Homberger/RC1_8_1.txt");
	//Environment env("../Instances/Homberger/C1_2_8.txt");

	cfg = new hust::Configuration();
	//lyh::MyString ms;

	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}
	env.seed = 1611589828;
	env.show();
	cfg->show();

	myRand = new Random(env.seed);
	myRandX = new RandomX(env.seed);

	Input input(env);
	Solver pBest(input, env);
	// TODO[lyh][0]:一定要记得cfg用cusCnt合法化一下
	cfg->repairByCusCnt(input.custCnt);

	yearTable = new Vec<Vec<LL> >
		(input.custCnt + 1, Vec<LL>(input.custCnt + 1, 0));
	//TODO[lyh][0]:
	pBest.RoutesCost = DisInf;

	Timer t1(cfg->runTimer);
	t1.setLenUseSecond(cfg->runTimer);
	t1.reStart();

	Vec<Solver> pool;
	pool.reserve(cfg->popSize);

	for (int i = 0; i < cfg->popSize; ++i) {
		Environment envt = env;
		envt.seed = (env.seed % Mod) + ((i + 1) * (myRand->pick(10000007))) % Mod;
		Solver st(input, envt);
		st.minimizeRN();
		//saveSlnFile(input, pBest.output, cfg, env);
		st.mRLLocalSearch({});
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

	int paUpNum = 0;
	int paNotUpNum = 0;

	int wtihSubcyNum = 0;
	int wtihSubcyRepair = 0;
	int wtihoutSubcyNum = 0;
	int wtihoutSubcyRepair = 0;

	int repairNum = 0;
	int bestSolContiNotUp = 1;

	Vec<int> solConti(cfg->popSize, 1);


	while (pBest.RoutesCost > input.sintefRecRL && !t1.isTimeOut()) {
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

		
		for (int ch = 1; ch <= 10; ++ch) {

			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			Solver pc = pa;

			//unsigned eaxSeed = (env.seed % Mod) + ((myRand->pick(10000007))) % Mod;
			EAX eax(pa, pb);
			Vec<int> newCus;
			// newCus = eax.doNaEAX(pa, pb, pc);
			if (ch <= 9) {
				newCus = eax.doNaEAX(pa, pb, pc);
			}
			else if (ch==10) {
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
					bool ruined = pa.ruinLocalSearch(myRand->pick(10) + 1);
					println("ruined: ", ruined, " paIndex:", paIndex, " pbIndex:", pbIndex);
				}
				continue;
			}

			++repairNum;

			pc.mRLLocalSearch(newCus);
			//pc.mRLLocalSearch({});
			bool up = updateBestSol(pBest, pc);
			if (up) {
				println("MAiter:", MAiter, " rep:", repairNum, " paUp:", paUpNum, " paNUp:", paNotUpNum, " pa:", paIndex, " pb:", pbIndex, " cost", pBest.RoutesCost,"bSolNUp", bestSolContiNotUp);
				bestSolContiNotUp = 1;
			}
			
			if (pc.RoutesCost < pa.RoutesCost) {
				pa = pc;
				ch = 1;
				++paUpNum;
			}
			else {
				++paNotUpNum;
			}
		}
		++bestSolContiNotUp;
		//println("bestSolContiNotUp:", bestSolContiNotUp);
		if (bestSolContiNotUp >= 50) {
			
			for (int i = 0; i < pool.size(); ++i) {
				Solver& si = pool[i];
				bool isruined = si.ruinLocalSearch(myRand->pick(10)+1);
				if (isruined) {
					println("i:",i,"runined can update");
					bool up = updateBestSol(pBest, si);
					if (up) {
						println("i:", i, "runined update best sol");
					}
				}
				else {
					auto sclone = si;
					sclone.patternAdjustment(input.custCnt*0.1);
					sclone.mRLLocalSearch({});
					if (sclone.RoutesCost < si.RoutesCost) {
						si = sclone;
						println("i:", i, "patternAdjustment can update");
						bool up = updateBestSol(pBest, si);
						if (up) {
							println("i:", i, "patternAdjustment update best sol");
						}
					}
					else {
						println("i:", i, "cant update");
					}
				}
			}

			bestSolContiNotUp = 1;
		}

	}

	Output output = pBest.saveToOutPut();
	output.runTime = t1.getRunTime();
	saveSlnFile(input, output, cfg, env);
	t1.disp();

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete cfg;

	return true;
}

bool justLocalSearch(int argc, char* argv[]) {

	//Environment env("../Instances/Solomon/C101.txt");
	//Environment env("../Instances/Homberger/RC1_8_3.txt");
	//Environment env("../Instances/Homberger/C1_4_2.txt");
	Environment env("../Instances/Homberger/RC1_8_1.txt");

	cfg = new Configuration();

	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}
	env.seed = 1611589828;
	env.show();
	cfg->show();

	myRand = new Random(env.seed);
	myRandX = new RandomX(env.seed);

	Input input(env);
	Solver pBest(input, env);
	// TODO[lyh][0]:一定要记得cfg用cusCnt合法化一下
	cfg->repairByCusCnt(input.custCnt);

	yearTable = new Vec< Vec<LL> >
		(input.custCnt + 1, Vec<LL>(input.custCnt + 1, 0));
	squIter = 1;

	int contiNoDown = 1;

	Timer t1(cfg->runTimer);
	t1.setLenUseSecond(cfg->runTimer);
	t1.reStart();

	Solver st(input, env);
	st.minimizeRN();
	st.saveOutAsSintefFile("minR");
	st.mRLLocalSearch({});
	st.saveOutAsSintefFile("minL");
	//TODO[lyh][0]:
	pBest = st;

	//updateBestSol(st);

	int ruinNum = 0;
	int parNum = 0;

	Vec<int> cusOrder(input.custCnt,1);
	std::iota(cusOrder.begin(), cusOrder.end(),1);

	while (pBest.RoutesCost > input.sintefRecRL && !t1.isTimeOut()) {

		st = pBest;

		int rnum = myRand->pick(10) + 1;
		bool isRuin = st.ruinLocalSearch(rnum);
		bool up1 = updateBestSol(pBest, st);
		if (up1) {
			println("cost:", pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
			contiNoDown = 1;
			++ruinNum;
		}

		//st.perturb(per);
		st.patternAdjustment();
		st.mRLLocalSearch({});
		bool up2 = updateBestSol(pBest, st);
		if (up2) {
			++parNum;
			println("cost:",pBest.RoutesCost, " ruinNum:", ruinNum, " parNum:", parNum);
		}
		
	}

	Output output = pBest.saveToOutPut();
	output.runTime = t1.getRunTime();
	saveSlnFile(input, output, cfg, env);
	t1.disp();

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete cfg;

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