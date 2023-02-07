
#ifndef CN_HUST_LYH_SOLVER_H
#define CN_HUST_LYH_SOLVER_H

#include <queue>
#include <functional>
#include <set>
#include <limits.h>

#include "Flag.h"
#include "Util_Common.h"
#include "Problem.h"
#include "YearTable.h"

namespace hust {

struct BKS;

struct Route {

public:

	int routeID = -1;
	int rCustCnt = 0; //没有计算仓库
	DisType rQ = 0;
	int head = -1;
	int tail = -1;

	DisType rPc = 0;
	DisType rPtw = 0;
	int rWeight = 1;
	DisType routeCost = 0;

	Route() {

		routeID = -1;
		rCustCnt = 0; //没有计算仓库
		rQ = 0;
		head = -1;
		tail = -1;

		rPc = 0;
		rPtw = 0;

		rWeight = 1;
		routeCost = 0;

	}

	Route(const Route& r) {

		this->routeID = r.routeID;
		this->rCustCnt = r.rCustCnt;
		this->rQ = r.rQ;
		this->head = r.head;
		this->tail = r.tail;

		this->rPc = r.rPc;
		this->rPtw = r.rPtw;
		this->rWeight = r.rWeight;
		this->routeCost = r.routeCost;
	}

	Route(int rid) {

		this->routeID = rid;
		this->rCustCnt = 0; //没有计算仓库
		this->head = -1;
		this->tail = -1;
		this->rQ = 0;
		this->rWeight = 1;
		this->rPc = 0;
		this->rPtw = 0;
		this->routeCost = 0;
	}

	Route& operator = (const Route& r) {

		if (this != &r) {

			this->routeID = r.routeID;
			this->rCustCnt = r.rCustCnt;
			this->rQ = r.rQ;
			this->head = r.head;
			this->tail = r.tail;

			this->rPc = r.rPc;
			this->rPtw = r.rPtw;
			this->rWeight = r.rWeight;
			this->routeCost = r.routeCost;
		}
		return *this;
	}

};

struct RTS {

	Vec<Route> ve;
	Vec<int> posOf;
	int cnt = 0;

	RTS() = default;

	RTS(int maxSize) {
		cnt = 0;
		ve.reserve(maxSize);
		for (int i = 0; i < maxSize; ++i) {
			Route r;
			ve.push_back(r);
		}
		posOf = Vec<int>(maxSize, -1);
	}

	RTS& operator = (const RTS& r) {
		if (this != &r) {
			cnt = r.cnt;
			ve = r.ve;
			posOf = r.posOf;
		}
		return *this;
	}

	~RTS() {
		ve.clear();
		posOf.clear();
	}

	Route& operator [](int index) {

#if LYH_CHECKING
		lyhCheckTrue(index < ve.size());
		lyhCheckTrue(index >= 0);
#endif // LYH_CHECKING

		return ve[index];
	}

	bool push_back(Route& r1) {

#if LYH_CHECKING
		lyhCheckTrue(r1.routeID >= 0);
#endif // LYH_CHECKING

		if (r1.routeID >= posOf.size()) {
			size_t newSize = posOf.size() + std::max<size_t>(r1.routeID + 1, posOf.size() / 2 + 1);
			ve.resize(newSize, Route());
			posOf.resize(newSize, -1);
		}

		ve[cnt] = r1;
		posOf[r1.routeID] = cnt;
		++cnt;
		return true;
	}

	bool removeIndex(int index) {

		if (index < 0 || index >= cnt) {
			return false;
		}

		if (index != cnt - 1) {
			int id = ve[index].routeID;

			int cnt_1_id = ve[cnt - 1].routeID;
			posOf[cnt_1_id] = index;
			posOf[id] = -1;
			ve[index] = ve[cnt - 1];
			cnt--;
		}
		else {
			int id = ve[index].routeID;
			posOf[id] = -1;
			cnt--;
		}

		return true;
	}

	Route& getRouteByRid(int rid) {

#if LYH_CHECKING
		lyhCheckTrue(rid >= 0);
		lyhCheckTrue(rid < posOf.size());
		lyhCheckTrue(posOf[rid] >= 0);
#endif // LYH_CHECKING

		int index = posOf[rid];

#if LYH_CHECKING
		lyhCheckTrue(index >= 0);
		lyhCheckTrue(index < cnt);
#endif // LYH_CHECKING

		return ve[index];
	}

	bool reset() {
		for (int i = 0; i < cnt; ++i) {
			posOf[ve[i].routeID] = -1;
		}
		cnt = 0;
		return true;
	}

};

struct ConfSet {

	Vec<int> ve;
	Vec<int> pos;
	int cnt = 0;

	ConfSet() = default;
	ConfSet(int maxSize) {
		cnt = 0;
		ve = Vec<int>(maxSize + 1, -1);
		pos = Vec<int>(maxSize + 1, -1);
	}

