
#ifndef CN_HUST_LYH_SOLVER_H
#define CN_HUST_LYH_SOLVER_H

#include <queue>
#include <functional>
#include <set>
#include <limits.h>

#include "Common.h"
#include "Util.h"
#include "Input.h"
#include "YearTable.h"

namespace hust {

struct BKS;

struct Route {

public:

	int head = -1;
	int tail = -1;
	int routeId = -1;
	int rCustCnt = 0; // not include depot
	DisType rQ = 0;


	DisType rPc = 0;
	DisType rPtw = 0;
	int rWeight = 1;
	DisType routeCost = 0;

	Route() {

		routeId = -1;
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

		this->routeId = r.routeId;
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

		this->routeId = rid;
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

			this->routeId = r.routeId;
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

	Vector<Route> ve;
	Vector<int> posOf;
	int cnt = 0;

	RTS() = default;

	RTS(int maxSize) {
		cnt = 0;
		ve.reserve(maxSize);
		for (int i = 0; i < maxSize; ++i) {
			Route r;
			ve.push_back(r);
		}
		posOf = Vector<int>(maxSize, -1);
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
		lyhCheckTrue(r1.routeId >= 0);
#endif // LYH_CHECKING

		if (r1.routeId >= static_cast<int>(posOf.size()) ) {
			size_t newSize = posOf.size() + std::max<size_t>(r1.routeId + 1, posOf.size() / 2 + 1);
			ve.resize(newSize, Route());
			posOf.resize(newSize, -1);
		}

		ve[cnt] = r1;
		posOf[r1.routeId] = cnt;
		++cnt;
		return true;
	}

	bool removeIndex(int index) {

		if (index < 0 || index >= cnt) {
			return false;
		}

		if (index != cnt - 1) {
			int id = ve[index].routeId;

			int cnt_1_id = ve[cnt - 1].routeId;
			posOf[cnt_1_id] = index;
			posOf[id] = -1;
			ve[index] = ve[cnt - 1];
			cnt--;
		}
		else {
			int id = ve[index].routeId;
			posOf[id] = -1;
			cnt--;
		}

		return true;
	}

	Route& getRouteByRouteId(int rid) {

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
			posOf[ve[i].routeId] = -1;
		}
		cnt = 0;
		return true;
	}

};

struct ConfSet {

	Vector<int> ve;
	Vector<int> pos;
	int cnt = 0;

