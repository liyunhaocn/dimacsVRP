
#include "EAX.h"
#include "Goal.h"

namespace hust {

Goal::Goal():gloalTimer(0){

	eaxYearTable = Vec<Vec<LL>>
		(globalCfg->popSize, Vec<LL>(globalCfg->popSize));

	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

}

Vec<int> Goal::getpairOfPaPb() {

	int paIndex = -1;
	int pbIndex = -1;
	int retNum = 0;

	for (int i = 1; i < globalCfg->popSize; ++i) {
		for (int j = 0; j < globalCfg->popSize; ++j) {
			if (i != j) {
				if (retNum == 0) {
					paIndex = i;
					pbIndex = j;
				}
				else {
					if (eaxYearTable[i][j] < eaxYearTable[paIndex][pbIndex]) {
						retNum = 1;
						paIndex = i;
						pbIndex = j;
					}
					else if (eaxYearTable[i][j] == eaxYearTable[paIndex][pbIndex]) {
						++retNum;
						if (myRand->pick(retNum) == 0) {
							paIndex = i;
							pbIndex = j;
						}
					}
				}
			}
		}
	}

	return { paIndex,pbIndex };
}

int Goal::doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

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

		if (eaxState == -1) {
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
		auto newCus = EAX::getDiffCusofPb(pa, pc);

		if (newCus.size() > 0) {
			pc.mRLLocalSearch(1, newCus);
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
			
			bool isup = bks->updateBKS(pc, "time:" + std::to_string(gloalTimer.getRunTime()) + " eax ls, kind:" + std::to_string(kind));
			if (isup) {
				ch = 1;
				retState = 1;
				//INFO("bestSolFound cost:", bestSolFound.RoutesCost, ", kind, "choosecyIndex:", eax.choosecyIndex, "chooseuId:", eax.unionIndex);
			}
		}

	}

	if (paBest.RoutesCost < pa.RoutesCost) {
		if (paBest.RoutesCost < pb.RoutesCost) {
			auto diffwithPa = EAX::getDiffCusofPb(paBest, pa);
			auto diffwithPb = EAX::getDiffCusofPb(paBest, pb);
			if (diffwithPa.size() <= diffwithPb.size()) {
				pa = paBest;
			}
			else {
				//INFO("replace with pb,kind:", kind);
				pb = paBest;
			}
		}
		else {
			pa = paBest;
		}
	}
	return retState;
}

