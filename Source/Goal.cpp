
#include "EAX.h"
#include "Goal.h"

namespace hust {

Goal::Goal(Input*input): 
aps(input->aps),
input(input),
yearTable(input),
bks(input),
random(&input->random),
randomx(&input->randomx),
timer(&input->timer)
{}

DisType Goal::getMinRoutesCostInPool(int rn) {
	DisType bestSolInPool = DisInf;
	auto& pool = mapOfPopulation[rn];
	for (int i = 0; i < static_cast<int>(pool.size()); ++i) {
		bestSolInPool = std::min<DisType>(bestSolInPool, pool[i].RoutesCost);
	}
	return bestSolInPool;
}

DisType Goal::doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

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

	for (int ch = 1; ch <= aps-> eamaEaxCh; ++ch) {

		Solver pc = pa;
		int eaxState = 0;
		if (kind == 0) {
			eaxState = eax.doNaEAX(pc);
		}
		else {
			eaxState = eax.doPrEAX(pc);
		}

		if (eaxState == -1) {
			eax.generateCycles();
			continue;
		}

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

		auto newCus = EAX::getDiffCusofPb(pa, pc);

		if (newCus.size() > 0) {
			pc.simpleLocalSearch(1, newCus);
		}
		else {
		}

		if (pc.RoutesCost < paBest.RoutesCost) {
			paBest = pc;
			
			bool isup = bks.updateBKSAndPrint(pc," eax ls, kind:" + std::to_string(kind));
			if (isup) {
				ch = 1;
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
				pb = paBest;
			}
		}
		else {
			pa = paBest;
		}
	}
	return paBest.RoutesCost;
}

void Goal::EAMA(int rn) { // 1 update bks 0 not update bks

	auto& pool = mapOfPopulation[rn];

	auto& ords = randomx->getMN(populationSize, populationSize);

	DisType bestSolInPool = getMinRoutesCostInPool(curSearchRouteNumber);

	for (int ct = 0; ct < 10; ct++) {
		random->shuffleVec(ords);
		for (int i = 0; i < populationSize; ++i) {
			int paIndex = ords[i];
			int pbIndex = ords[(i + 1) % populationSize];
			Solver& pa = pool[paIndex];
			Solver& pb = pool[pbIndex];

			doTwoKindEAX(pa, pb, 0);
		}
		
		DisType curBest = getMinRoutesCostInPool(curSearchRouteNumber);
		if (curBest < bestSolInPool ) {
			bestSolInPool = curBest;
			ct = 0;
		}
	}

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
}

int Goal::gotoPopulationAtRouteNumber(int rn) {

	if (rn < routeNumberLowerBound || rn > routeNumberUpperBound) {
		return routeNumberLowerBound;
	}

	lyhCheckTrue(static_cast<int>(mapOfPopulation[rn].size()) == aps->populationSizeMax);
	
	aps->ruinLmax = input->customerNumer / rn;
	auto& poolAtRouteNumberRn = mapOfPopulation[rn];

	for (int pIndex = 0; pIndex < aps->populationSizeMax; ++pIndex) {
		auto& sol = poolAtRouteNumberRn[pIndex];
		sol = mapOfPopulation[random->pick(routeNumberLowerBound, rn+1)][pIndex];
		sol.adjustRouteNumber(rn);
		lyhCheckTrue(sol.adjustRouteNumber(rn) == true);
		sol.perturb(500);
		lyhCheckTrue(sol.rts.cnt == rn);
		bks.updateBKSAndPrint(sol, "adjust from cur + ls");
	}
	return rn;
}

int Goal::callSimulatedannealing() {

	//int ourTarget = aps->lkhRN;
	int ourTarget = 0;

	Solver st(input, &yearTable,&bks);

	st.initSolution(0);
	st.adjustRouteNumber(ourTarget);

	st.simpleLocalSearch(0, {});
	st.refinement(0,1000, 20.0, 1);
	bks.updateBKSAndPrint(st, "Simulatedannealing(0,1000, 20.0, 1)");

	st.refinement(1, 1000, 20.0, aps->ruinC_);
	bks.updateBKSAndPrint(st,"Simulatedannealing(1, 1000, 20.0, aps->ruinC_)");

	//saveSlnFile();
	return true;
}

