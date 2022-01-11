
#include "EAX.h"
#include "Goal.h"

namespace hust {

Goal::Goal(){

	eaxTabuTable = Vec<Vec<bool>>
		(globalCfg->popSize, Vec<bool>(globalCfg->popSize,false));

}

#if 0
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
#endif // 0

DisType Goal::getMinRtCostInPool() {
	DisType bestSolInPool = DisInf;
	auto& pool = ppool[curSearchRN];
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
			//pc.mRLLocalSearch(0, {});
			pc.mRLLocalSearch(1, newCus);
			//auto cus1 = EAX::getDiffCusofPb(pa, pc);
			//if (cus1.size() == 0) {
			//	//debug("pa is same as pa");
			//}
		}
		else {
			//debug("pa is same as pa");
		}

		if (pc.RoutesCost < paBest.RoutesCost) {
			paBest = pc;
			
			bool isup = bks->updateBKSAndPrint(pc," eax ls, kind:" + std::to_string(kind));
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

	auto& pool = ppool[curSearchRN];
	auto& sol = pool[i];
	//auto before = sol.RoutesCost;
	Solver sclone = sol;
	
	bool isPerOnePerson = false;
	for (int i = 0; i < 10; ++i) {

		//尝试使用 100度的模拟退火进行扰动
		//sclone.Simulatedannealing(0,100,100.0,globalCfg->ruinC_);
		
		int kind = myRand->pick(4);
		if (kind <= 2) {
			int clearEPkind = myRand->pick(6);
			int ruinCusNum = std::min<int>(globalInput->custCnt/2, globalCfg->ruinC_);
			sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
		}
		else if(kind ==3){
			int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt*0.4);
			sclone.patternAdjustment(step);
			sclone.reCalRtsCostAndPen();
		}
		 
		//else if (kind == 4) {
			//int step = myRand->pick(sclone.input.custCnt * 0.1, sclone.input.custCnt * 0.2);
			//int step = sclone.input.custCnt * 0.1;
			//sclone.perturbBasedejepool(step);

			//sclone.perturbBasedejepool(2 * globalCfg->ruinC_);
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
			isPerOnePerson = true;
			break;
		}
		else {
			sclone = sol;
		}
	}
	return isPerOnePerson;
}