	ConfSet(const ConfSet& cs) {
		cnt = cs.cnt;
		ve = cs.ve;
		pos = cs.pos;
	}

	ConfSet& operator = (const ConfSet& cs) {
		if (this != &cs) {
			this->cnt = cs.cnt;
			this->ve = cs.ve;
			this->pos = cs.pos;
		}
		return *this;
	}

	ConfSet(ConfSet&& cs) noexcept = default;
	ConfSet& operator = (ConfSet&& cs) noexcept = default;

	int size() {
		return cnt;
	}

	bool reset() {

		for (int i = 0; i < cnt; ++i) {
			// 
			pos[ve[i]] = -1;
			ve[i] = -1;
		}

		cnt = 0;
		return true;
	}

	bool insert(int val) {

#if LYH_CHECKING
		lyhCheckTrue(val >= 0);
#endif // LYH_CHECKING

		if (val >= pos.size()) {
			int newSize = static_cast<int>(pos.size()) + std::max<int>(val + 1, static_cast<int>(pos.size()) / 2 + 1);
			ve.resize(newSize, -1);
			pos.resize(newSize, -1);
		}

		if (pos[val] >= 0) {
			return false;
		}

		ve[cnt] = val;
		pos[val] = cnt;
		++cnt;
		return true;
	}

	Vec<int>putElementInVector() {
		return Vec<int>(ve.begin(), ve.begin() + cnt);
	}

	int randomPeek(Random* random) {
		if (cnt == 0) {
			Logger::ERROR("container.cnt == 0");
		}
		int index = random->pick(cnt);
		return ve[index];
	}

	bool removeVal(int val) {

		if (val >= pos.size() || val < 0) {
			return false;
		}

		int index = pos[val];
		if (index == -1) {
			return false;
		}
		pos[ve[cnt - 1]] = index;
		pos[val] = -1;

		ve[index] = ve[cnt - 1];
		cnt--;
		return true;
	}

};

struct NextPermutation {

	bool hasNext(Vec<int>& ve, int Kmax, int& k, int N) {

		if (k == 1 && ve[k] == N) {
			return false;
		}
		else {
			return true;
		}
		return false;
	}

	bool nextPer(Vec<int>& ve, int Kmax, int& k, int N) {

		if (k < Kmax && ve[k] < N) {
			++k;
			ve[k] = ve[k - 1] + 1;
		}
		else if (ve[k] == N) {
			k--;
			++ve[k];
		}
		else if (k == Kmax && ve[k] < N) {
			++ve[k];
		}

		return true;
	}

};

class YearTable;

struct DeltPen {

	DisType deltPtw = DisInf;
	DisType deltPc = DisInf;
	DisType PcOnly = DisInf;
	DisType PtwOnly = DisInf;
	DisType deltCost = DisInf;

	DeltPen() {

		deltPc = DisInf;
		PcOnly = DisInf;
		PtwOnly = DisInf;
		deltCost = DisInf;
	}
	DeltPen(const DeltPen& d) {
		this->deltPc = d.deltPc;
		this->deltPtw = d.deltPtw;
		this->PtwOnly = d.PtwOnly;
		this->PcOnly = d.PcOnly;
		this->deltCost = d.deltCost;
	}
};

struct TwoNodeMove {

	int v = -1;
	int w = -1;
	int kind = -1;

	DeltPen deltPen;

	TwoNodeMove(int v, int w, int kind, DeltPen d) :
		v(v), w(w), kind(kind), deltPen(d) {
	}

	TwoNodeMove(Vec<int> ve, int kind, DeltPen d) :
		kind(kind), deltPen(d) {}

	TwoNodeMove() :
		v(0), w(0), kind(0) {
		deltPen.deltPc = DisInf;
		deltPen.deltPtw = DisInf;
		deltPen.PtwOnly = DisInf;
		deltPen.PcOnly = DisInf;
	}

	bool reset() {
		v = 0;
		w = 0;
		kind = 0;
		deltPen.deltPc = DisInf;
		deltPen.deltPtw = DisInf;
		deltPen.PtwOnly = DisInf;
		deltPen.PcOnly = DisInf;
		deltPen.deltCost = DisInf;
		return true;
	}
};

class Solver
{
public:

	Input* input = nullptr;

	Vec<Customer> customers;

	DisType penalty = 0;
	DisType Ptw = 0;
	DisType PtwNoWei = 0;
	DisType Pc = 0;

	DisType alpha = 1;
	DisType beta = 1;
	DisType gamma = 1;

	DisType RoutesCost = DisInf;

	RTS rts;
	ConfSet EP;

	ConfSet PtwConfRts, PcConfRts;

