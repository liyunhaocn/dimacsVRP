
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

DisType Goal::getMinRtCostInPool(int rn) {
	DisType bestSolInPool = DisInf;
	auto& pool = ppool[rn];
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
		
		if (myRand->pick(2)==0) {
			int kind = myRand->pick(4);
			int clearEPkind = myRand->pick(6);
			int ruinCusNum = std::min<int>(globalInput->custCnt/2, globalCfg->ruinC_);
			sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
		}
		else{
			int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt * 0.4);
			sclone.patternAdjustment(step);
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

int Goal::naMA(int rn) { // 1 代表更新了最优解 0表示没有

	auto& pool = ppool[rn];

	int popSize = globalCfg->popSize;
	auto& ords = myRandX->getMN(popSize, popSize);
	
	DisType bestSolInPool = getMinRtCostInPool(curSearchRN);

	//TODO[-1]:naMA这里改10了
	for (int ct = 0; ct < 10; ct++) {
		myRand->shuffleVec(ords);
		for (int i = 0; i < popSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % popSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			doTwoKindEAX(pa, pb, 0);
		}
		
		DisType curBest = getMinRtCostInPool(curSearchRN);
		if (curBest < bestSolInPool ) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

	//TODO[-1]:naMA这里改10了
	bestSolInPool = getMinRtCostInPool(curSearchRN);
	for (int ct = 0; ct < 10; ct++) {
		myRand->shuffleVec(ords);
		for (int i = 0; i < popSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % popSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			doTwoKindEAX(pa, pb, 1);
		}

		DisType curBest = getMinRtCostInPool(curSearchRN);
		if (curBest < bestSolInPool) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

	for (int i = 0; i < popSize; ++i) {
		if (bks->bestSolFound.rts.cnt == pool[i].rts.cnt) {
			auto pa = bks->bestSolFound;
			auto pb = pool[i];
			doTwoKindEAX(pa, pb, 0);
			doTwoKindEAX(pa, pb, 1);
		}
	}

	INFO("curSearchRN:",curSearchRN,"getMinRtCostInPool():", getMinRtCostInPool(curSearchRN));
	return 0;
}

int Goal::gotoRNPop(int rn) {

	//TODO[-1]:注释掉了
	if (ppool[rn].size() == 0) {
		fillPopulation(rn);
	}
	
	//TODO[0]:Lmax和ruinLmax的定义
	globalCfg->ruinLmax = globalInput->custCnt / rn;
	globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
	//globalCfg->ruinC_ = 15;

	auto& pool = ppool[rn];
	int popSize = globalCfg->popSize;

	Vec<int> popIsAlreadrn;

	for (int i = 0; i < popSize; ++i) {
		if (pool[i].rts.cnt == rn) {
			popIsAlreadrn.push_back(i);
		}
	}
	if (popIsAlreadrn.size() == popSize) {
		return rn;
	}

	if (rn == poprnLowBound) { //r如果是
		INFO("rn == poprnLowBound,no this sit");
		return -1;
	}

	for (int pIndex = 0; pIndex < popSize; ++pIndex) {

		auto& sol = pool[pIndex];
		if (sol.rts.cnt != rn) {

			bool isAdj = false;

			//TODO[-1]:从刚才搜索的位置跳

			int downRn = -1;
			int sameNum = 0;
			for (int i = rn - 1; i >= poprnLowBound; --i) {

				if (ppool[i][pIndex].rts.cnt > 0 ) {
					if (myRand->pick(++sameNum) == 0) {
						downRn = i;
					}
				}
			}

			lyhCheckTrue(downRn >= poprnLowBound);
			sol = ppool[downRn][pIndex];
			isAdj = sol.adjustRN(rn);

			lyhCheckTrue(isAdj == true);
			#if CHECKING
			lyhCheckTrue(isAdj==true);
			#endif // CHECKING
			bks->updateBKSAndPrint(sol, "adjust from cur + ls");
		}
	}
	return rn;
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
	bks->updateBKSAndPrint(st, "Simulatedannealing(0,1000, 20.0, 1)");

	st.Simulatedannealing(1, 1000, 20.0, globalCfg->ruinC_);
	bks->updateBKSAndPrint(st,"Simulatedannealing(1, 1000, 20.0, globalCfg->ruinC_)");

	//saveSlnFile();
	return true;
}

bool Goal::test() {

	Vec<Solver> poolt(1);

	poolt[0].initSolution(0);
	poolt[0].mRLLocalSearch(0, {});
	while (true)
	{
		globalCfg->ruinLmax = globalInput->custCnt / poolt[0].rts.cnt;
		globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
		poolt[0].Simulatedannealing(1, 30, 100.0, globalCfg->ruinC_);
		poolt[0].patternAdjustment(50);
		poolt[0].mRLLocalSearch(0, {});
		bks->updateBKSAndPrint(poolt[0], " poolt[i] init");
	}



	return true;
}

void Goal::updateppol(Solver& sol, int index) {
	int tar = sol.rts.cnt;
	int popSize = globalCfg->popSize;

	if (ppool[tar].size() == 0) {
		ppool[tar].resize(popSize);
	}
	if (sol.RoutesCost < ppool[tar][index].RoutesCost) {
		INFO("update ppool rn:", tar, "index:", index);
		ppool[tar][index] = sol;
	}
};

Vec<int> Goal::getTheRangeMostHope() {

	int popSize = globalCfg->popSize;

	Vec<Solver> poolt(popSize);

	for (int i = 0; i < popSize; ++i) {
		poolt[i].initSolution(i);
		poolt[i].mRLLocalSearch(0, {});
		globalCfg->ruinLmax = globalInput->custCnt / poolt[i].rts.cnt;
		globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
		poolt[i].Simulatedannealing(1, 1000, 100.0, globalCfg->ruinC_);
		updateppol(poolt[i],i);
		bks->updateBKSAndPrint(poolt[i], " poolt[i] init");
	}
	
	Vec <Vec<Solver>> soles(popSize);
	for (int peopleIndex = 0; peopleIndex < popSize; ++peopleIndex) {
		auto& sol = poolt[peopleIndex];
		//sol.Simulatedannealing(0, 100, 50.0, globalCfg->ruinC_);
		if (sol.rts.cnt < 2) {
			sol.adjustRN(5);
		}

		while (sol.rts.cnt > 2) {

			soles[peopleIndex].push_back(sol);
			bool isDel = sol.minimizeRN(sol.rts.cnt - 1);
			if (!isDel) {
				break;
			}
			bks->updateBKSAndPrint(sol, " poolt[0] mRLS(0, {})");
		}
	}

	poprnUpBound = IntInf;
	poprnLowBound = IntInf;
	for (int i = 0; i < popSize; ++i) {
		poprnUpBound = std::min<int>(poprnUpBound, soles[i].front().rts.cnt);
		poprnLowBound = std::min<int>(poprnLowBound, soles[i].back().rts.cnt);
	}

	poprnUpBound = std::min<int>(poprnLowBound + 20, globalInput->custCnt);

	poprnUpBound = std::max<int>(poprnUpBound,bks->bestSolFound.rts.cnt);
	poprnLowBound = std::min<int>(poprnLowBound,bks->bestSolFound.rts.cnt);

	//TODO[-1]:这个很重要 考虑了vehicleCnt！！！
	poprnUpBound = std::min<int>(poprnUpBound, globalInput->vehicleCnt);

	INFO("poprnLowBound:",poprnLowBound,"poprnUpBound:", poprnUpBound);

	if (poprnLowBound > globalInput->vehicleCnt) {
		ERROR("!!!!!this alg finshed! even cant get a sol sat vehicleCnt");
	}
	INFO("soles.size():", soles.size());

	for (int rn = poprnLowBound; rn <= poprnUpBound; ++rn) {
		fillPopulation(rn);
	}

	// 所有解
	for (int popIndex = 0;popIndex<popSize;++popIndex) {
		auto& deorsoles = soles[popIndex];
		for (auto& sol : deorsoles) {
			if (sol.rts.cnt >= poprnLowBound
				&& sol.rts.cnt <= poprnUpBound) {
				ppool[sol.rts.cnt][popIndex] = sol;
			}
		}
	}

	std::queue<int> alreadyBound;

	for (int i = 0; i < popSize; ++i) {
		if (ppool[poprnLowBound][i].rts.cnt == poprnLowBound) {
			alreadyBound.push(i);
		}
	}
	if (bks->bestSolFound.rts.cnt == poprnLowBound) {
		alreadyBound.push(popSize);
	}

	INFO("alreadyBound.size():", alreadyBound.size());
	lyhCheckTrue(alreadyBound.size() > 0);
	
	for (int i = 0; i < popSize; ++i) {
		auto& sol = ppool[poprnLowBound][i];
		if (sol.rts.cnt != poprnLowBound) {
			int index = alreadyBound.front();
			alreadyBound.pop();
			alreadyBound.push(index);
			sol = index == popSize ? bks->bestSolFound : ppool[poprnLowBound][index];
			sol.patternAdjustment();
		}
	}

	Vec <int> rnOrderqu;
	UnorderedSet<int> rnSet;

	for (int i = poprnLowBound + 1 ; i < std::min<int>(poprnUpBound, poprnLowBound + 5); ++i) {
		curSearchRN = gotoRNPop(i);
		auto& pool = ppool[curSearchRN];
		for (int i = 0; i < popSize; ++i) {
			globalCfg->ruinLmax = globalInput->custCnt / pool[i].rts.cnt;
			globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
			pool[i].Simulatedannealing(0, 100, 100.0, globalCfg->ruinC_);
			updateppol(pool[i], i);
			bks->updateBKSAndPrint(pool[i], " pool[i] init");
		}
	}

	#if 1
	for (int i = poprnUpBound; i >= poprnLowBound; --i) {
		if (rnSet.count(i) == 0) {
			rnOrderqu.push_back(i);
			rnSet.insert(i);
		}
	}
	#else
	for (int i = poprnLowBound; i <= poprnLowBound + 3; ++i) {
		if (rnSet.count(i) == 0) {
			rnOrderqu.push(i);
			rnSet.insert(i);
		}
	}
	#endif // 0
	//if (poprnLowBound <= globalCfg->lkhRN) {
	//	for (int i = globalCfg->lkhRN + 3;
	//		i > std::max<int>(globalCfg->lkhRN - 1, poprnLowBound); --i) {
	//		if (rnSet.count(i) == 0) {
	//			rnOrderqu.push_back(i);
	//			rnSet.insert(i);
	//		}
	//	}
	//}

	return rnOrderqu;
}

int Goal::TwoAlgCombine() {

	int popSize = globalCfg->popSize;

	getTheRangeMostHope();
	Vec<int> rnOrderqu;

	auto getNextRnSolGO = [&]() -> int {

		int bksRN = bks->bestSolFound.rts.cnt;
		if (bksRN == poprnUpBound) {
			rnOrderqu = { bksRN - 2 ,bksRN - 1 };
		}
		else if (bksRN == poprnLowBound) {
			rnOrderqu = { bksRN + 2 ,bksRN + 1 };
		}
		else {
			rnOrderqu = { bksRN + 1 ,bksRN - 1 };
		}
		rnOrderqu.push_back(myRand->pick(poprnLowBound,poprnUpBound+1));
		myRand->shuffleVec(rnOrderqu);

		return rnOrderqu[myRand->pick(rnOrderqu.size())];
	};

	DisType bksLastLoop = bks->bestSolFound.RoutesCost;
	int contiNotDown = 1;

	curSearchRN = bks->bestSolFound.rts.cnt;
	curSearchRN = gotoRNPop(curSearchRN);

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
		
		if (gloalTimer->getRunTime() + 20 > globalCfg->runTimer) {
			saveBKStoCsvFile();
		}

		if (bks->bksAtRn[curSearchRN] <= bks->bestSolFound.RoutesCost) {
		//if (bks->bksAtRn[curSearchRN] < bksLastLoop) {
			if (bksLastLoop != bks->bksAtRn[curSearchRN]) {
				contiNotDown = 1;
			}
			else {
				++contiNotDown;
			}
		}
		else {
			++contiNotDown;
		}

		bksLastLoop = bks->bksAtRn[curSearchRN];

		int plangoto = -1;

		INFO("twoAlg contiNotDown:", contiNotDown,"curSearchRN:",curSearchRN);

		if (contiNotDown % 3 == 0) {

			int bksRN = bks->bestSolFound.rts.cnt;
			if (curSearchRN != bksRN && bksRN >= poprnLowBound) {
				plangoto = bks->bestSolFound.rts.cnt;
				
			}
			else {
				plangoto = getNextRnSolGO();
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
		naMA(curSearchRN);
		Solver& sol = bks->bestSolFound;
		//Solver clone = sol;
		//clone.Simulatedannealing(0, 40, 1.0, globalCfg->ruinC_);
		//bks->updateBKSAndPrint(clone, " bks ruin simulate 0");
		//updateppol(sol, 0);

		Solver clone = sol;
		clone.Simulatedannealing(1, 100, 20.0, globalCfg->ruinC_);		
		bks->updateBKSAndPrint(clone, " bks ruin simulate 1");
		//updateppol(sol, 0);

		int ruinUppoolNum = 0;
		for (int i = 0; i < popSize; ++i) {
			Solver& sol = pool[i];
			Solver clone = sol;
			clone.Simulatedannealing(0, 50, 20.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 0");
			updateppol(sol, i);

			clone = sol;
			clone.Simulatedannealing(1, 30, 20.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 1");
			if (clone.rts.cnt == sol.rts.cnt && clone.RoutesCost < sol.RoutesCost) {
				++ruinUppoolNum;
			}
			//updateppol(sol, i);
			
		}

		if (ruinUppoolNum >= 4) {
			naMA(curSearchRN);
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