	ConfSet() = default;
	ConfSet(int maxSize) {
		cnt = 0;
		ve = Vector<int>(maxSize + 1, -1);
		pos = Vector<int>(maxSize + 1, -1);
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

		if (val >= static_cast<int>(pos.size())) {
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

	Vector<int>putElementInVector() {
		return Vector<int>(ve.begin(), ve.begin() + cnt);
	}

	int randomPeek(Random* random) {
		if (cnt == 0) {
			Logger::ERROR("container.cnt == 0");
		}
		int index = random->pick(cnt);
		return ve[index];
	}

	bool removeVal(int val) {

		if (val >= static_cast<int>(pos.size()) || val < 0) {
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

struct WeightedEjectPool {

	int sumCost = 0;
	ConfSet container;
	Input* input;
	WeightedEjectPool(Input* input) :container(input->customerNumer + 1), input(input) {}

	WeightedEjectPool(const WeightedEjectPool& ej) {
		this->container = ej.container;
		this->sumCost = ej.sumCost;
		this->input = ej.input;
	}

	WeightedEjectPool& operator = (const WeightedEjectPool& ej) {
		if (this != &ej) {
			this->container = ej.container;
			this->sumCost = ej.sumCost;
			this->input = ej.input;
		}
		return *this;
	}

	~WeightedEjectPool() { }

	void insert(int v) {
		sumCost += input->P[v];
		container.insert(v);
	}

	void remove(int v) {
		if (container.pos[v] == -1) {
			Logger::ERROR("container.pos[v]==-1");
		}
		sumCost -= input->P[v];
		container.removeVal(v);
	}

	int randomPeek() {
		return container.randomPeek(&input->random);
	}

	void reset() {
		container.reset();
		sumCost = 0;
	}

	Vector<int>putElementInVector() {
		return  container.putElementInVector();
	}

	int size() {
		return container.cnt;
	}
};

class YearTable;

struct DeltaPenalty {

	DisType deltPtw = DisInf;
	DisType deltPc = DisInf;
	DisType PcOnly = DisInf;
	DisType PtwOnly = DisInf;
	DisType deltCost = DisInf;

	DeltaPenalty() {

		deltPc = DisInf;
		PcOnly = DisInf;
		PtwOnly = DisInf;
		deltCost = DisInf;
	}
	DeltaPenalty(const DeltaPenalty& d) {
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

	DeltaPenalty deltPen;

	TwoNodeMove(int v, int w, int kind, DeltaPenalty d) :
		v(v), w(w), kind(kind), deltPen(d) {
	}

	TwoNodeMove(Vector<int> ve, int kind, DeltaPenalty d) :
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

struct Customer {
public:

	//int id = -1;
	int prev = -1;
	int next = -1;
	int routeId = -1;

	DisType av = 0;
	DisType zv = 0;

	DisType avp = 0;
	DisType zvp = 0;

	DisType TW_X = 0;
	DisType TWX_ = 0;

	DisType Q_X = 0;
	DisType QX_ = 0;

	Customer() :prev(-1), next(-1), av(0), zv(0), avp(0),
		zvp(0), TW_X(0), TWX_(0), Q_X(0), QX_(0) {}

	bool reset() {
		//id = -1;
		prev = -1;
		next = -1;
		routeId = -1;

		av = 0;
		zv = 0;

		avp = 0;
		zvp = 0;

		TW_X = 0;
		TWX_ = 0;

		Q_X = 0;
		QX_ = 0;

		return true;
	}
};

struct Solver
{
public:

	Vector<Customer> customers;
	RTS rts;
	Input* input = nullptr;
	AlgorithmParameters* aps = nullptr;

	YearTable* yearTable = nullptr;

	BKS* bks = nullptr;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
	Timer* timer = nullptr;
	
	DisType penalty = 0;
	DisType Ptw = 0;
	DisType PtwNoWei = 0;
	DisType Pc = 0;

	DisType alpha = 1;
	DisType beta = 1;
	DisType gamma = 1;

	DisType RoutesCost = DisInf;
	
	ConfSet EP;

	ConfSet PtwConfRts, PcConfRts;

	int minEPcus = IntInf;

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

		Vector<int> ejeVe;
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

	Vector<int> rPutCustomersInVector(Route& r);

	void rReCalCusNumAndSetCusrIdWithHeadrId(Route& r);

	void reCalRtsCostAndPen();

	void reCalRtsCostSumCost();

	CircleSector rGetCircleSector(Route& r);

	void sumRoutesPenalty();

	void sumRoutesCost();

	DisType updatePenalty(const DeltaPenalty& delt);

	Position findBestPositionInSolution(int w);

	Position findBestPositionInSolutionForInitial(int w);

	bool initSolutionBySecOrder();

	bool initSolutionSortOrder(int kind);

	bool initSolutionMaxRoute();

	//bool loadIndividual(const Individual* indiv);
	//void exportIndividual(Individual* indiv);

	bool initSolution(int kind);

	DeltaPenalty estimatevw(int kind, int v, int w, int oneR);

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
	inline Vector<int> putCustomersInVectorBetweenTwoCus(int customerBegin, int customerEnd) {
		Vector<int> arr;
		for (int i = customerBegin; i != -1; i = customers[i].next) {
			arr.push_back(i);
			if (i == customerEnd) {
				break;
			}
		}
		return arr;
	}

	inline DisType getPtwWithVectorOfCustomers(const Vector<int>& cusv) {

		DisType newPtw = customers[cusv.front()].TW_X + customers[cusv.back()].TWX_;
		DisType lastav = customers[cusv.front()].av;
		for (int i = 1; i < static_cast<int>(cusv.size()); ++i) {
			int pt = cusv[i], ptpre = cusv[i - 1];
			DisType avp = lastav + input->datas[ptpre].SERVICETIME + input->getDisof2(ptpre, pt);
			newPtw += std::max<DisType>(0, avp - input->datas[pt].DUEDATE);
			lastav = avp > input->datas[pt].DUEDATE ? input->datas[pt].DUEDATE : std::max<DisType>(avp, input->datas[pt].READYTIME);
		}
		return newPtw + std::max<DisType>(0, lastav - customers[cusv.back()].zv);
	}

	//开区间(twbegin，twend) twbegin，twend的各项值都是可靠的，开区间中间的点可以变化 twbegin，twend可以是仓库 
	DisType getaRangeOffPtw(int twbegin, int twend);

	DeltaPenalty twoOptStarOpenvv_(int v, int w);

	DeltaPenalty twoOptStarOpenvvj(int v, int w);

	DeltaPenalty outOnevToww_(int v, int w, int oneR);

	DeltaPenalty outOnevTowwj(int v, int w, int oneR);

	DeltaPenalty inOnevv_(int v, int w, int oneR);

	DeltaPenalty inOnevvj(int v, int w, int oneR);

	DeltaPenalty swapOneOnevw(int v, int w, int oneR);

	DeltaPenalty swapOneOnevw_(int v, int w, int oneR);

	DeltaPenalty swapOneOnevwj(int v, int w, int oneR);

	DeltaPenalty swapTwoOnevvjw(int v, int w, int oneR);

	DeltaPenalty swapOneTwovwwj(int v, int w, int oneR);

	DeltaPenalty swapThreeTwovvjvjjwwj(int v, int w, int oneR);

	DeltaPenalty swapThreeOnevvjvjjw(int v, int w, int oneR);

	DeltaPenalty outTwovvjTowwj(int v, int w, int oneR);

	DeltaPenalty outTwovv_Toww_(int v, int w, int oneR);

	DeltaPenalty reversevw(int v, int w);

	DeltaPenalty nOptStar(Vector<int>& nodes);

	bool doMoves(TwoNodeMove& M);

	bool doTwoOptStar(TwoNodeMove& M);

	bool doOutRelocate(TwoNodeMove& M);

	bool doInRelocate(TwoNodeMove& M);

	bool doExchange(TwoNodeMove& M);

	bool doReverse(TwoNodeMove& M);

	Vector<int> getPtwNodes(Route& r, int ptwKind = 0);

	TwoNodeMove getMovesRandomly(std::function<bool(TwoNodeMove& t, TwoNodeMove& bestM)>updateBestM);

	bool resetConfRts();

	bool resetConfRtsByOneMove(Vector<int> ids);

	void doEjection(Vector<eOneRNode>& XSet);

	void simpleClearEP();

	bool managerCusMem(Vector<int>& releaseNodes);

	bool removeOneRouteByRouteId(int rId = -1);

	DisType verify();

	DeltaPenalty getDeltIfRemoveOneNode(Route& r, int pt);

	bool addWeightToRoute(TwoNodeMove& bestM);

	Vector<eOneRNode> ejeNodesAfterSqueeze;

	bool routeWeightedRepair();

	Position findBestPositionForRuin(int w);

	static Vector<int>ClearEPOrderContribute;

	void ruinClearEP(int kind);

	int ruinGetSplitDepth(int maxDept);

	Vector<int> ruinGetRuinCusBySting(int ruinK, int ruinL);

	Vector<int> ruinGetRuinCusByRound(int ruinCusNum);

	Vector<int> ruinGetRuinCusByRand(int ruinCusNum);

	Vector<int> ruinGetRuinCusByRandOneR(int ruinCusNum);

	Vector<int> ruinGetRuinCusBySec(int ruinCusNum);

	int ruinLocalSearchNotNewR(int ruinCusNum);

	int CVB2ruinLS(int ruinCusNum);

	Position findBestPosToSplit(Route& r);

	int getARouteIdCanUsed();

	int splitLocalSearch();

	void reInsertCustomersIntoSolution(int kind);

	void ruin(int kind, int iterMax, double temperature, int ruinNum);

	inline DisType  getDeltDistanceCostIfRemoveCustomer(int v) {
		DisType delta = 0;
		int prev = customers[v].prev;
		int next = customers[v].next;
		delta -= input->getDisof2(prev, v);
		delta -= input->getDisof2(v, next);
		delta += input->getDisof2(prev, next);
		return delta;
	}

	Vector<int> getRuinCustomers(int perturbkind, int ruinCusNum);

	bool doOneTimeRuinPer(int perturbkind, int ruinCusNum, int clearEPKind);

	bool perturbBasedRuin(int perturbkind, int ruinCusNum, int clearEPKind);

	bool AWLS();

	void perturb(int Irand = -1);

	void perturbBasedEjectionPool(int ruinCusNum);

	Vector<eOneRNode> ejectFromPatialSol();

	eOneRNode ejectOneRouteOnlyHasPcMinP(Route& r, int Kmax);

	eOneRNode ejectOneRouteOnlyP(Route& r, int kind, int Kmax);

	eOneRNode ejectOneRouteMinPsumGreedy
	(Route& r, eOneRNode cutBranchNode = eOneRNode());

	bool resetSolver();

	bool minimizeRouteNumber(int ourTarget);

	bool adjustRouteNumber(int ourTarget);

	bool repair();

	bool simpleLocalSearch(int hasRange, Vector<int> newCus);

	bool printDimacs();

	~Solver();

public:
};

struct BKS {

	Solver bestSolFound;
	DisType lastPrCost = DisInf;
	UnorderedMap<int, DisType> bksAtRn;
	Timer::TimePoint lastPrintTp;
	Timer* timer = nullptr;
	BKS(Input* input);

	void reset();

	bool updateBKSAndPrint(Solver& newSol, String opt = "");

	void resetBksAtRn();
};
}

#endif // !CN_HUST_LYH_SOLVER_H
