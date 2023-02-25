
#include "EAX.h"
#include "Goal.h"

namespace hust {

Goal::Goal(Input*input): 
input(input),
yearTable(input),
bks(input),
aps(input->aps),
random(&input->random),
randomx(&input->randomx)
{}

DisType Goal::getMinRoutesCostInPool(int rn) {
	DisType bestSolInPool = DisInf;
	auto& pool = mapOfPopulation[rn];
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

	for (int ch = 1; ch <= aps->namaEaxCh; ++ch) {

		Solver pc = pa;
		int eaxState = 0;
		if (kind == 0) {
			eaxState = eax.doNaEAX(pc);
		}
		else {
			eaxState = eax.doPrEAX(pc);
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
			pc.simpleLocalSearch(1, newCus);
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
			
			bool isup = bks.updateBKSAndPrint(pc," eax ls, kind:" + std::to_string(kind));
			if (isup) {
				ch = 1;
				retState = 1;
				//Logger::INFO("bestSolFound cost:", bestSolFound.RoutesCost, ", kind, "choosecyIndex:", eax.choosecyIndex, "chooseuId:", eax.unionIndex);
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
				//Logger::INFO("replace with pb,kind:", kind);
				pb = paBest;
			}
		}
		else {
			pa = paBest;
		}
	}
	return paBest.RoutesCost;
}

bool Goal::perturbOneSolution(Solver& sol) {

	//auto before = sol.RoutesCost;
	Solver sclone = sol;

	bool isPerOnePerson = false;
	for (int i = 0; i < 10; ++i) {

		//尝试使用 100度的模拟退火进行扰动
		//sclone.Simulatedannealing(0,100,100.0,aps->ruinC_);
		
		if (random->pick(2)==0) {
			int perkind = random->pick(5);
			int clearEPkind = random->pick(6);
			int ruinCusNum = std::min<int>(input->custCnt/2, aps->ruinC_);
			sclone.perturbBasedRuin(perkind, ruinCusNum, clearEPkind);
		}
		else{
			int step = random->pick(static_cast<int>(sclone.input->custCnt * 0.2),static_cast<int>( sclone.input->custCnt * 0.3));
			sclone.patternAdjustment(step);
		}
		 
		auto diff = EAX::getDiffCusofPb(sol, sclone);
		//Logger::INFO("i:",i,"diff.size:",diff.size());
		if (static_cast<int>(diff.size()) > static_cast<int>(sol.input->custCnt * 0.2)) {
			sclone.simpleLocalSearch(1, diff);
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

int Goal::EAMA(int rn) { // 1 代表更新了最优解 0表示没有

	auto& pool = mapOfPopulation[rn];

	auto& ords = randomx->getMN(populationSize, populationSize);
	//random->shuffleVec(ords);

	DisType bestSolInPool = getMinRoutesCostInPool(curSearchRouteNumber);

	//TODO[-1]:naMA这里改10了
	for (int ct = 0; ct < 10; ct++) {
		random->shuffleVec(ords);
		for (int i = 0; i < populationSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % populationSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];

#if LYH_CHECKING
			if (pa.verify() < 0) {
				Logger::ERROR("pa.verify():", pa.verify());
			}

			if (pb.verify() < 0) {
				Logger::ERROR("pb.verify():", pb.verify());
			}
#endif // LYH_CHECKING

			

			doTwoKindEAX(pa, pb, 0);
		}
		
		DisType curBest = getMinRoutesCostInPool(curSearchRouteNumber);
		if (curBest < bestSolInPool ) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

	//TODO[-1]:naMA这里改10了
	bestSolInPool = getMinRoutesCostInPool(curSearchRouteNumber);
	for (int ct = 0; ct < 10; ct++) {
		random->shuffleVec(ords);
		for (int i = 0; i < populationSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % populationSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];
			doTwoKindEAX(pa, pb, 1);
		}

		DisType curBest = getMinRoutesCostInPool(curSearchRouteNumber);
		if (curBest < bestSolInPool) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

	for (int i = 0; i < populationSize; ++i) {
		if (bks.bestSolFound.rts.cnt == pool[i].rts.cnt) {
			auto pa = bks.bestSolFound;
			auto pb = pool[i];
			doTwoKindEAX(pa, pb, 0);
			doTwoKindEAX(pa, pb, 1);
		}
	}

	Logger::INFO("curSearchRouteNumber:",curSearchRouteNumber,"getMinRoutesCostInPool():", getMinRoutesCostInPool(curSearchRouteNumber));
	return 0;
}

int Goal::gotoPopulationAtRouteNumber(int rn) {

	//TODO[-1]:注释掉了
	if (mapOfPopulation[rn].size() == 0) {
		Solver sol(input, &yearTable, &bks);
		fillPopulation(rn);
	}
	
	//TODO[0]:Lmax和ruinLmax的定义
	aps->ruinLmax = input->custCnt / rn;
	//aps->ruinC_ = (aps->ruinLmax + 1)/2;
	//aps->ruinC_ = (aps->ruinLmax + 1);
	//aps->ruinC_ = 15;
	//aps->ruinC_ = std::max<int>(aps->ruinC_, (aps->ruinLmax + 1) / 2);

	auto& pool = mapOfPopulation[rn];

	if (rn == routeNumberLowerBound) { //r如果是
		return rn;
	}

	for (int pIndex = 0; pIndex < populationSize; ++pIndex) {

		auto& sol = pool[pIndex];
		//if (sol.rts.cnt != rn) {

		bool isAdj = false;

		//TODO[-1]:从刚才搜索的位置跳
		int downRn = -1;
		DisType minRc = DisInf;

		for (int i = routeNumberUpperBound; i >= routeNumberLowerBound; --i) {
			if (mapOfPopulation[i][pIndex].rts.cnt == i) {
				if (mapOfPopulation[i][pIndex].RoutesCost < minRc) {
					minRc = mapOfPopulation[i][pIndex].RoutesCost;
					downRn = i;
				}
			}
		}

#if LYH_CHECKING
		if (downRn == -1) {
			Logger::ERROR("downRn:", downRn);
			Logger::ERROR("rn:", rn);
		}
#endif // LYH_CHECKING

		if (downRn == rn) {

#if LYH_CHECKING
			if (sol.rts.cnt != rn) {
				Logger::ERROR("sol.rts.cnt != rn");
				Logger::ERROR("sol.rts.cnt:", sol.rts.cnt);
				Logger::ERROR("rn:",rn);
				Logger::ERROR("rn:",rn);
			}
#endif // LYH_CHECKING

			sol.patternAdjustment(100);
			isAdj = true;
		}
		else {
			sol = mapOfPopulation[downRn][pIndex];
			isAdj = sol.adjustRouteNumber(rn);
			
		}

		if (!isAdj) {

			sol = mapOfPopulation[routeNumberLowerBound][pIndex];
			isAdj = sol.adjustRouteNumber(rn);
		}

#if LYH_CHECKING
		if (sol.rts.cnt != rn) {
			Logger::ERROR("downRn:", downRn);
			Logger::ERROR("isAdj:", isAdj);
			Logger::ERROR("rn:", rn);
			Logger::ERROR("sol.rts.cnt:", sol.rts.cnt);
			Logger::ERROR("sol.rts.cnt:", sol.rts.cnt);
		}
#endif // LYH_CHECKING

		updateMapOfPopulation(sol, pIndex);
		bks.updateBKSAndPrint(sol, "adjust from cur + ls");
	//}
	}
	return rn;
}

bool Goal::fillPopulation(int rn) {

	auto& pool = mapOfPopulation[rn];

	if (pool.size() == 0 ) {
		Solver sol(input, &yearTable, &bks);
		pool.resize(aps->populationSizeMax,sol);
	}

	return true;
}

int Goal::callSimulatedannealing() {

	//int ourTarget = aps->lkhRN;
	int ourTarget = 0;

	Solver st(input, &yearTable,&bks);

	st.initSolution(0);
	st.adjustRouteNumber(ourTarget);

	st.simpleLocalSearch(0, {});
	st.simulatedannealing(0,1000, 20.0, 1);
	bks.updateBKSAndPrint(st, "Simulatedannealing(0,1000, 20.0, 1)");

	st.simulatedannealing(1, 1000, 20.0, aps->ruinC_);
	bks.updateBKSAndPrint(st,"Simulatedannealing(1, 1000, 20.0, aps->ruinC_)");

	//saveSlnFile();
	return true;
}

bool Goal::test() {
	
	Solver sol(input, &yearTable,&bks);
	sol.initSolution(0);

	sol.simpleLocalSearch(0, {});

	for (;;) {
		aps->ruinLmax = input->custCnt / sol.rts.cnt;
		//aps->ruinC_ = (aps->ruinLmax + 1);
		sol.simulatedannealing(1, 500, 100.0, aps->ruinC_);
		sol.patternAdjustment(100);
	}
	return true;
}

bool Goal::callRouteNumberMinimization() {
	//input->initDetail();

	Solver solver(input, &yearTable,&bks);
	solver.initSolution(3);
	
	bool succeed = solver.minimizeRouteNumber(0);
	if (succeed) {
		Logger::INFO("succeed to delete");
	}
	else {
		Logger::INFO("fail to delete");
	}
	bks.bestSolFound = solver;
	return true;
}

void Goal::updateMapOfPopulation(Solver& sol, int index) {
	int tar = sol.rts.cnt;

	if (mapOfPopulation[tar].size() == 0) {
		Solver sol(input,&yearTable,&bks);
		mapOfPopulation[tar].resize(aps->populationSizeMax,sol);
	}
	if (sol.RoutesCost < mapOfPopulation[tar][index].RoutesCost) {
		Logger::INFO("update mapOfPopulation rn:", tar, "index:", index);
		mapOfPopulation[tar][index] = sol;
	}
};

void Goal::initialMapOfPopulation() {

	Solver sol(input, &yearTable, &bks);
	
	sol.initSolution(0);

	int adjBig = std::min<int>(input->vehicleCnt, sol.rts.cnt + 15);

	if (adjBig > sol.rts.cnt) {
		sol.adjustRouteNumber(adjBig);
	}
	
	aps->ruinLmax = input->custCnt / sol.rts.cnt;
	//aps->ruinC_ = (aps->ruinLmax + 1);
	
	int& neiborRange1 = aps->neiborRange[1];
	neiborRange1 = 40;

	sol.simulatedannealing(1, 1000, 100.0, aps->ruinC_);
	
	if (input->custCnt < sol.rts.cnt * 25 ) {
		//short route
		aps->populationSizeMin = 2;
		aps->populationSizeMax = 6;
		populationSize = aps->populationSizeMin;
		aps->neiborSizeMax = 25;
	}
	else {//long route
		aps->populationSizeMin = 4;
		aps->populationSizeMax = 50;
		populationSize = aps->populationSizeMin;
		aps->neiborSizeMax = 35;
	}

	Vector<Solver> poolt(aps->populationSizeMax,sol);
	updateMapOfPopulation(sol, 0);
	input->initDetail();

	for (int i = 1; i < aps->populationSizeMax; ++i) {
		int kind = (i == 4 ? 4 : i % 4);
		poolt[i].initSolution(kind);
		int adjBig = std::min<int>(input->vehicleCnt, poolt[i].rts.cnt + 15);

		poolt[i].adjustRouteNumber(adjBig);

		if (i <= 4 ) {
			aps->ruinLmax = input->custCnt / poolt[i].rts.cnt;
			poolt[i].simulatedannealing(1, 500, 100.0, aps->ruinC_);
			updateMapOfPopulation(poolt[i], i);
			
		}
		bks.updateBKSAndPrint(poolt[i], " poolt[i] init");
	}
	
	bks.resetBksAtRn();
	neiborRange1 = aps->neiborSizeMin;

	Vector <Vector<Solver>> soles(aps->populationSizeMax);

	int glbound = IntInf;

	for (int peopleIndex = 0; peopleIndex < aps->populationSizeMax; ++peopleIndex) {
		auto& sol = poolt[peopleIndex];
		soles[peopleIndex].push_back(sol);

		if (sol.rts.cnt < 2) {
			sol.adjustRouteNumber(5);
		}

		glbound = std::min<int>(glbound, poolt[peopleIndex].rts.cnt);
		int bound = (peopleIndex == 0 ? 2 : glbound);
		while (sol.rts.cnt > bound) {
			soles[peopleIndex].push_back(sol);
			bool isDel = sol.minimizeRouteNumber(sol.rts.cnt - 1);
			if (!isDel) {
				break;
			}
			bks.updateBKSAndPrint(sol, " poolt[0] mRLS(0, {})");
		}
	}

	routeNumberUpperBound = 0;
	routeNumberLowerBound = IntInf;
	for (int i = 0; i < aps->populationSizeMax; ++i) {
		//Logger::DEBUG(i);
		routeNumberUpperBound = std::max<int>(routeNumberUpperBound, soles[i].front().rts.cnt);
		routeNumberLowerBound = std::min<int>(routeNumberLowerBound, soles[i].back().rts.cnt);
	}

	routeNumberUpperBound = std::min<int>(routeNumberLowerBound + 15, input->custCnt);
	routeNumberUpperBound = std::max<int>(routeNumberUpperBound,bks.bestSolFound.rts.cnt);
	routeNumberLowerBound = std::min<int>(routeNumberLowerBound,bks.bestSolFound.rts.cnt);

	//TODO[-1]:这个很重要 考虑了vehicleCnt！！！
	routeNumberUpperBound = std::min<int>(routeNumberUpperBound, input->vehicleCnt);

	Logger::INFO("routeNumberLowerBound:",routeNumberLowerBound,"routeNumberUpperBound:", routeNumberUpperBound);

	if (routeNumberLowerBound > input->vehicleCnt) {
		Logger::ERROR("!!!!!this alg finshed! even cant get a sol sat vehicleCnt");
	}
	Logger::INFO("soles.size():", soles.size());

	for (int rn = routeNumberLowerBound; rn <= routeNumberUpperBound; ++rn) {
		fillPopulation(rn);
	}

	// 所有解
	for (int popIndex = 0;popIndex< aps->populationSizeMax;++popIndex) {
		auto& deorsoles = soles[popIndex];
		for (auto& sol : deorsoles) {
			if (sol.rts.cnt >= routeNumberLowerBound
				&& sol.rts.cnt <= routeNumberUpperBound) {
				mapOfPopulation[sol.rts.cnt][popIndex] = sol;
			}
		}
	}

	std::queue<int> alreadyBound;

	for (int i = 0; i < aps->populationSizeMax; ++i) {
		if (mapOfPopulation[routeNumberLowerBound][i].rts.cnt == routeNumberLowerBound) {
			alreadyBound.push(i);
		}
	}
	if (bks.bestSolFound.rts.cnt == routeNumberLowerBound) {
		alreadyBound.push(aps->populationSizeMax);
	}

	Logger::INFO("alreadyBound.size():", alreadyBound.size());
	lyhCheckTrue(alreadyBound.size() > 0);
	
	for (int i = 0; i < aps->populationSizeMax ; ++i) {
		auto& sol = mapOfPopulation[routeNumberLowerBound][i];
		if (sol.rts.cnt != routeNumberLowerBound) {
			int index = alreadyBound.front();
			alreadyBound.pop();
			alreadyBound.push(i);
			alreadyBound.push(index);
			sol = (index == aps->populationSizeMax ? bks.bestSolFound : mapOfPopulation[routeNumberLowerBound][index]);
			sol.patternAdjustment(100);
		}
	}

	populationSize = aps->populationSizeMax;
	//for (int i = routeNumberUpperBound ; i >= routeNumberLowerBound; --i) {
	for (int i = routeNumberLowerBound  ; i <= routeNumberUpperBound; ++i) {
		curSearchRouteNumber = gotoPopulationAtRouteNumber(i);
	}
	populationSize = aps->populationSizeMin;
}

int Goal::run() {

	std::queue<int>queNext;
	
	auto fillQueNext = [&]() -> void {

		Vector<int> rns;
		int bksRN = bks.bestSolFound.rts.cnt;
		if (bksRN == routeNumberUpperBound) {
			rns = { bksRN, bksRN - 1 ,bksRN - 2 };
		}
		else if (bksRN == routeNumberLowerBound) {
			rns = { bksRN, bksRN + 1 ,bksRN + 2 };
		}
		else {
			rns = { bksRN,bksRN + 1 ,bksRN - 1 };
		}

		std::sort(rns.begin(), rns.end(), [&](int x, int y) {
			return bks.bksAtRn[x] < bks.bksAtRn[y];
		});

		int n = static_cast<int>(rns.size());
		for (int i = 0; i < n ; ++i) {
		//for (int i = 0; i < 3; ++i) {
			queNext.push(rns[i]);
		}

		for (int rn : rns) {
			auto& pool = mapOfPopulation[rn];
			int pn = static_cast<int>(pool.size());
			for (int i = 0; i +1 < pn; ++i) {
				int rdi = random->pick(i+1,pn);
				std::swap(pool[i], pool[rdi]);
			}
		}
	};
	
	auto getNextRouteNumerToRun = [&]() -> int {

		if(queNext.size() == 0) {

			fillQueNext();

			populationSize *= 3;
			populationSize = std::min<int>(populationSize, aps->populationSizeMax);

			aps->ruinC_ += 3;
			if (aps->ruinC_ > aps->ruinC_Max) {
				aps->ruinC_ = aps->ruinC_Min;
			}

			int& neiborRange1 = aps->neiborRange[1];
			neiborRange1 += 5;
			if (neiborRange1 > aps->neiborSizeMax) {
				neiborRange1 = aps->neiborSizeMin;
			}
		}

		Logger::INFO("aps->populationSize:", populationSize);
		Logger::INFO("queNext.size():", queNext.size());
		
		int ret = queNext.front();
		queNext.pop();

		return ret;
	};

	initialMapOfPopulation();

	int rnn = getNextRouteNumerToRun();

	populationSize = aps->populationSizeMin;
	
	curSearchRouteNumber = gotoPopulationAtRouteNumber(rnn);

	DisType bksLastLoop = bks.bestSolFound.RoutesCost;
	int stagnation = 1;

#if LYH_CHECKING
	for (int rn = routeNumberLowerBound; rn <= routeNumberUpperBound; ++rn) {
		for (int i = 0; i < aps->popSizeMax; ++i) {
			if (mapOfPopulation[rn][i].rts.cnt != rn) {
				Logger::ERROR("rn:", rn);
				Logger::ERROR("i:", i);
				Logger::ERROR("i:", i);
			}
		}
	}
#endif // LYH_CHECKING

	while (true) {
		
#if LYH_CHECKING
		for (int rn = routeNumberLowerBound; rn <= routeNumberUpperBound; ++rn) {
			for (int i = 0; i < aps->popSizeMax; ++i) {
				if (mapOfPopulation[rn][i].rts.cnt != rn) {
					Logger::ERROR("rn:", rn);
					Logger::ERROR("i:", i);
				}
			}
		}
#endif // LYH_CHECKING

		int& neiSize = aps->neiborRange[1];

		Logger::INFO("curSearchRouteNumber:", curSearchRouteNumber,
			"populationSize:",populationSize,
			"aps->ruinC_:", aps->ruinC_,
			"neiSize:", neiSize);

		bksLastLoop = bks.bksAtRn[curSearchRouteNumber];

		auto& pool = mapOfPopulation[curSearchRouteNumber];
		
		EAMA(curSearchRouteNumber);

		int outpeopleNum = std::min<int>(5,populationSize);
		auto& arr = randomx->getMN(populationSize, outpeopleNum);

		for (int i = 0; i < outpeopleNum; ++i) {
			int index = arr[i];
			Solver& sol = pool[index];
			Solver clone = sol;
			clone.simulatedannealing(1, 100, 50.0, aps->ruinC_);
			bks.updateBKSAndPrint(clone, " pool sol simulate 1");
			updateMapOfPopulation(sol, i);
		}

		Solver& sol = bks.bestSolFound;
		Solver clone = sol;
		clone.simulatedannealing(1, 500, 100.0, aps->ruinC_);
		bks.updateBKSAndPrint(clone, " bks ruin simulate 1");
		updateMapOfPopulation(sol, 0);
		
		if (bks.bksAtRn[curSearchRouteNumber] < bksLastLoop) {
			stagnation = 1;
		}
		else {
			++stagnation;
		}

		int plangoto = -1;
		if (stagnation >= 2) {
			plangoto = getNextRouteNumerToRun();
		}

		if (plangoto != -1) {
			curSearchRouteNumber = gotoPopulationAtRouteNumber(plangoto);
			if (curSearchRouteNumber == plangoto) {
				Logger::INFO("jump succeed curSearchRouteNumber", curSearchRouteNumber);
			}
			else {
				Logger::INFO("jump fail curSearchRouteNumber", curSearchRouteNumber, "plangoto:", plangoto);
			}
		}
	}

	bks.bestSolFound.printDimacs();

	timer->disp();

	return true;
}

}