	//LL EPIter = 1;
	int minEPcus = IntInf;

	AlgorithmParameters* aps = nullptr;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
	BKS* bks = nullptr;
	Timer* timer = nullptr;
	YearTable* yearTable = nullptr;

	struct Position {
		int rIndex = -1;
		int pos = -1;
		DisType pen = DisInf;
		DisType cost = DisInf;
		DisType secDis = DisInf;
		int year = IntInf;
		Position() {}
		Position(int rIndex,int pos,DisType pen = DisInf,DisType cost = DisInf, DisType secDis = DisInf,int year = IntInf)
			:rIndex(rIndex),pos(pos),pen(pen),cost(cost),secDis(secDis),year(year)
		{}
	};

	struct eOneRNode {

		Vec<int> ejeVe;
		int Psum = 0;
		int rId = -1;

		int getMaxEle() {
			int ret = -1;
			for (int i : ejeVe) {
				ret = std::max<int>(ret, i);
			}
			return ret;
		}

		eOneRNode() {
			Psum = IntInf;
			rId = -1;
		}

		eOneRNode(int id) {
			Psum = IntInf;
			rId = id;
		}

		bool reSet() {
			ejeVe.clear();
			Psum = IntInf;
			rId = -1;
			return true;
		}
	};

	//Solver();

	Solver(Input* input,YearTable* yearTable,BKS* bks);

	Solver(const Solver& s);

	Solver& operator = (const Solver& s);

	// route function
	Route rCreateRoute(int id);

	DisType rUpdatePc(Route& r);

	bool rReset(Route& r);

	bool rRemoveAllCustomersInRoute(Route& r);

	bool rInsertAtPosition(Route& r, int pos, int node);

	bool rInsAtPosPre(Route& r, int pos, int node);

	bool rRemoveAtPosition(Route& r, int a);

	void rPrevDisplay(Route& r);

	void rNextDisplay(Route& r);

	bool rUpdateAvQFrom(Route& r, int vv);

	void rUpdateZvQFrom(Route& r, int vv);

	int rGetCustomerNumber(Route& r);

	void rReCalculateRouteCost(Route& r);

	Vec<int> rPutCustomersInVector(Route& r);

	void rReCalCusNumAndSetCusrIdWithHeadrId(Route& r);

	void reCalRtsCostAndPen();

	void reCalRtsCostSumCost();

	CircleSector rGetCircleSector(Route& r);

	void sumRtsPenalty();

	void sumRtsCost();

	DisType updatePenalty(const DeltPen& delt);

	Position findBestPositionInSolution(int w);

	Position findBestPositionInSolutionForInitial(int w);

	bool initSolutionBySecOrder();

	bool initSolutionSortOrder(int kind);

	bool initSolutionMaxRoute();

	//bool loadIndividual(const Individual* indiv);
	//void exportIndividual(Individual* indiv);

	bool initSolution(int kind);

	DeltPen estimatevw(int kind, int v, int w, int oneR);

	inline int getFrontofTwoCus(int v, int w) {
		if (customers[v].Q_X == customers[w].Q_X) {
			for (int pt = v; pt != -1; pt = customers[pt].next) {
				if (pt == w) {
					return v;
				}
			}
			return w;
		}
		else {
			return customers[v].Q_X < customers[w].Q_X ? v : w;
		}
		return -1;
	}
	
	// begin is at front of end in a route
	inline Vec<int> putCustomersInVectorBetweenTwoCus(int customerBegin, int customerEnd) {
		Vec<int> arr;
		for (int i = customerBegin; i != -1; i = customers[i].next) {
			arr.push_back(i);
			if (i == customerEnd) {
				break;
			}
		}
		return arr;
	}

	inline DisType getPtwWithVectorOfCustomers(const Vec<int>& cusv) {

		DisType newPtw = customers[cusv.front()].TW_X + customers[cusv.back()].TWX_;
		DisType lastav = customers[cusv.front()].av;
		for (int i = 1; i < cusv.size(); ++i) {
			int pt = cusv[i], ptpre = cusv[i - 1];
			DisType avp = lastav + input->datas[ptpre].SERVICETIME + input->getDisof2(ptpre, pt);
			newPtw += std::max<DisType>(0, avp - input->datas[pt].DUEDATE);
			lastav = avp > input->datas[pt].DUEDATE ? input->datas[pt].DUEDATE : std::max<DisType>(avp, input->datas[pt].READYTIME);
		}
		return newPtw + std::max<DisType>(0, lastav - customers[cusv.back()].zv);
	}

	//开区间(twbegin，twend) twbegin，twend的各项值都是可靠的，开区间中间的点可以变化 twbegin，twend可以是仓库 
	DisType getaRangeOffPtw(int twbegin, int twend);

	DeltPen _2optOpenvv_(int v, int w);

