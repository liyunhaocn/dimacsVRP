
#include "Goal.h"
#include "EAX.h"

namespace hust {

Goal::Goal():gloalTimer(0){

	eaxYearTable = Vec<Vec<LL>>
		(globalCfg->popSize, Vec<LL>(globalCfg->popSize));

}

#if 0
Vec<int> Goal::getpairOfPaPb() {

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

int Goal::naMA() { // 1 代表更新了最优解 0表示没有

	int popSize = globalCfg->popSize;
	Vec<int> papborder(popSize, 0);
	std::iota(papborder.begin(), papborder.end(), 0);

	myRand->shuffleVec(papborder);

	int updateState = 0;

	//for (int orderIndex = 0; orderIndex < popSize; ++orderIndex) {
	for (int orderIndex = 0; orderIndex < popSize; ++orderIndex) {
		int paIndex = papborder[orderIndex];
		int pbIndex = papborder[(orderIndex + 1) % popSize];
		Solver& pa = pool[paIndex];
		Solver& pb = pool[pbIndex];
		int before = EAX::getabCyNum(pa, pb);
		if (before <= 1) {
			perturbThePop();
			int after = EAX::getabCyNum(pa, pb);
			//INFO("before:", before, "after:", after, "abCyNum <= 1");
		}
		int isabState = doTwoKindEAX(pa, pb, 0);
		if (isabState == 1) {
			updateState = 1;
		}
	}
	if (updateState == 1) {
		return updateState;
	}
	for (int orderIndex = 0; orderIndex < popSize; ++orderIndex) {
		int paIndex = papborder[orderIndex];
		int pbIndex = papborder[(orderIndex + 1) % popSize];
		Solver& pa = pool[paIndex];
		Solver& pb = pool[pbIndex];
		int before = EAX::getabCyNum(pa, pb);
		if (before <= 1) {
			perturbThePop();
			int after = EAX::getabCyNum(pa, pb);
			//INFO("before:", before, "after:", after, "abCyNum <= 1");
		}
		int isabState = doTwoKindEAX(pa, pb, 1);
		if (isabState == 1) {
			updateState = 1;
		}
	}
	return updateState;
}

bool Goal::initPopulation() {

	//for (int initKind = 0; initKind < 5; ++initKind) {
	//
	//	Solver st;
	//	st.initSolution(initKind);
	//	INFO("initKind:", initKind);
	//	char a[20];
	//	snprintf(a, 20, "k%d", initKind);
	//	st.saveOutAsSintefFile(a);
	//}

	pool.reserve(globalCfg->popSize);

	//Vec<int> kset = { 1,4 };
	Vec<int> kset = { 0,1,2,3,4 };
	myRand->shuffleVec(kset);

	int ourTarget = -1;

	#if DIMACSGO
	ourTarget = globalCfg->lkhRN;
	#else
	ourTarget = globalInput->sintefRecRN;
	#endif // DIMACSGO

	// TODO[0]:使用了 ourTarget += 1;
	//if (globalCfg->cmdIsopt != 1) {
	//	ourTarget += 1;
	//}
	//for (int initKind = 0; initKind < kset.size(); ++initKind) {
	//	std::string name = "init" + std::to_string(initKind);
	//	Solver st;
	//	st.initSolution(initKind);
	//	st.adjustRN(ourTarget);
	//	st.saveOutAsSintefFile(name);
	//}

	Solver s0;
	s0.initSolution(kset[0]);
	s0.adjustRN(ourTarget);
	//s0.minimizeRN(0);

	INFO("s0.RoutesCost:",s0.RoutesCost);
	ourTarget = s0.rts.cnt;
	s0.mRLLocalSearch(0, {});

	INFO("s0.RoutesCost:", s0.RoutesCost);

	INFO("sol0 inint kind:", kset[0]);

	s0.ruinLocalSearch(globalCfg->ruinC_);


	INFO("s0.RoutesCost:",s0.RoutesCost);

	bks->updateBKS(s0);
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
			st.ruinLocalSearch(globalCfg->ruinC_);
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
			sclone.ruinLocalSearch(globalCfg->ruinC_);
			bks->updateBKS(sclone);
			pool.push_back(sclone);
		}
	}
	return true;
}

