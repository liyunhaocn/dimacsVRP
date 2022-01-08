
#include "EAX.h"
#include "Goal.h"

namespace hust {

Goal::Goal():gloalTimer(0){

	eaxTabuTable = Vec<Vec<bool>>
		(globalCfg->popSize, Vec<bool>(globalCfg->popSize,false));

	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

}

Vec<int>Goal::getNotTabuPaPb() {
	int popSize = globalCfg->popSize;
	int cnt = 0;
	Vec<int> ret = {-1,-1};
	for (int i = 0; i < popSize; ++i) {
		for (int j = 0; j < popSize; ++j) {
			if (i != j && !eaxTabuTable[i][j]) {
				if (myRand->pick(++cnt) == 0) {
					ret = { i,j };
				}
			}
		}
	}
	return ret;
}

Vec<int> Goal::getpairOfPaPb() {

	int popSize = globalCfg->popSize;
	auto popOr = myRandX->getMN(globalCfg->popSize, popSize);
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
	}
	return {};
}

DisType Goal::getMinRtCostInPool() {
	DisType bestSolInPool = DisInf;
	for (int i = 0; i < pool.size(); ++i) {
		bestSolInPool = std::min<DisType>(bestSolInPool, pool[i].RoutesCost);
	}
	return bestSolInPool;
}

DisType Goal::doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

	int retState = 0; // 0 表示没有成功更新最优解，1表示更新了最优解 -1表示这两个解无法进行eax

	EAX eax(pa, pb);
	eax.generateCycles();

	if (eax.abCycleSet.size() <= 1) {
		return -1;
	}
	if (kind == 1 && eax.abCycleSet.size() < 4) {
		return -1;
	}

	Solver paBest = pa;

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
			//break;
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
	return paBest.RoutesCost;
}

bool Goal::perturbOnePop(int i) {

	auto& sol = pool[i];
	//auto before = sol.RoutesCost;
	Solver sclone = sol;
	
	int isPerOnePerson = 0;
	for (int i = 0; i < 10; ++i) {

		//尝试使用 100度的模拟退火进行扰动
		//sclone.Simulatedannealing(0,100,1000.0,2*globalCfg->ruinC_);
		int kind = myRand->pick(4);
		if (kind <= 2) {
			int clearEPkind = myRand->pick(6);
			int ruinCusNum = std::min<int>(globalInput->custCnt/2, globalCfg->ruinC_);
			sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
		}
		else if(kind ==3){
			int step = myRand->pick(sclone.input.custCnt * 0.4, sclone.input.custCnt*0.6);
			sclone.patternAdjustment(step);
			sclone.reCalRtsCostAndPen();
		}
		//else if (kind == 4) {
		//	int step = myRand->pick(sclone.input.custCnt * 0.1, sclone.input.custCnt * 0.2);
		//	sclone.perturbBasedejepool(step);
		//}

		//int step = myRand->pick(sclone.input.custCnt * 0.1, sclone.input.custCnt * 0.2);
		//sclone.patternAdjustment(step);
		//int clearEPkind = myRand->pick(6);
		//int ruinCusNum = std::min<int>(globalInput->custCnt/2, 2 * globalCfg->ruinC_);
		//sclone.perturbBaseRuin(1, ruinCusNum, clearEPkind);
		//int ejNum = myRand->pick(sclone.input.custCnt * 0.1, sclone.input.custCnt*0.2);

		auto diff = EAX::getDiffCusofPb(sol, sclone);
		//INFO("i:",i,"diff.size:",diff.size());
		if (diff.size() > sol.input.custCnt * 0.2) {
			sclone.mRLLocalSearch(1, diff);
			sol = sclone;
			isPerOnePerson = 1;
			break;
		}
		else {
			sclone = sol;
		}
	}
	return true;
}

int Goal::naMA() { // 1 代表更新了最优解 0表示没有

	int popSize = globalCfg->popSize;
	auto& ords = myRandX->getMN(popSize, popSize);
	
	DisType bestSolInPool = getMinRtCostInPool();

	for (int ct = 0; ct < 10; ct++) {
		myRand->shuffleVec(ords);
		for (int i = 0; i < popSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % popSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			doTwoKindEAX(pa, pb, 0);
		}
		
		DisType curBest = getMinRtCostInPool();
		if (curBest < bestSolInPool ) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

	bestSolInPool = getMinRtCostInPool();
	for (int ct = 0; ct < 10; ct++) {
		myRand->shuffleVec(ords);
		for (int i = 0; i < popSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % popSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			doTwoKindEAX(pa, pb, 1);
		}

		DisType curBest = getMinRtCostInPool();
		if (curBest < bestSolInPool) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}
	INFO("getMinRtCostInPool():", getMinRtCostInPool());
	return 0;
}

bool Goal::initPopulation() {

	pool.reserve(globalCfg->popSize);
	Vec<int> kset = { 4,3,2,1,0};


	int ourTarget = globalCfg->lkhRN;
	//int ourTarget = globalCfg->lkhRN;

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
			s0.ruinLocalSearchNotNewR(1);
			bks->updateBKS(st);
			INFO("sol inint kind:", kset[i % kset.size()]);
			pool.push_back(st);
		}
	}

	i = 0;
	if (pool.size() < globalCfg->popSize) {
		INFO("dont get enough population");
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

	for (int i = 0; i < sol.rts.cnt; ++i) {
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

	st.mRLLocalSearch(0, {});
	st.Simulatedannealing(0,1000, 20.0, 1);

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

		globalRepairSquIter();

		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}

		st.Simulatedannealing(1, 10000, 100.0, globalCfg->ruinC_);
		bks->updateBKS(st);
	}

	saveSlnFile();
	return true;
}

bool Goal::test() {

	return true;
}

int Goal::TwoAlgCombine() {

	initPopulation();
	int popSize = globalCfg->popSize;

	DisType naMAHis = 0;
	DisType ruinHis = 0;

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

		globalRepairSquIter();

		naMA();

		Solver& sol = bks->bestSolFound;
		Solver clone = sol;
		clone.Simulatedannealing(0, 50, 0.0, 15);
		bks->updateBKS(clone, "time:" + std::to_string(gloalTimer.getRunTime()) + " ls after ruin");
		for (int i = 0; i < popSize; ++i) {
			Solver& sol = i < popSize ? pool[i] : bks->bestSolFound;
			Solver clone = sol;
			clone.Simulatedannealing(0, 20, 0.0, 15);
			//clone.Simulatedannealing(0, 20, 1.0, globalCfg->ruinC_);
			
			bks->updateBKS(clone, "time:" + std::to_string(gloalTimer.getRunTime()) + " ls after ruin");
			if (clone.RoutesCost < sol.RoutesCost) {
				sol = clone;
			}
		}

		INFO("perturbAllPop");
		for (int i = 0; i < popSize; ++i) {
			perturbOnePop(i);
		}
	}

	saveSlnFile();
	gloalTimer.disp();

	return true;
}

}
