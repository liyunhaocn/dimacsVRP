
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
	
	//if (myRand->pick(3) == 0) {
	//	sclone.resetSol();
	//	sclone.initSolution(myRand->pick(4));
	//	bool isadj = sclone.adjustRN(sol.rts.cnt);
	//	if (isadj) {
	//		sol = sclone;
	//		return true;
	//	}
	//	else {
	//		sclone = sol;
	//	}
	//}
	
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
			int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt * 0.4);
			sclone.patternAdjustment(step);
		}
		 
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
	//globalCfg->ruinC_ = (globalCfg->ruinLmax + 1)/2;
	globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
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
	
	Solver sol;
	sol.initSolution(0);

	sol.adjustRN(sol.rts.cnt + 10);

	for (;;) {
		globalCfg->ruinLmax = globalInput->custCnt / sol.rts.cnt;
		//globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
		sol.Simulatedannealing(1, 500, 100.0, globalCfg->ruinC_);
		sol.patternAdjustment(100);
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

void Goal::getTheRangeMostHope() {

	int popSize = globalCfg->popSize;

	Vec<Solver> poolt(popSize);

	for (int i = 0; i < popSize; ++i) {
		poolt[i].initSolution(i%4);
		//poolt[i].initSolution(1);
		//DEBUG("poolt[i].rts.cnt:", poolt[i].rts.cnt);
		//DEBUG("globalInput->custCnt:", globalInput->custCnt);
		int adjBig = std::min<int>(globalInput->vehicleCnt, poolt[i].rts.cnt + 15);
		poolt[i].adjustRN(adjBig);
		//for (int j = 0; j < poolt[i].rts.cnt; ++j) {
		//	DEBUG("poolt[i].rts[j].rCustCnt��", poolt[i].rts[j].rCustCnt);
		//}

		if (i < 4 ) {
			//poolt[i].mRLLocalSearch(0, {});
			globalCfg->ruinLmax = globalInput->custCnt / poolt[i].rts.cnt;
			//globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
			poolt[i].Simulatedannealing(1, 500, 100.0, globalCfg->ruinC_);
			//poolt[i].Simulatedannealing(1, 2, 50.0, 1);
			updateppol(poolt[i], i);
			if (i == 0) {
				globalInput->initDetail();
			}
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
		//glbound = std::min<int>(glbound, poolt[0].rts.cnt);
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
			alreadyBound.push(i);
			alreadyBound.push(index);
			sol = (index == popSize ? bks->bestSolFound : ppool[poprnLowBound][index]);
			sol.patternAdjustment(100);
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

	globalCfg->popSize = 4;

	auto fillqu = [&]() -> void {

		Vec<int> rns;
		int bksRN = bks->bestSolFound.rts.cnt;
		if (bksRN == poprnUpBound) {
			rns = { bksRN, bksRN - 1 ,bksRN - 2 };
		}
		else if (bksRN == poprnLowBound) {
			rns = { bksRN, bksRN + 1 ,bksRN + 2 };
		}
		else {
			rns = { bksRN,bksRN + 1 ,bksRN - 1 };
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
			globalCfg->popSize *= 2;
			globalCfg->popSize %= globalCfg->popSizeMax;

			if (globalCfg->popSize < 4) {
				globalCfg->popSize = 4;
			}
			//globalCfg->popSize = std::min<int>(globalCfg->popSize, globalCfg->popSizeMax);

			if (globalCfg->popSize == globalCfg->popSizeMax) {
				globalCfg->outNeiSize = 50;
				globalCfg->mRLLocalSearchRange[1] = 50;
			}
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
		
		if (gloalTimer->getRunTime() + 30 > globalCfg->runTimer) {
			saveBKStoCsvFile();
		}

		INFO("curSearchRN:", curSearchRN);

		bksLastLoop = bks->bksAtRn[curSearchRN];

		auto& pool = ppool[curSearchRN];
		globalRepairSquIter();

		naMA(curSearchRN);

		//if (bks->bksAtRn[curSearchRN] <= bks->bestSolFound.RoutesCost) {
		if (bks->bksAtRn[curSearchRN] < bksLastLoop) {
			//globalCfg->close10randorder = 0;
			contiNotDown = 1;
		}
		else {
			//globalCfg->close10randorder = 1 ;
			++contiNotDown;
		}

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
		clone.Simulatedannealing(0, 100, 30.0, globalCfg->ruinC_);
		bks->updateBKSAndPrint(clone, " bks ruin simulate 0");
		updateppol(sol, 0);

		clone = sol;
		clone.Simulatedannealing(1, 500, 30.0, globalCfg->ruinC_);
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