bool Goal::CVB2LocalSearch(){

	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

	initPopulation();
	//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
	auto pBest = pool[0];

	int contiNotDown = 1;

	int pIndex = 0;

	//ProbControl pc(2*globalCfg->ruinC_);
	#if DIMACSGO
	while (!gloalTimer.isTimeOut()) {
		#else
	while (bks->bestSolFound.RoutesCost > sintefRecRL && !gloalTimer.isTimeOut()) {
		#endif // DIMACSGO

		auto st = pBest;
		//INFO("contiNotDown:", contiNotDown);
		//if (contiNotDown > 50) {
		//	++pIndex;
		//	if (pIndex >= pool.size()) {
		//		INFO("perturbThePop");
		//		perturbThePop();
		//		pIndex = 0;
		//	}
		//	contiNotDown = 1;
		//}

		if (squIter * 10 > IntInf) {
			squIter = 1;
			for (auto& i : (*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}

		auto beforeLS = st.RoutesCost;

		st.CVB2ruinLS(2*globalCfg->ruinC_);

		if (st.RoutesCost < beforeLS) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}

		bks->updateBKS(st);

		if (st.RoutesCost < pBest.RoutesCost) {
			INFO("pBest.rts.cnt:", pBest.rts.cnt);
			pBest = st;
			contiNotDown = 1;
		}
	}
	saveSlnFile();
	return true;


	return false;
}

bool Goal::TwoAlgCombine() {

	initPopulation();
	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

	int contiNotDown = 1;
	//TODO[0]:先使用哪个
	int whichAlg = 0; // 0代表局部搜搜 1 代表交叉
	int popSize = globalCfg->popSize;


	#if DIMACSGO
	while (!gloalTimer.isTimeOut()) {
	#else
	while (bks->bestSolFound.RoutesCost > sintefRecRL && !gloalTimer.isTimeOut()) {
	#endif // DIMACSGO

		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg-> d15RecRL) {
				break;
			}
		}

		//INFO("squIter:", squIter);
		if (squIter *10 > IntInf) {
			squIter = 1;
			for (auto& i:(*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}

		int upState = 0;

		if (whichAlg == 0) {

			auto bestClone = bks->bestSolFound;
			bestClone.ruinLocalSearch(globalCfg->ruinC_);
			//bestClone.ruinLocalSearch(1);
			
			if (bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime())+" ruin ls")) {
				upState = 1;
			}
			for (auto& better : pool) {
				better.ruinLocalSearch(globalCfg->ruinC_);
				//better.ruinLocalSearch(1);
				if (bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime()) + " ruin ls")) {
					upState = 1;
				}
			}
		}
		else {

			upState = naMA();
		}

		if (upState == 1) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}

		if(contiNotDown % 2 == 0) {
			++whichAlg;
			whichAlg %= 2;
		}

		if (contiNotDown == 50) {
			perturbThePop();
			contiNotDown = 1;
		}
	}

	saveSlnFile();
	gloalTimer.disp();

	return true;
}

bool Goal::justLocalSearch() {

	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

	initPopulation();
	//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
	auto pBest = pool[0];

	int contiNotDown = 1;

	int pIndex = 0;

	//ProbControl pc(2*globalCfg->ruinC_);
	#if DIMACSGO
	while (!gloalTimer.isTimeOut()) {
	#else
	while (bks->bestSolFound.RoutesCost > sintefRecRL && !gloalTimer.isTimeOut()) {
	#endif // DIMACSGO

		auto& st = pool[pIndex];
		//INFO("contiNotDown:", contiNotDown);
		if (contiNotDown > 50) {
			++pIndex;
 			if (pIndex >= pool.size()) {
				INFO("perturbThePop");
				perturbThePop(); 
				pIndex = 0;
			}
			contiNotDown = 1;
		}

		if (squIter * 10 > IntInf) {
			squIter = 1;
			for (auto& i : (*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}

		//st.ruinLocalSearch(c_ + 1);
		//st.ruinLocalSearch(globalCfg->ruinC_);
		auto beforeLS = st.RoutesCost;

		st.ruinLocalSearch(globalCfg->ruinC_);

		if (st.RoutesCost < beforeLS) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}

		bks->updateBKS(st);

		if (st.RoutesCost < pBest.RoutesCost) {
			//++pc.data[c_];
			pBest = st;
			contiNotDown = 1;
		}
	}
	saveSlnFile();
	return true;
}

bool Goal::saveSlnFile() {

	Input& input = *globalInput;

	MyString ms;
	// 输出 tm 结构的各个组成部分
	//std::string day = /*ms.LL_str(d.year) + */ms.LL_str(d.month) + ms.LL_str(d.day);

	#if DIMACSGO
	std::string type = "[dim]";
	#else
	std::string type = "[sin]";
	#endif // DIMACSGO

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

bool Goal::test() {

	saveSlnFile();
	return true;
}

}