bool Goal::perturbThePop() {

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
		INFO("before:", before, "after:", after, "initKind:", initKind);
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
				int ruinCusNum = std::min<int>(globalInput->custCnt/2, 4 * globalCfg->ruinC_);
				sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
			}
			else {

				int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt);
				sclone.patternAdjustment(step);
			}

			auto diff = EAX::getDiffCusofPb(sol, sclone);
			if (diff.size() > sol.input.custCnt * 0.2) {
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

int Goal::naMA(Solver& pa, Solver& pb) { // 1 代表更新了最优解 0表示没有
	
	int updateState = 0;
	int isabState = doTwoKindEAX(pa, pb, 0);
	if (isabState == 1) {
		updateState = 1;
	}
	//int before = EAX::getabCyNum(pa, pb);
	isabState = doTwoKindEAX(pa, pb, 1);
	if (isabState == 1) {
		updateState = 1;
	}
	return updateState;
}

bool Goal::initPopulation() {

	pool.reserve(globalCfg->popSize);
	//Vec<int> kset = { 1,4 };
	Vec<int> kset = { 4,3,2,1,0};
	//myRand->shuffleVec(kset);

	int ourTarget = globalCfg->lkhRN;

	Solver s0;
	s0.initSolution(kset[0]);
	s0.adjustRN(ourTarget);

	INFO("s0.RoutesCost:",s0.RoutesCost);
	ourTarget = s0.rts.cnt;
	s0.mRLLocalSearch(0, {});

	INFO("s0.RoutesCost:", s0.RoutesCost);
	INFO("sol0 inint kind:", kset[0]);

	bks->updateBKS(s0);

	s0.ruinLocalSearchNotNewR(1);

	pool.push_back(s0);
	
	int produceNum = globalCfg->popSize * 2;
	int i = 1;
	while (++i <= produceNum && pool.size() < globalCfg->popSize) {
		Solver st;
		int initKind = kset[i % kset.size()];
		st.initSolution(initKind);
		
		st.adjustRN(ourTarget);
		if (st.rts.cnt == ourTarget) {
			st.mRLLocalSearch(0, {});
			//st.callTowKindOfRuin(0,1000);
			bks->updateBKS(st);
			INFO("sol inint kind:", kset[i % kset.size()]);
			pool.push_back(st);
		}
	}

	if (pool.size() < globalCfg->popSize) {
		INFO("dont get enough population");
		int poolCurSize = pool.size();
		int i = 0;
		while (pool.size() < globalCfg->popSize) {
			Solver sclone = pool[i++];
			sclone.patternAdjustment(globalInput->custCnt);
			sclone.reCalRtsCostAndPen();
			sclone.mRLLocalSearch(0, {});
			//sclone.Simulatedannealing(0,1000);
			bks->updateBKS(sclone);
			pool.push_back(sclone);
		}
	}
	return true;
}

bool Goal::saveSlnFile() {

	Input& input = *globalInput;

	MyString ms;
	// 输出 tm 结构的各个组成部分
	//std::string day = /*ms.LL_str(d.year) + */ms.LL_str(d.month) + ms.LL_str(d.day);

	std::string type = "[dim]";

	std::string path = type + __DATE__;
	path += std::string(1, '_') + __TIME__;

	for (auto& c : path) {
		if (c == ' ' || c == ':') {
			c = '_';
		}
	}

	std::string pwe0 = ms.int_str(globalCfg->Pwei0);
	std::string pwe1 = ms.int_str(globalCfg->Pwei1);
	std::string minKmax = ms.int_str(globalCfg->minKmax);
	std::string maxKmax = ms.int_str(globalCfg->maxKmax);

	std::ofstream rgbData;
	std::string wrPath = globalCfg->outputPath + "_" + path + ".csv";

	bool isGood = false;{
		std::ifstream f(wrPath.c_str());
		isGood = f.good();
	}

	rgbData.open(wrPath, std::ios::app | std::ios::out);

	if (!rgbData) {
		INFO("output file open errno");
		return false;
	}
	if (!isGood) {
		rgbData << "ins,isopt,lyhrl,lyhrn,time,gap,lkhrn,lkhrl,d15rn,d15RL,sinrn,sinrl,narn,narl,rts,seed" << std::endl;
	}

	auto& sol = bks->bestSolFound;

	rgbData << input.example << ",";
	rgbData << globalCfg->cmdIsopt << ",";

	auto lyhrl = sol.verify();
	rgbData << lyhrl << ",";

	rgbData << sol.rts.size() << ",";

	//TODO[-1]:timer
	rgbData << gloalTimer.getRunTime() << ",";

	rgbData << double((double)(lyhrl - globalCfg->lkhRL) / globalCfg->lkhRL) * 100 << ",";

	rgbData << globalCfg->lkhRN << ",";
	rgbData << globalCfg->lkhRL << ",";

	rgbData << globalCfg->d15RecRN << ",";
	rgbData << globalCfg->d15RecRL << ",";

	rgbData << globalCfg->sintefRecRN << ",";
	rgbData << globalCfg->sintefRecRL << ",";

	rgbData << globalCfg->naRecRN << ",";
	rgbData << globalCfg->naRecRL << ",";

	for (std::size_t i = 0; i < sol.rts.cnt; ++i) {
		rgbData << "Route  " << i + 1 << " : ";
		Route& r = sol.rts[i];
		auto cusArr = sol.rPutCusInve(r);
		for (int c: cusArr) {
			rgbData << c << " ";
		}
		rgbData << "| ";
	}

	rgbData << ",";
	rgbData << globalCfg->seed;

	rgbData << std::endl;
	rgbData.close();

	return true;
}

int Goal::callSimulatedannealing() {

	int ourTarget = globalCfg->lkhRN;

	Solver st;

	st.initSolution(4);
	st.adjustRN(ourTarget);
	//s0.minimizeRN(0);
	st.mRLLocalSearch(0, {});
	st.Simulatedannealing(0, 1000);

	#if DIMACSGO
	while (true) {
	#else
	while (!gloalTimer.isTimeOut()) {
		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}
	#endif // DIMACSGO

		if (squIter * 10 > IntInf) {
			squIter = 1;
			for (auto& i : (*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}
		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}

		st.Simulatedannealing(0, 1000);
		bks->updateBKS(st);
	}

	saveSlnFile();
	return true;
}

bool Goal::test() {

	saveSlnFile();
	return true;
}

int Goal::TwoAlgCombine() {

	initPopulation();

	int contiNotDown = 1;
	//TODO[0]:先使用哪个
	int whichAlg = 1; // 0代表局部搜搜 1 代表交叉
	int popSize = globalCfg->popSize;

	int iter = 1;
	#if DIMACSGO

	while (true) {

	#else
	while (!gloalTimer.isTimeOut()) {
		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}
	#endif // DIMACSGO

		++iter;
		//INFO("squIter:", squIter);
		if (squIter * 10 > IntInf) {
			squIter = 1;
			for (auto& i : (*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}

		int upState = 0;
		if (whichAlg == 0) {

			Timer t(100);

			auto bestClone = bks->bestSolFound;
			bestClone.Simulatedannealing(0, 100);

			if (bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime()) + " ruin ls")) {
				upState = 1;
			}
			auto difcus = EAX::getDiffCusofPb(bestClone, bks->bestSolFound);
			if (difcus.size()>0) {
				bestClone.mRLLocalSearch(1, difcus);
			}
			bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime()) + " bks ls after ruin");

			for (auto& sol : pool) {
				auto clone = sol;
				sol.Simulatedannealing(0, 100);
				int cynum = EAX::getabCyNum(sol, clone);
				auto difcus = EAX::getDiffCusofPb(sol, clone);
				if (bks->updateBKS(sol, "time:" + std::to_string(gloalTimer.getRunTime()) + " ruin ls")) {
					upState = 1;
				}
				if (difcus.size() > 0) {
					sol.mRLLocalSearch(1,difcus);
					bks->updateBKS(sol, "time:" + std::to_string(gloalTimer.getRunTime()) + " ls after ruin");
				}
			}

			INFO("mul huo run", t.getRunTime());
			INFO("mul huo run", t.getRunTime());

		}
		else {

			Timer t(100);
			auto popOr = myRandX->getMN(popSize, popSize);
			myRand->shuffleVec(popOr);

			Vec< Vec<int> > ords;
			for (int i = 0; i < popSize; ++i) {
				int paIndex = popOr[i];
				int pbIndex = popOr[(i + 1) % popSize];
				ords.push_back({ paIndex ,pbIndex });
			}
			// 0, 1, 2, 3, 4
			std::swap(popOr[0], popOr[1]);
			std::swap(popOr[3], popOr[4]);

			for (int i = 0; i < popSize; ++i) {
				int paIndex = popOr[i];
				int pbIndex = popOr[(i + 1) % popSize];
				ords.push_back({ paIndex ,pbIndex });
			}

			for (auto& apair : ords) {
				int paIndex = apair[0];
				int pbIndex = apair[1];
				Solver& pa = pool[paIndex];
				Solver& pb = pool[pbIndex];
				int abcyNum = EAX::getabCyNum(pa, pb);
				//INFO("papb abcyNum:", abcyNum);
				if (abcyNum <= 1) {
					INFO("papb abcyNum < 1 abcyNum:", abcyNum);
					continue;
					//pa.Simulatedannealing(0, 100);
					//abcyNum = EAX::getabCyNum(pa, pb);
				}

				int up1 = naMA(pa, pb);
				//int up2 = naMA(pb, pa);
				if (up1 == 1) {
					upState = 1;
				}
			}
			INFO("eax run", t.getRunTime());
			INFO("eax run", t.getRunTime());

		}

		//whichAlg %= 2;
		//contiNotDown = 1;

		if (upState == 1) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}
		if (contiNotDown > 5) {
			//++whichAlg;
			//whichAlg %= 2;
			contiNotDown = 1;
		}

	}

	saveSlnFile();
	gloalTimer.disp();

	return true;
}

}