bool Goal::test() {
	
	Solver sol(input, &yearTable,&bks);
	sol.initSolution(0);

	sol.simpleLocalSearch(0, {});

	for (;;) {
		aps->ruinLmax = input->customerNumer / sol.rts.cnt;
		//aps->ruinC_ = (aps->ruinLmax + 1);
		sol.refinement(1, 500, 100.0, aps->ruinC_);
		sol.perturb(100);
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

void Goal::addSolverToMapOfPopulation(Solver& sol) {
	int routeNumner = sol.rts.cnt;

	if (static_cast<int>(mapOfPopulation[routeNumner].size()) < aps->populationSizeMax) {
		mapOfPopulation[routeNumner].push_back(sol);
	}
	else {
		for (int index = 0; index < aps->populationSizeMax; ++index) {
			if (sol.RoutesCost < mapOfPopulation[routeNumner][index].RoutesCost) {
				mapOfPopulation[routeNumner][index] = sol;
				break;
			}
		}
	}
};

void Goal::initialMapOfPopulation() {

	Solver solver(input, &yearTable, &bks);
	solver.initSolution(1);

	int moreNumberOfRoutes = std::min<int>(input->vehicleNumber, solver.rts.cnt + 10);
	solver.adjustRouteNumber(moreNumberOfRoutes);

	int& neiborRange1 = aps->neiborRange[1];
	neiborRange1 = 40;
	solver.refinement(1, 100, 100.0, aps->ruinC_);

	addSolverToMapOfPopulation(solver);
	input->initDetail();

	populationSize = aps->populationSizeMin;

	routeNumberUpperBound = std::min<int>(input->vehicleNumber, solver.rts.cnt);
	routeNumberLowerBound = routeNumberUpperBound;

	int testBound = 0;

	Logger::INFO("initialPops aps->populationSizeMax:", aps->populationSizeMax);
	for (int index = 0; index < std::min<int>(5,aps->populationSizeMax); ++index) {
		
		solver.initSolution(index%5);

		do {
			addSolverToMapOfPopulation(solver);
			bks.updateBKSAndPrint(solver);
			routeNumberLowerBound = std::min<int>(routeNumberLowerBound,solver.rts.cnt);
		} while (solver.adjustRouteNumber(solver.rts.cnt-1) && solver.rts.cnt > testBound);
		testBound = std::max<int>(testBound, solver.rts.cnt);
	}

	if (routeNumberLowerBound > input->vehicleNumber) {
		Logger::ERROR("sovler finshed! even cant get a feasible vehicleNumber");
		exit(0);
	}

	routeNumberUpperBound = std::min<int>(routeNumberUpperBound, routeNumberLowerBound + 10);

	for (auto it = mapOfPopulation.begin(); it != mapOfPopulation.end();) {
		if (it->first < routeNumberLowerBound || it->first > routeNumberUpperBound) {
			it = mapOfPopulation.erase(it);
		}
		else {
			++it;
		}
	}
	
	auto& popAtRouteNumberBound = mapOfPopulation[routeNumberLowerBound];
	lyhCheckTrue(popAtRouteNumberBound.size() > 0);

	for (int rn = routeNumberLowerBound; rn <= routeNumberUpperBound; ++rn) {
		auto& popAtRouteNumberI = mapOfPopulation[rn];
		
		while ( static_cast<int>(popAtRouteNumberI.size()) < aps->populationSizeMax) {
			int chooseIndex = random->pick(static_cast<int>(popAtRouteNumberBound.size()));
			solver = popAtRouteNumberBound[chooseIndex];
			solver.adjustRouteNumber(rn);
			solver.perturb(100);
			popAtRouteNumberI.push_back(solver);
		}
		lyhCheckTrue(static_cast<int>(mapOfPopulation[rn].size()) == aps->populationSizeMax);
		lyhCheckTrue( mapOfPopulation[rn][0].rts.cnt == rn);
	}
}

void Goal::fillAlternativeRouteNumbers() {

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
	for (int i = 0; i < n; ++i) {
		//for (int i = 0; i < 3; ++i) {
		alternativeRouteNumbers.push(rns[i]);
	}

	for (int rn : rns) {
		auto& pool = mapOfPopulation[rn];
		int pn = static_cast<int>(pool.size());
		for (int i = 0; i + 1 < pn; ++i) {
			int rdi = random->pick(i + 1, pn);
			std::swap(pool[i], pool[rdi]);
		}
	}
};

int Goal::getNextRouteNumerToRun() {

	if (alternativeRouteNumbers.size() == 0) {

		fillAlternativeRouteNumbers();

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

	int ret = alternativeRouteNumbers.front();
	alternativeRouteNumbers.pop();
	return ret;
};

int Goal::run() {

	initialMapOfPopulation();
	int nextRouteNumber = getNextRouteNumerToRun();
	curSearchRouteNumber = gotoPopulationAtRouteNumber(nextRouteNumber);
	populationSize = aps->populationSizeMin;

	DisType bksLastLoop = bks.bestSolFound.RoutesCost;
	int stagnation = 0;

	while (!timer->isTimeOut()) {
		
		int& neiSize = aps->neiborRange[1];

		Logger::INFO(
			"curSearchRouteNumber:", curSearchRouteNumber,
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
			clone.refinement(1, 100, 50.0, aps->ruinC_);
			bks.updateBKSAndPrint(clone, " pool sol simulate 1");
			addSolverToMapOfPopulation(sol);
		}

		Solver& sol = bks.bestSolFound;
		Solver clone = sol;
		clone.refinement(1, 500, 100.0, aps->ruinC_);
		bks.updateBKSAndPrint(clone, " bks refinement simulate 1");
		addSolverToMapOfPopulation(sol);
		
		if (bks.bksAtRn[curSearchRouteNumber] < bksLastLoop) {
			stagnation = 0;
		}
		else {
			++stagnation;
		}

		if (stagnation >= 2) {
			int planGotoRouterNumber = getNextRouteNumerToRun();
			curSearchRouteNumber = gotoPopulationAtRouteNumber(planGotoRouterNumber);
			stagnation = 0;
		}
	}

	bks.bestSolFound.printDimacs();
	timer->disp();

	return true;
}

void Goal::saveBestFoundSolution() {

	String outputFilePath = input->commandLine->outputDir;

	std::fstream fs1(outputFilePath, std::ios::in | std::ios::out | std::ios::app);

	auto& bestSolFound = bks.bestSolFound;

	for (int i = 0; i < bestSolFound.rts.cnt; ++i) {
		Route& r = bestSolFound.rts[i];
		fs1<< "Route #" << i + 1<<":";
		auto arr = bestSolFound.rPutCustomersInVector(r);
		for (int pt :arr) {
			fs1<<" "<< pt;
		}
		fs1 << std::endl; 
	}

	char a[30];
	snprintf(a,30,"Cost %.1lf\n", double(bestSolFound.RoutesCost) / 10);
	fs1 << a;

	fs1.clear();
	fs1.close();
}

}