	DeltPen _2optOpenvvj(int v, int w);

	DeltPen outrelocatevToww_(int v, int w, int oneR);

	DeltPen outrelocatevTowwj(int v, int w, int oneR);

	DeltPen inrelocatevv_(int v, int w, int oneR);

	DeltPen inrelocatevvj(int v, int w, int oneR);

	DeltPen exchangevw(int v, int w, int oneR);

	DeltPen exchangevw_(int v, int w, int oneR);

	DeltPen exchangevwj(int v, int w, int oneR);

	DeltPen exchangevvjw(int v, int w, int oneR);

	DeltPen exchangevwwj(int v, int w, int oneR);

	DeltPen exchangevvjvjjwwj(int v, int w, int oneR);

	DeltPen exchangevvjvjjw(int v, int w, int oneR);

	DeltPen outrelocatevvjTowwj(int v, int w, int oneR);

	DeltPen outrelocatevv_Toww_(int v, int w, int oneR);

	DeltPen reversevw(int v, int w);

	DeltPen _Nopt(Vec<int>& nodes);

	bool doMoves(TwoNodeMove& M);

	bool twoOptStar(TwoNodeMove& M);

	bool outRelocate(TwoNodeMove& M);

	bool inRelocate(TwoNodeMove& M);

	bool exchange(TwoNodeMove& M);

	bool doReverse(TwoNodeMove& M);

	Vec<int> getPtwNodes(Route& r, int ptwKind = 0);

	TwoNodeMove getMovesRandomly(std::function<bool(TwoNodeMove& t, TwoNodeMove& bestM)>updateBestM);

	bool resetConfRts();

	bool resetConfRtsByOneMove(Vec<int> ids);

	bool doEjection(Vec<eOneRNode>& XSet);

	bool EPNodesCanEasilyPut();

	bool managerCusMem(Vec<int>& releaseNodes);

	bool removeOneRouteByRid(int rId = -1);

	DisType verify();

	DeltPen getDeltIfRemoveOneNode(Route& r, int pt);

	bool addWeightToRoute(TwoNodeMove& bestM);

	Vec<eOneRNode> ejeNodesAfterSqueeze;

	bool routeWeightedRepair();

	Position findBestPositionForRuin(int w);

	static Vec<int>ClearEPOrderContribute;

	void ruinClearEP(int kind);

	int ruinGetSplitDepth(int maxDept);

	Vec<int> ruinGetRuinCusBySting(int ruinK, int ruinL);

	Vec<int> ruinGetRuinCusByRound(int ruinCusNum);

	Vec<int> ruinGetRuinCusByRand(int ruinCusNum);

	Vec<int> ruinGetRuinCusByRandOneR(int ruinCusNum);

	Vec<int> ruinGetRuinCusBySec(int ruinCusNum);

	int ruinLocalSearchNotNewR(int ruinCusNum);

	int CVB2ruinLS(int ruinCusNum);

	Position findBestPosToSplit(Route& r);

	int getARouteIdCanUsed();

	int splitLocalSearch();

	int CVB2ClearEPAllowNewR(int kind);

	int simulatedannealing(int kind, int iterMax, double temperature, int ruinNum);

	bool doOneTimeRuinPer(int perturbkind, int ruinCusNum, int clearEPKind);

	bool perturbBasedRuin(int perturbkind, int ruinCusNum, int clearEPKind);

	bool ejectLocalSearch();

	bool patternAdjustment(int Irand = -1);

	void perturbBasedEjectionPool(int ruinCusNum);

	Vec<eOneRNode> ejectFromPatialSol();

	eOneRNode ejectOneRouteOnlyHasPcMinP(Route& r, int Kmax);

	eOneRNode ejectOneRouteOnlyP(Route& r, int kind, int Kmax);

	eOneRNode ejectOneRouteMinPsumGreedy
	(Route& r, eOneRNode cutBranchNode = eOneRNode());

	bool resetSolver();

	bool minimizeRouteNumber(int ourTarget);

	bool adjustRouteNumber(int ourTarget);

	bool repair();

	bool minimizeRouteDistanceLocalSearch(int hasRange, Vec<int> newCus);

	Output saveToOutPut();

	bool printDimacs();

	bool saveOutAsSintefFile(std::string outputPath, std::string opt = "");

	~Solver();

public:
};

struct BKS {

	Solver bestSolFound;
	DisType lastPrCost = DisInf;
	UnorderedMap<int, DisType> bksAtRn;
	Timer::TimePoint lastPrintTp;
	Timer* timer = nullptr;
	BKS(Input* input,YearTable*yearTable,Timer* timer);

	void reset();

	bool updateBKSAndPrint(Solver& newSol, std::string opt = "");

	void resetBksAtRn();
};
}

#endif // !CN_HUST_LYH_SOLVER_H
