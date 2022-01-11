
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
		
		if (myRand->pick(2)==0) {
			int kind = myRand->pick(4);
			int clearEPkind = myRand->pick(6);
			int ruinCusNum = std::min<int>(globalInput->custCnt/2, globalCfg->ruinC_);
			sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
		}
		else{
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

		int alreadyI = 0;

		for (int i = 0; i < popSize; ++i) {
			if (pool[i].rts.cnt != rn) {
				pool[i].initSolution(kset[i]);
				bool isAdj = pool[i].adjustRN(rn);
				if (isAdj) {
					pool[i].mRLLocalSearch(0, {});
					bks->updateBKSAndPrint(pool[i]);
					popIsAlreadrn.push_back(i);
				}
				else {
					#if CHECKING
					lyhCheckTrue(popIsAlreadrn.size() > 0);
					#endif // CHECKING
					INFO("rn", rn);
					INFO("popIsAlreadrn.size()",popIsAlreadrn.size());
					pool[i] = pool[popIsAlreadrn[alreadyI]];
					pool[i].patternAdjustment(globalInput->custCnt);
					++alreadyI;
					alreadyI %= popIsAlreadrn.size();
				}
			}
			else {
				;
			}
		}
		return rn;
	}
	else {

		for (int i = 0; i < popSize; ++i) {
			if (pool[i].rts.cnt == 0) {

				//TODO[-1]:bug
				pool[i] = ppool[poprnLowBound][i];
				bool isAdj = pool[i].adjustRN(rn);
				pool[i].mRLLocalSearch(0, {});

				#if CHECKING
				lyhCheckTrue(isAdj==true);
				#endif // CHECKING
				bks->updateBKSAndPrint(pool[i]);
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

	Solver sol;
	sol.initSolution(0);
	sol.minimizeRN(0);

	//srnbound.mRLLocalSearch(0, {});
	for (;;) {
		sol.patternAdjustment(1000);
		sol.reCalRtsCostAndPen();

		for (int i = 0; i < sol.rts.cnt; ++i) {

			Route& r = sol.rts[i];

			auto retRts = sol.rSplit(r);

			if (retRts.size() > 1) {
				
				auto arr1 = sol.rPutCusInve(sol.rts[i]);
				for (auto v : arr1) {
					sol.rRemoveAtPos(r,v);
				}

				int removeId = sol.rts[i].routeID;
				sol.removeOneRouteByRid(removeId);

				for (auto& rt : retRts) {
					int rid = -1;
					for (int j = 0; j < sol.rts.posOf.size(); ++j) {
						if (sol.rts.posOf[j] == -1) {
							rid = j;
							break;
						}
					}

					if (rid == -1) {
						rid = sol.rts.posOf.size();
					}

					Route r1 = sol.rCreateRoute(rid);
					
					for (int pt : rt) {
						sol.rInsAtPosPre(r1, r1.tail, pt);
					}

					sol.rUpdateAvQfrom(r1, r1.head);
					sol.rUpdateZvQfrom(r1, r1.tail);
					sol.rts.push_back(r1);
				}

				auto before = sol.RoutesCost;
				sol.reCalRtsCostAndPen();
				INFO("before:",before,"after:", sol.RoutesCost, "ver after", sol.verify());

			}
		}
	}
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
	
	for (int i = poprnLowBound; i < poprnLowBound + 3; ++i) {
		curSearchRN = gotoRNPop(i);
	}
	
	auto updateppol = [&](Solver& sol,int index) {
		int tar = sol.rts.cnt;
		if (sol.rts.cnt != curSearchRN) {
			if (ppool[tar].size() == 0) {
				ppool[tar].resize(popSize);
			}
			if (sol.RoutesCost < ppool[tar][index].RoutesCost) {
				INFO("update ppool rn:", tar, "index:", index);
				ppool[tar][index] = sol;
			}
		}
	};
	
	DisType bksLastLoop = bks->bestSolFound.RoutesCost;
	int contiNotDown = 1;

	std::queue <int> rnOrderqu;
	UnorderedSet<int> rnSet;

	#if 0
	for (int i = poprnLowBound + 3; i >= poprnLowBound; --i) {
		if (rnSet.count(i) == 0) {
			rnOrderqu.push(i);
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

	if (poprnLowBound <= globalCfg->lkhRN) {
		for (int i = globalCfg->lkhRN + 3; i > std::max<int>(globalCfg->lkhRN - 1, poprnLowBound); --i) {
			if (rnSet.count(i) == 0) {
				rnOrderqu.push(i);
				rnSet.insert(i);
			}
		}
	}

	auto getNextRnSolGO = [&]() -> int {
		int ret = rnOrderqu.front();
		rnOrderqu.pop();
		rnOrderqu.push(ret);
		return ret;
	};

	curSearchRN = getNextRnSolGO();
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
		
		if (bks->bestSolFound.RoutesCost < bksLastLoop) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}
		bksLastLoop = bks->bestSolFound.RoutesCost;

		int plangoto = -1;

		if (curSearchRN != bks->bestSolFound.rts.cnt && bks->bestSolFound.rts.cnt >= poprnLowBound) {
			plangoto = bks->bestSolFound.rts.cnt;
		}

		if (contiNotDown % 3 == 0) {
			plangoto = getNextRnSolGO();
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

		// TODO[-1]:这个全领域没必要太耗时间了感觉
		#if 0
		for (int i = 0; i < popSize; ++i) {
			Solver& sol = i < popSize ? pool[i] : bks->bestSolFound;
			Solver clone = sol;
			clone.mRLLocalSearch(0, {});
			bks->updateBKSAndPrint(clone, " mRLLocalSearch(0, {})");
			if (clone.RoutesCost < sol.RoutesCost) {
				sol = clone;
			}
		}
		#endif // 0

		Solver& sol = bks->bestSolFound;
		Solver clone = sol;
		clone.Simulatedannealing(0, 40, 1.0, globalCfg->ruinC_);
		bks->updateBKSAndPrint(clone, " bks ruin simulate 0");

		clone.Simulatedannealing(1, 40, 1.0, globalCfg->ruinC_);		
		bks->updateBKSAndPrint(clone, " bks ruin simulate 1");
		updateppol(clone, 1);

		int ruinUppoolNum = 0;
		for (int i = 0; i < popSize; ++i) {
			Solver& sol = pool[i];
			Solver clone = sol;
			clone.Simulatedannealing(0, 20, 1.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 0");

			clone.Simulatedannealing(1, 20, 1.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 1");
			updateppol(sol, i);

			if (clone.RoutesCost < sol.RoutesCost) {
				if (clone.rts.cnt == sol.rts.cnt) {
					++ruinUppoolNum;
					sol = clone;
				}
			}
		}

		if (ruinUppoolNum > 3) {
			naMA();
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
