
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

	int retState = 0; // 0 ��ʾû�гɹ��������Ž⣬1��ʾ���������Ž� -1��ʾ���������޷�����eax

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

bool Goal::perturbOneSol(Solver& sol) {

	//auto before = sol.RoutesCost;
	Solver sclone = sol;
	
	bool isPerOnePerson = false;
	for (int i = 0; i < 10; ++i) {

		//����ʹ�� 100�ȵ�ģ���˻�����Ŷ�
		//sclone.Simulatedannealing(0,100,100.0,globalCfg->ruinC_);
		
		if (myRand->pick(2)==0) {
			int kind = myRand->pick(4);
			int clearEPkind = myRand->pick(6);
			int ruinCusNum = std::min<int>(globalInput->custCnt/2, globalCfg->ruinC_);
			sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
		}
		else{
			int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt * 0.3);
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

int Goal::naMA(int rn) { // 1 ������������Ž� 0��ʾû��

	auto& pool = ppool[rn];

	int popSize = globalCfg->popSize;
	auto& ords = myRandX->getMN(popSize, popSize);
	
	DisType bestSolInPool = getMinRtCostInPool(curSearchRN);

	//TODO[-1]:naMA�����10��
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

	//TODO[-1]:naMA�����10��
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

	//TODO[-1]:ע�͵���
	if (ppool[rn].size() == 0) {
		fillPopulation(rn);
	}
	
	//TODO[0]:Lmax��ruinLmax�Ķ���
	globalCfg->ruinLmax = globalInput->custCnt / rn;
	globalCfg->ruinC_ = (globalCfg->ruinLmax + 1)/2;
	//globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
	//globalCfg->ruinC_ = 15;
	//globalCfg->ruinC_ = std::max<int>(globalCfg->ruinC_, (globalCfg->ruinLmax + 1) / 2);

	auto& pool = ppool[rn];
	int popSize = globalCfg->popSize;

	if (rn == poprnLowBound) { //r�����
		return rn;
	}

	for (int pIndex = 0; pIndex < popSize; ++pIndex) {

		auto& sol = pool[pIndex];
		//if (sol.rts.cnt != rn) {

		bool isAdj = false;

		//TODO[-1]:�Ӹղ�������λ����
		int downRn = -1;
		DisType minRc = DisInf;

		for (int i = poprnUpBound; i >= poprnLowBound; --i) {
			if (ppool[i][pIndex].rts.cnt == i) {
				if (ppool[i][pIndex].RoutesCost < minRc) {
					minRc = ppool[i][pIndex].RoutesCost;
					downRn = i;
				}
			}
		}
		if (downRn == rn) {
			perturbOneSol(ppool[downRn][pIndex]);
			isAdj = true;
		}
		else {
			sol = ppool[downRn][pIndex];
			isAdj = sol.adjustRN(rn);
		}
		
		if (!isAdj) {

			for (int i = rn - 1; i >= poprnLowBound; --i) {

				if (ppool[i][pIndex].rts.cnt == i) {
					if (ppool[i][pIndex].RoutesCost < minRc) {
						minRc = ppool[i][pIndex].RoutesCost;
						downRn = i;
					}
				}
			}
			sol = ppool[downRn][pIndex];
			isAdj = sol.adjustRN(rn);
		}
		updateppol(sol, pIndex);
		bks->updateBKSAndPrint(sol, "adjust from cur + ls");
	//}
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

void Goal::getTheRangeMostHope() {

	int popSize = globalCfg->popSize;

	Vec<Solver> poolt(popSize);

	for (int i = 0; i < popSize; ++i) {
		poolt[i].initSolution( i%4);
		//DEBUG("poolt[i].rts.cnt:", poolt[i].rts.cnt);
		//DEBUG("globalInput->custCnt:", globalInput->custCnt);
		poolt[i].adjustRN(poolt[i].rts.cnt+10);
		//for (int j = 0; j < poolt[i].rts.cnt; ++j) {
		//	DEBUG("poolt[i].rts[j].rCustCnt��", poolt[i].rts[j].rCustCnt);
		//}

		if (i < 4 ) {
			//poolt[i].mRLLocalSearch(0, {});
			globalCfg->ruinLmax = globalInput->custCnt / poolt[i].rts.cnt;
			//globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
			poolt[i].Simulatedannealing(1, 500, 50.0, globalCfg->ruinC_);
			//poolt[i].Simulatedannealing(1, 2, 50.0, 1);
			updateppol(poolt[i], i);
		}
		bks->updateBKSAndPrint(poolt[i], " poolt[i] init");
	}
	
	bks->resetBksAtRn();
	Vec <Vec<Solver>> soles(popSize);

	int glbound = IntInf;

	for (int peopleIndex = 0; peopleIndex < popSize; ++peopleIndex) {
		auto& sol = poolt[peopleIndex];
		soles[peopleIndex].push_back(sol);

		//sol.Simulatedannealing(0, 2, 1.0, globalCfg->ruinC_);
		if (sol.rts.cnt < 2) {
			sol.adjustRN(5);
		}

		//if (peopleIndex >= 10) {
		//	continue;
		//}

		glbound = std::min<int>(glbound, poolt[peopleIndex].rts.cnt);
		int bound = peopleIndex == 0 ? 2 : glbound;
		while (sol.rts.cnt > bound) {
		//while (sol.rts.cnt > 2) {
			soles[peopleIndex].push_back(sol);
			bool isDel = sol.minimizeRN(sol.rts.cnt - 1);
			if (!isDel) {
				break;
			}
			bks->updateBKSAndPrint(sol, " poolt[0] mRLS(0, {})");
		}
	}

	poprnUpBound = 0;
	poprnLowBound = IntInf;
	for (int i = 0; i < popSize; ++i) {
		//DEBUG(i);
		poprnUpBound = std::max<int>(poprnUpBound, soles[i].front().rts.cnt);
		poprnLowBound = std::min<int>(poprnLowBound, soles[i].back().rts.cnt);
	}

	poprnUpBound = std::min<int>(poprnLowBound + 15, globalInput->custCnt);
	poprnUpBound = std::max<int>(poprnUpBound,bks->bestSolFound.rts.cnt);
	poprnLowBound = std::min<int>(poprnLowBound,bks->bestSolFound.rts.cnt);

	//TODO[-1]:�������Ҫ ������vehicleCnt������
	poprnUpBound = std::min<int>(poprnUpBound, globalInput->vehicleCnt);

	INFO("poprnLowBound:",poprnLowBound,"poprnUpBound:", poprnUpBound);

	if (poprnLowBound > globalInput->vehicleCnt) {
		ERROR("!!!!!this alg finshed! even cant get a sol sat vehicleCnt");
	}
	INFO("soles.size():", soles.size());

	for (int rn = poprnLowBound; rn <= poprnUpBound; ++rn) {
		fillPopulation(rn);
	}

	// ���н�
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
			sol = (index == popSize ? bks->bestSolFound : ppool[poprnLowBound][index]);
			sol.patternAdjustment(globalInput->custCnt);
		}
	}

	//for (int i = poprnUpBound ; i >= poprnLowBound; --i) {
	for (int i = poprnLowBound  ; i <= poprnUpBound; ++i) {
		curSearchRN = gotoRNPop(i);
	}

	//// globalCfg->popSize = globalCfg->popSizeMax;
	//globalCfg->popSize = 3;
	////for (int rn = poprnUpBound ; rn >= poprnLowBound; --rn) {
	//for (int rn = poprnLowBound; rn <= poprnUpBound; ++rn) {
	//	curSearchRN = gotoRNPop(rn);
	//	naMA(curSearchRN);
	//}
}

int Goal::TwoAlgCombine() {

	getTheRangeMostHope();

	std::queue<int>qunext;

	int iterFillqu = 0;

	globalCfg->popSize = 5;

	auto fillqu = [&]() -> void {

		//Vec<int> rns(poprnUpBound - poprnLowBound + 1);

		//Vec<DisType> avgrl(poprnUpBound + 1, 0);
		//for (int rn = poprnLowBound; rn < poprnUpBound; ++rn) {
		//	for (int i = 0; i < globalCfg->popSize; ++i) {
		//		avgrl[rn] += ppool[rn][i].RoutesCost;
		//	}
		//}
		UnorderedSet<int> s = { poprnLowBound };
		if (poprnLowBound + 1 <= poprnUpBound) {
			s.insert(poprnLowBound + 1);
		}

		if (poprnLowBound + 2 <= poprnUpBound) {
			s.insert(poprnLowBound + 2);
		}
		s.insert(bks->bestSolFound.rts.cnt);
		s.insert(poprnUpBound);

		//Vec<int> rns = putEleInVec(s);
		//std::iota(rns.begin(), rns.end(), poprnLowBound);
		//std::sort(rns.begin(), rns.end(), [&](int x, int y) {
		//	return avgrl[x] < avgrl[y];
		//});

		Vec<int> rns;
		int bksRN = bks->bestSolFound.rts.cnt;
		if (bksRN == poprnUpBound) {
			rns = { bksRN - 2 ,bksRN - 1 };
		}
		else if (bksRN == poprnLowBound) {
			rns = { bksRN + 2 ,bksRN + 1 };
		}
		else {
			rns = { bksRN + 1 ,bksRN - 1 };
		}

		std::sort(rns.begin(), rns.end(), [&](int x, int y) {
			return bks->bksAtRn[x] < bks->bksAtRn[y];
		});

		int n = rns.size();
		for (int i = 0; i < n ; ++i) {
		//for (int i = 0; i < 3; ++i) {
			qunext.push(rns[i]);
		}

		for (int rn : rns) {
			auto& pool = ppool[rn];
			int pn = pool.size();
			for (int i = 0; i +1 < pn; ++i) {
				int rdi = myRand->pick(i+1,pn);
				std::swap(pool[i], pool[rdi]);
			}
		}
		//if (iterFillqu + 2 < n) {
		//	++iterFillqu;
		//}
	};
	
	auto getNextRnSolGO = [&]() -> int {

		//return bks->bestSolFound.rts.cnt;

		if(qunext.size() == 0) {

			fillqu();

			globalCfg->popSize += 2;
			globalCfg->popSize = std::min<int>(globalCfg->popSize, globalCfg->popSizeMax);

		}

		INFO("globalCfg->popSize:", globalCfg->popSize);
		INFO("qunext.size():", qunext.size());
		auto q = qunext;
		while (!q.empty()) {
			INFO("q.front():", q.front());
			q.pop();
		}

		int ret = qunext.front();
		qunext.pop();
		return ret;
	};

	int rnn = getNextRnSolGO();
	curSearchRN = gotoRNPop(rnn);

	DisType bksLastLoop = bks->bestSolFound.RoutesCost;
	int contiNotDown = 1;

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
		
		if (gloalTimer->getRunTime() + 100 > globalCfg->runTimer) {
			saveBKStoCsvFile();
		}

		INFO("curSearchRN:", curSearchRN);

		bksLastLoop = bks->bksAtRn[curSearchRN];

		auto& pool = ppool[curSearchRN];
		globalRepairSquIter();

		naMA(curSearchRN);

		int outpeopleNum = std::min<int>(5,globalCfg-> popSize);
		auto& arr = myRandX->getMN(globalCfg->popSize, outpeopleNum);
		for (int i = 0; i < outpeopleNum; ++i) {
			int index =arr[i];
			Solver& sol = pool[index];
			Solver clone = sol;
			clone.Simulatedannealing(0, 50, 30.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 0");
			updateppol(sol, index);
			clone = sol;
			clone.Simulatedannealing(1, 50, 30.0, globalCfg->ruinC_);
			bks->updateBKSAndPrint(clone, " pool sol simulate 1");
			updateppol(sol, i);
		}
		Solver& sol = bks->bestSolFound;
		Solver clone = sol;
		clone.Simulatedannealing(0, 100, 100.0, globalCfg->ruinC_);
		bks->updateBKSAndPrint(clone, " bks ruin simulate 0");
		updateppol(sol, 0);
		clone = sol;
		clone.Simulatedannealing(1, 100, 100.0,globalCfg->ruinC_);
		bks->updateBKSAndPrint(clone, " bks ruin simulate 1");
		updateppol(sol, 0);

		//clone = sol;
		//clone.splitLS();
		//bks->updateBKSAndPrint(clone, " bks splitLS");
		//updateppol(sol, 0);

		//======
		INFO("perturbAllPop");
		for (int i = 0; i < globalCfg->popSize; ++i) {
			perturbOneSol(pool[i]);
		}
		//if (bks->bksAtRn[curSearchRN] <= bks->bestSolFound.RoutesCost) {
		if (bks->bksAtRn[curSearchRN] < bksLastLoop) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}
		

		int plangoto = -1;
		if (contiNotDown >= 2) {
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
	}

	bks->bestSolFound.printDimacs();

	//TODO[-1]:����ύ������ǵ�ȥ��
	saveBKStoCsvFile();

	gloalTimer->disp();

	return true;
}

}