int Goal::naMA() { // 1 代表更新了最优解 0表示没有

	auto& pool = ppool[curSearchRN];

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

int Goal::gotoRNPop(int rn) {

	fillPopulation(rn);

	auto& pool = ppool[rn];
	int popSize = globalCfg->popSize;

	Vec<int> kset = { 4,3,2,1,0 };

	Vec<int> adSucceed;
	for (int i = 0; i < popSize; ++i) {
		if (pool[i].rts.cnt == 0) {
			pool[i].initSolution(kset[i]);
			bool isAdj = pool[i].adjustRN(rn);
			if (isAdj) {
				pool[i].mRLLocalSearch(0, {});
				bks->updateBKSAndPrint(pool[i]);
				adSucceed.push_back(i);
			}
		}
		else {
			adSucceed.push_back(i);
		}
	}

	if (adSucceed.size() == 0) {

		for (int i = 0; i < popSize; ++i) {
			pool[i] = ppool[poprnLowBound][i];
			pool[i].adjustRN(rn);
		}
		return rn;
	}
	else if (adSucceed.size() == popSize) {
		return rn;
	}
	else if (adSucceed.size() < popSize) {
		
		int usei = 0;

		INFO("dont get enough population");

		for (int i = 0; i < popSize; ++i) {

			if (pool[i].rts.cnt != rn) {
				pool[i] = pool[usei];

				pool[i].patternAdjustment(globalInput->custCnt);
				pool[i].reCalRtsCostAndPen();
				bks->updateBKSAndPrint(pool[i]);

				usei %= (usei + 1) % adSucceed.size();
			}
		}
		return rn;
	}
	return -1;

}

bool Goal::fillPopulation(int rn) {

	int popSize = globalCfg->popSize;
	auto& pool = ppool[rn];

	if (pool.size() == 0 ) {
		pool.resize(popSize);
	}

	return true;
}

int Goal::callSimulatedannealing() {

	//int ourTarget = globalCfg->lkhRN;
	int ourTarget = 0;

	Solver st;

	st.initSolution(0);
	st.adjustRN(ourTarget);

	st.mRLLocalSearch(0, {});
	st.Simulatedannealing(0,1000, 20.0, 1);

	st.Simulatedannealing(1, 1000, 20.0, globalCfg->ruinC_);
	bks->updateBKSAndPrint(st);

	//saveSlnFile();
	return true;
}

bool Goal::test() {

	return true;
}

int Goal::TwoAlgCombine() {

	int popSize = globalCfg->popSize;
	Solver srnbound;
	srnbound.initSolution(0);
	srnbound.mRLLocalSearch(0, {});
	bks->updateBKSAndPrint(srnbound,"srnbound mRLLocalSearch(0, {})");
	srnbound.minimizeRN(2);
	poprnLowBound = srnbound.rts.cnt;
	fillPopulation(poprnLowBound);

	ppool[poprnLowBound][0] = srnbound;

	curSearchRN = gotoRNPop(poprnLowBound);

	if (curSearchRN != poprnLowBound) {
		ERROR("srnbound rn cant reach");
	}

	auto updateppol = [&](Solver& sol,int index) {
		int tar = sol.rts.cnt;
		if (ppool[tar].size() == 0) {
			ppool[tar].resize(popSize);
		}
		if (sol.RoutesCost < ppool[tar][index].RoutesCost) {
			INFO("update pool rn:",tar,"index:",index);
			ppool[tar][index] = sol;
		}
	};
	
	DisType bksLastLoop = bks->bestSolFound.RoutesCost;
	int contiNotDown = 1;

	std::queue <int> rnOrderqu;

	//int poprnUpBound = -1;
	if (poprnLowBound <= globalCfg->lkhRN) {
		for (int i = globalCfg->lkhRN + 3; i > std::max<int>(globalCfg->lkhRN - 3, poprnLowBound); --i) {
			rnOrderqu.push(i);
		}
		//poprnUpBound = globalCfg->lkhRN + 3;
	}
	else {
		for (int i = poprnLowBound + 2; i >= poprnLowBound; --i) {
			rnOrderqu.push(i);
		}
		//poprnUpBound = poprnLowBound + 2;
	}
	
	curSearchRN = gotoRNPop(rnOrderqu.front());
	rnOrderqu.push(rnOrderqu.front());
	rnOrderqu.pop();
	
	#if DIMACSGO
	while (true) {
	#else
	while (!gloalTimer->isTimeOut()) {
		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}
	#endif // DIMACSGO
		
		if (bksLastLoop < bks->bestSolFound.RoutesCost) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}
		bksLastLoop = bks->bestSolFound.RoutesCost;

		int plangoto = -1;

		if (curSearchRN != bks->bestSolFound.rts.cnt && bks->bestSolFound.rts.cnt>=poprnLowBound) {
			plangoto = bks->bestSolFound.rts.cnt;
		}
		else {
			if (contiNotDown % 10 == 0) {
				plangoto = rnOrderqu.front();
				rnOrderqu.pop();
				rnOrderqu.push(plangoto);
			}
		}

		if (plangoto != -1) {
			curSearchRN = gotoRNPop(plangoto);
			if (curSearchRN == plangoto) {
				INFO("jump succeed curSearchRN", curSearchRN);
			}
			else {
				INFO("jump fail curSearchRN", curSearchRN, "plangoto:", plangoto);
			}
		}
		
		auto& pool = ppool[curSearchRN];
		globalRepairSquIter();
		naMA();

		for (int i = 0; i < popSize; ++i) {
			Solver& sol = i < popSize ? pool[i] : bks->bestSolFound;
			Solver clone = sol;
			clone.mRLLocalSearch(0, {});
			bks->updateBKSAndPrint(clone, " mRLLocalSearch(0, {})");
			if (clone.RoutesCost < sol.RoutesCost) {
				sol = clone;
			}
		}

		Solver& sol = bks->bestSolFound;
		Solver clone = sol;
		clone.Simulatedannealing(0, 100, 1.0, globalCfg->ruinC_);
		bks->updateBKSAndPrint(clone, " bks ruin simulate 0");

		clone.Simulatedannealing(1, 100, 1.0, globalCfg->ruinC_);		
		bks->updateBKSAndPrint(clone, " bks ruin simulate 1");
		updateppol(clone, 1);

		for (int i = 0; i < popSize; ++i) {
			Solver& sol = pool[i];
			Solver clone = sol;
			clone.Simulatedannealing(0, 50, 1.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 0");

			clone.Simulatedannealing(1, 50, 1.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 1");
			updateppol(sol, i);

			if (clone.RoutesCost < sol.RoutesCost) {
				if (clone.rts.cnt == sol.rts.cnt) {
					sol = clone;
				}
			}
		}

		INFO("perturbAllPop");
		for (int i = 0; i < popSize; ++i) {
			perturbOnePop(i);
		}
	}

	bks->bestSolFound.printDimacs();

	//TODO[-1]:如果提交求解器记得去掉
	saveBKStoCsvFile();

	gloalTimer->disp();

	return true;
}

}
