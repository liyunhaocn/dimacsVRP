#ifndef vrptwNew_SOLVER_H
#define vrptwNew_SOLVER_H

#include <queue>
#include <functional>
#include <set>
#include <limits.h>

#include "./Environment.h"
#include "./Utility.h"
#include "./Problem.h"
#include "./Flag.h"

namespace hust {

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

		this->routeID = r.routeID;
		this->rCustCnt = r.rCustCnt;
		this->rQ = r.rQ;
		this->head = r.head;
		this->tail = r.tail;

		this->rPc = r.rPc;
		this->rPtw = r.rPtw;
		this->rWeight = r.rWeight;
		this->routeCost = r.routeCost;
		return *this;
	}

};

struct RTS {

	Vec<Route> ve;
	Vec<int> posOf;
	int cnt = 0;

	int size() {
		return cnt;
	}

	//RTS() {}
		
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
		cnt = r.cnt;
		ve = r.ve;
		posOf = r.posOf;
		return *this;
	}

	~RTS() {
		//debug("~RTS")
		ve.clear();
		posOf.clear();
	}

	Route& operator [](int index) {

#if CHECKING
		lyhCheckTrue(index < ve.size());
		lyhCheckTrue(index >= 0);
#endif // CHECKING

		return ve[index];
	}

	bool push_back(Route& r1) {

		ve[cnt] = r1;
		posOf[r1.routeID] = cnt;
		++cnt;
		return true;
	}

	bool removeIndex(int index) {

		if (index < 0 || index >= cnt) {
			return false;
		}

		int id = ve[index].routeID;

		int cnt_1_id = ve[cnt - 1].routeID;
		posOf[cnt_1_id] = index;
		posOf[id] = -1;
		ve[index] = ve[cnt - 1];
		cnt--;
		return true;
	}

	bool removeRouteId(int rid) {

#if CHECKING
		lyhCheckTrue(rid >=0);
		lyhCheckTrue(rid < posOf.size());
		lyhCheckTrue(posOf[rid] >=0 );
#endif // CHECKING

		removeIndex(posOf[rid]);
		return true;
	}

	Route& getRouteByRid(int rid) {

#if CHECKING
		lyhCheckTrue(rid >= 0);
		lyhCheckTrue(rid < posOf.size());
		lyhCheckTrue(posOf[rid] >= 0);
#endif // CHECKING

		int index = posOf[rid];

#if CHECKING
		lyhCheckTrue(index >= 0);
		lyhCheckTrue(index < cnt);
#endif // CHECKING

		return ve[index];
	}

	bool reSet() {
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
		cnt = cs.cnt;
		ve = cs.ve;
		pos = cs.pos;
		return *this;
	}

	ConfSet(ConfSet&& cs) noexcept {
		cnt = std::move(cs.cnt);
		ve = std::move(cs.ve);
		pos = std::move(cs.pos);
	}
		
	ConfSet& operator = (ConfSet&& cs) noexcept {
		cnt = std::move(cs.cnt);
		ve = std::move(cs.ve);
		pos = std::move(cs.pos);
		return *this;
	}

	bool reSet() {

		for (int i = 0; i < cnt; ++i) {
			// 
			pos[ve[i]] = -1;
			ve[i] = -1;
		}

		/*for (int i = 0; i < pos.size(); ++i) {
			pos[i] = -1;
		}*/

		cnt = 0;
		return true;
	}

	bool ins(int val) {

		if (pos[val] >= 0) {
			return false;
		}

		ve[cnt] = val;
		pos[val] = cnt;
		++cnt;
		return true;
	}

	int posAt(int val) {

		if (val < 0 && val >= static_cast<int>(pos.size())) {
			return -1;
		}
		return pos[val];
	}

	bool removeVal(int val) {

		int index = pos[val];
		if (index == -1 || cnt <= 0) {
			return false;
		}

		pos[ve[cnt - 1]] = index;
		pos[val] = -1;

		ve[index] = ve[cnt - 1];
		cnt--;
		return true;
	}

};

struct RandomX {

public:

	using Generator = std::mt19937;

	RandomX(unsigned seed) : rgen(seed) { initMpLLArr(); }
	RandomX() : rgen(generateSeed()) { initMpLLArr(); }

	RandomX(const RandomX& rhs) {
		this->mpLLArr = rhs.mpLLArr;
		this->maxRange = rhs.maxRange;
		this->rgen = rhs.rgen;
	}

	Vec< Vec<int> > mpLLArr;

	int maxRange = 1001;

	bool initMpLLArr() {
		mpLLArr = Vec< Vec<int> >(maxRange);

		for (int m = 1; m < maxRange;++m) {
			mpLLArr[m] = Vec<int>(m, 0);
			auto& arr = mpLLArr[m];
			std::iota(arr.begin(), arr.end(), 0);
		}
		return true;
	}

	static unsigned generateSeed() {
		unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
		return seed;
	}

	Generator::result_type operator()() { return rgen(); }

	// pick with probability of (numerator / denominator).
	bool isPicked(unsigned numerator, unsigned denominator) {
		return ((rgen() % denominator) < numerator);
	}

	// pick from [min, max).
	int pick(int min, int max) {
		return ((rgen() % (max - min)) + min);
	}
	// pick from [0, max).
	int pick(int max) {
		return (rgen() % max);
	}

	Vec<int>& getMN(int M, int N) {

		if (M >= maxRange) {
			mpLLArr.resize(M * 2 + 1);
			maxRange = M * 2 + 1;
		}

		Vec<int>& ve = mpLLArr[M];

		if (ve.empty()) {
			mpLLArr[M] = Vec<int>(M, 0);
			auto& arr = mpLLArr[M];
			std::iota(arr.begin(),arr.end(),0);
		}

		for (int i = 0; i < N; ++i) {
			int index = pick(i, M);
			std::swap(ve[i], ve[index]);
		}
		return ve;
	}

	RandomX& operator = (RandomX&& rhs) noexcept {
		this->mpLLArr = std::move(rhs.mpLLArr);
		this->maxRange = rhs.maxRange;
		this->rgen = rhs.rgen;
		return *this;
	}

	RandomX& operator = (const RandomX& rhs) {
		this->mpLLArr = rhs.mpLLArr;
		this->maxRange = rhs.maxRange;
		this->rgen = rhs.rgen;
		return *this;
	}

	Generator rgen;
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

struct SolClone
{
	Vec<Customer> customers;
	RTS rts;
	DisType Pc = 0;
	DisType Ptw = 0;
	DisType PtwNoWei = 0;
	DisType penalty = 0;
	DisType RoutesCost = 0;
	Route EPr;

	SolClone(Vec<Customer>& customers,
		RTS rts,
		Route EPr,
		DisType Pc,
		DisType Ptw,
		DisType PtwNoWei,
		DisType penalty,
		DisType RoutesCost
	) :
		customers(customers),
		rts(rts),
		Pc(Pc),
		Ptw(Ptw),
		PtwNoWei(PtwNoWei),
		penalty(penalty),
		RoutesCost(RoutesCost)
		, EPr(EPr) {
	}

	SolClone& operator = (const SolClone& s) {

		this->customers = s.customers;
		this->rts = s.rts;
		this->Pc = s.Pc;
		this->Ptw = s.Ptw;
		this->PtwNoWei = s.PtwNoWei;
		this->penalty = s.penalty;
		this->RoutesCost = s.RoutesCost;
		this->EPr = s.EPr;

		return *this;
	}
};

class Solver
{
public:

	Input& input;
	//Output output;
	Environment env;

	Vec<Customer> customers;

	int ourTarget = 0;
	DisType penalty = 0;
	DisType Ptw = 0;
	DisType PtwNoWei = 0;
	DisType Pc = 0;

	DisType alpha = 1;
	DisType beta = 1;
	DisType gamma = 0;

	DisType RoutesCost = 0;

	RTS rts;

	ConfSet PtwConfRts, PcConfRts;

	Vec<LL> P;

	LL EPIter = 1;
	int minEPcus = IntInf;

	Route EPr;

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
		Vec<int> ve;
		int kind = -1;

		DeltPen deltPen;

		TwoNodeMove(int v, int w, int kind, DeltPen d) :
			v(v), w(w), kind(kind), deltPen(d) {
		}

		TwoNodeMove(Vec<int> ve, int kind, DeltPen d) :
			kind(kind), deltPen(d) {
			this->ve = ve;
		}

		TwoNodeMove() :
			v(0), w(0), kind(0) {
			deltPen.deltPc = DisInf;
			deltPen.deltPtw = DisInf;
			deltPen.PtwOnly = DisInf;
			deltPen.PcOnly = DisInf;
			ve.clear();
		}

		bool reSet() {
			v = 0;
			w = 0;
			kind = 0;
			deltPen.deltPc = DisInf;
			deltPen.deltPtw = DisInf;
			deltPen.PtwOnly = DisInf;
			deltPen.PcOnly = DisInf;
			deltPen.deltCost = DisInf;
			ve.clear();
			return true;
		}
	};

	struct Position {
		int rIndex =-1;
		int pos = -1;
		DisType pen = DisInf;
		DisType cost = DisInf;
		DisType secDis = DisInf;
		LL year = LLInf;
	};

	struct eOneRNode {

		Vec<int> ejeVe;
		int Psum = 0;
		int rId = -1;

		int getMaxEle() {
			int ret = -1;
			for (int i : ejeVe) {
				ret = std::max<int>(ret,i);
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

	bool initEPr() {

		Route& r = EPr;

		r.head = input.custCnt + 1;
		r.tail = input.custCnt + 2;

		customers[r.head].next = r.tail;
		customers[r.tail].pre = r.head;

		customers[r.head].routeID = -1;
		customers[r.tail].routeID = -1;

		/*customers[r.head].av = customers[r.head].avp
			= input.datas[r.head].READYTIME;
		customers[r.head].QX_ = 0;
		customers[r.head].Q_X = 0;
		customers[r.head].TWX_ = 0;
		customers[r.head].TW_X = 0;

		customers[r.tail].zv = customers[r.tail].zvp
			= input.datas[r.tail].DUEDATE;
		customers[r.tail].QX_ = 0;
		customers[r.tail].Q_X = 0;
		customers[r.tail].TWX_ = 0;
		customers[r.tail].TW_X = 0;*/

		return true;
	}

	inline int reCusNo(int x) {
		return x <= input.custCnt ? x : 0;
	} 

	Solver(Input& input, Environment& env) :
		input(input), env(env), 
		lyhTimer(cfg->runTimer),
		PtwConfRts(input.custCnt), 
		PcConfRts(input.custCnt), 
		rts(input.custCnt)
	{

		P = Vec<LL>(input.custCnt + 1, 1);
		customers = Vec<Customer>(input.custCnt *3+3);
		alpha = 1;
		beta = 1;
		gamma = 1;
		squIter = 0;
		penalty = 0;
		Ptw = 0;
		PtwNoWei = 0;
		Pc = 0;
		//minEPSize = inf;
		RoutesCost = 0;
		initEPr();
		//println("Solver construct");
	}

	Solver(const Solver& s) :
		input(s.input), 
		env(s.env),
		lyhTimer(s.lyhTimer), 
		PtwConfRts(s.PtwConfRts),
		PcConfRts(s.PcConfRts) ,
		rts(s.rts)
	{

		//this->output = s.output;
		this->P = s.P;
		this->customers = s.customers;
		this->penalty = s.penalty;
		this->Ptw = s.Ptw;
		this->PtwNoWei = s.PtwNoWei;
		this->Pc = s.Pc;
		this->alpha = s.alpha;
		this->beta = s.beta;
		this->gamma = s.gamma;
		this->EPr = s.EPr;
		this->EPIter = s.EPIter;
		this->ourTarget = s.ourTarget;
		this->RoutesCost = s.RoutesCost;
	}

	Solver& operator = (const Solver& s) {

		this->P = s.P;
		this->customers = s.customers;

		this->rts = s.rts;
		this->PtwConfRts = s.PtwConfRts;
		this->PcConfRts = s.PcConfRts;
		this->penalty = s.penalty;
		this->Ptw = s.Ptw;
		this->PtwNoWei = s.PtwNoWei;
		this->alpha = s.alpha;
		this->beta = s.beta;
		this->gamma = s.gamma;
		this->Pc = s.Pc;
		this->RoutesCost = s.RoutesCost;
		this->EPr = s.EPr;
		this->EPIter = s.EPIter;
		this->ourTarget = s.ourTarget;

		return *this;
	}

	// route function
	Route rCreateRoute(int id) {

		Route r(id);
		int index = (rts.cnt + 1) * 2 + input.custCnt + 1;

		r.head = index;
		r.tail = index + 1;

		customers[r.head].next = r.tail;
		customers[r.tail].pre = r.head;

		customers[r.head].routeID = id;
		customers[r.tail].routeID = id;

		customers[r.head].av = customers[r.head].avp
			= input.datas[r.head].READYTIME;
		customers[r.head].QX_ = 0;
		customers[r.head].Q_X = 0;
		customers[r.head].TWX_ = 0;
		customers[r.head].TW_X = 0;

		customers[r.tail].zv = customers[r.tail].zvp
			= input.datas[r.tail].DUEDATE;
		customers[r.tail].QX_ = 0;
		customers[r.tail].Q_X = 0;
		customers[r.tail].TWX_ = 0;
		customers[r.tail].TW_X = 0;

		return r;

	}

	DisType rUpdatePc(Route& r) {

		int pt = r.head;
		r.rQ = 0;
		while (pt != -1) {
			r.rQ += input.datas[pt].DEMAND;
			pt = customers[pt].next;
		}
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);
		return r.rPc;
	}

	bool rReset(Route& r) {

		r.rCustCnt = 0; //没有计算仓库
		//r.routeID = -1;
		r.routeCost = 0;
		r.rPc = 0;
		r.rPtw = 0;
		r.rWeight = 1;

		int pt = r.head;

		while (pt != -1) {

			int ptnext = customers[pt].next;
			customers[pt].reSet();
			pt = ptnext;
		}

		r.head = -1;
		r.tail = -1;

		return true;
	}

	bool rRemoveAllCusInR(Route& r) {

		r.routeCost = 0;
		r.rWeight = 1;

		Vec<int> ve = rPutCusInve(r);
		for (int pt : ve) {
			rRemoveAtPos(r, pt);
		}

		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);

		return true;
	}

	bool rInsAtPos(Route& r, int pos, int node) {

		customers[node].routeID = r.routeID;
		r.rQ += input.datas[node].DEMAND;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);

		int anext = customers[pos].next;
		customers[node].next = anext;
		customers[node].pre = pos;
		customers[pos].next = node;
		customers[anext].pre = node;

		++r.rCustCnt;

		return true;
	}

	bool rInsAtPosPre(Route& r, int pos, int node) {

		customers[node].routeID = r.routeID;
		r.rQ += input.datas[node].DEMAND;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);

		int apre = customers[pos].pre;
		customers[node].pre = apre;
		customers[node].next = pos;
		customers[pos].pre = node;
		customers[apre].next = node;
		++r.rCustCnt;

		return true;
	}

	bool rRemoveAtPos(Route& r, int a) {

		if (r.rCustCnt <= 0) {
			return false;
		}

		r.rQ -= input.datas[a].DEMAND;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);

		Customer& temp = customers[a];
		Customer& tpre = customers[temp.pre];
		Customer& tnext = customers[temp.next];

		tnext.pre = temp.pre;
		tpre.next = temp.next;

		temp.next = -1;
		temp.pre = -1;
		temp.routeID = -1;
		r.rCustCnt--;
		return true;
	}

	void rPreDisp(Route& r) {

		int pt = r.tail;
		std::cout << "predisp: ";
		while (pt != -1) {
			std::cout << pt << " ";
			std::cout << ",";
			pt = customers[pt].pre;
		}
		std::cout << std::endl;

	}

	void rNextDisp(Route& r) {

		int pt = r.head;

		std::cout << "nextdisp: ";
		while (pt != -1) {
			//out( mem[pt].val );
			std::cout << pt << " ";
			std::cout << ",";
			pt = customers[pt].next;
		}
		std::cout << std::endl;
	}

	DisType rUpdateAvfrom(Route& r, int vv) {

		int v = vv;
		if (v == r.head) {
			v = customers[v].next;
		}
		int pt = v;
		int ptpre = customers[pt].pre;

		while (pt != -1) {

			customers[pt].avp =
				customers[pt].av = customers[ptpre].av + input.disOf[reCusNo(ptpre)][reCusNo(pt)] + input.datas[ptpre].SERVICETIME;

			customers[pt].TW_X = customers[ptpre].TW_X;
			customers[pt].TW_X +=std::max<DisType>(customers[pt].avp - input.datas[pt].DUEDATE, 0);

			if (customers[pt].avp <= input.datas[pt].DUEDATE) {
				customers[pt].av =std::max<DisType>(customers[pt].avp, input.datas[pt].READYTIME);
			}
			else {
				customers[pt].av = input.datas[pt].DUEDATE;
			}

			ptpre = pt;
			pt = customers[pt].next;
		}

		r.rPtw = customers[r.tail].TW_X;
		return r.rPtw;
	}

	DisType rUpdateAQfrom(Route& r, int v) {

		if (v == r.head) {
			v = customers[v].next;
		}

		int pt = v;
		int ptpre = customers[pt].pre;
		while (pt != -1) {
			customers[pt].Q_X = customers[ptpre].Q_X + input.datas[pt].DEMAND;
			ptpre = pt;
			pt = customers[pt].next;
		}

		r.rQ = customers[r.tail].Q_X;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);
		return r.rPc;
	}

	bool rUpdateAvQfrom(Route& r, int vv) {

		int v = vv;
		if (v == r.head) {
			v = customers[v].next;
		}
		int pt = v;
		int ptpre = customers[pt].pre;

		while (pt != -1) {

			customers[pt].avp =
				customers[pt].av = customers[ptpre].av + input.disOf[reCusNo(ptpre)][reCusNo(pt)] + input.datas[ptpre].SERVICETIME;

			customers[pt].TW_X = customers[ptpre].TW_X;
			customers[pt].TW_X +=std::max<DisType>(customers[pt].avp - input.datas[pt].DUEDATE, 0);

			if (customers[pt].avp <= input.datas[pt].DUEDATE) {
				customers[pt].av =std::max<DisType>(customers[pt].avp, input.datas[pt].READYTIME);
			}
			else {
				customers[pt].av = input.datas[pt].DUEDATE;
			}

			customers[pt].Q_X = customers[ptpre].Q_X + input.datas[pt].DEMAND;

			ptpre = pt;
			pt = customers[pt].next;
		}

		r.rPtw = customers[r.tail].TW_X;
		r.rQ = customers[r.tail].Q_X;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);

		return true;
	}

	DisType rUpdateZvfrom(Route& r, int vv) {

		int v = vv;

		if (v == r.tail) {
			v = customers[v].pre;
		}

		int pt = v;
		int ptnext = customers[pt].next;

		while (pt != -1) {

			customers[pt].zvp =
				customers[pt].zv = customers[ptnext].zv - input.disOf[reCusNo(pt)][reCusNo(ptnext)] - input.datas[pt].SERVICETIME;

			customers[pt].TWX_ = customers[ptnext].TWX_;

			customers[pt].TWX_ +=std::max<DisType>(input.datas[pt].READYTIME - customers[pt].zvp, 0);

			customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
				std::min<DisType>(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;

			ptnext = pt;
			pt = customers[pt].pre;
		}
		r.rPtw = customers[r.head].TWX_;
		return r.rPtw;
	}

	DisType rUpdateZQfrom(Route& r, int v) {

		if (v == r.tail) {
			v = customers[v].pre;
		}
		int pt = v;
		int ptnext = customers[pt].next;
		while (pt != -1) {
			customers[pt].QX_ = customers[ptnext].QX_ + input.datas[pt].DEMAND;
			ptnext = pt;
			pt = customers[pt].pre;
		}

		r.rQ = customers[r.head].QX_;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);
		return r.rPc;
	}

	bool rUpdateZvQfrom(Route& r, int vv) {

		int v = vv;

		if (v == r.tail) {
			v = customers[v].pre;
		}

		int pt = v;
		int ptnext = customers[pt].next;

		while (pt != -1) {

			customers[pt].zvp =
				customers[pt].zv = customers[ptnext].zv - input.disOf[reCusNo(pt)][reCusNo(ptnext)] - input.datas[pt].SERVICETIME;

			customers[pt].TWX_ = customers[ptnext].TWX_;

			customers[pt].TWX_ +=std::max<DisType>(input.datas[pt].READYTIME - customers[pt].zvp, 0);

			customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
				std::min<DisType>(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;
				
			customers[pt].QX_ = customers[ptnext].QX_ + input.datas[pt].DEMAND;
			ptnext = pt;
			pt = customers[pt].pre;
		}
		r.rPtw = customers[r.head].TWX_;
		r.rQ = customers[r.head].QX_;
		r.rPc =std::max<DisType>(0, r.rQ - input.Q);
		return true;
	}

	int rGetCusCnt(Route& r) {

		int pt = customers[r.head].next;
		int ret = 0;
		while (pt <= input.custCnt) {
			++ret;
			pt = customers[pt].next;
		}
		return ret;
	}

	void rReCalRCost(Route& r) {
		int pt = r.head;
		int ptnext = customers[pt].next;
		r.routeCost = 0;
		while (pt != -1 && ptnext != -1) {
			r.routeCost += input.disOf[reCusNo(pt)][reCusNo(ptnext)];
			pt = ptnext;
			ptnext = customers[ptnext].next;
		}
	}

	Vec<int> rPutCusInve(Route& r) {

		Vec<int> ret;
		if (r.rCustCnt > 0) {
			ret.reserve(r.rCustCnt);
		}

		int pt = customers[r.head].next;

		while (pt <= input.custCnt) {
			ret.push_back(pt);
			pt = customers[pt].next;
		}

		return ret;
	}

	bool rtsCheck() {

		for (int i = 0; i < rts.cnt; ++i) {
			Route& r = rts[i];

			Vec<int> cus1;
			Vec<int> cus2;

			int pt = r.head;
			while (pt != -1) {
					
				if (customers[pt].routeID != r.routeID) {
					println("customers[pt].routeID != r.routeID", pt, r.routeID);
				}
				cus1.push_back(pt);
				pt = customers[pt].next;
			}

			pt = r.tail;
			while (pt != -1) {
				cus2.push_back(pt);
				pt = customers[pt].pre;
			}

			if (cus1.size() != cus2.size()) {
				debug(cus1.size() != cus2.size());
			}

			for (int i = 0; i < cus1.size(); ++i) {

				if (cus1[i] != cus2[cus1.size() - 1 - i]) {
					debug(cus1[i] != cus2[cus1.size() - 1 - i]);
				}
			}

			if (r.rCustCnt != cus1.size() - 2) {
				debug(r.rCustCnt);
				debug(cus1.size());
				rNextDisp(r);
				rNextDisp(r);
			}

		}
		return true;
	}

	bool rReCalCusNum(Route& r) {

		int pt = r.head;
		r.rCustCnt = 0;
		while (pt != -1) {
			customers[pt].routeID = r.routeID;
			r.tail = pt;
			if (pt <= input.custCnt) {
				++r.rCustCnt;
			}
			pt = customers[pt].next;
		}

		return true;
	}

	bool reCalRtsCostAndPen() {

		Pc = 0;
		Ptw = 0;
		PtwNoWei = 0;
		RoutesCost = 0;

		for (int i = 0; i < rts.cnt; ++i) {
			Route& r = rts[i];
			rReCalCusNum(r);
			rUpdateAvQfrom(r,r.head);
			rUpdateZvQfrom(r,r.tail);
			rReCalRCost(r);

			PtwNoWei += r.rPtw;
			Ptw += r.rWeight * r.rPtw;
			Pc += r.rPc; 
			RoutesCost += r.routeCost;
		}
		penalty = alpha * Ptw + beta * Pc;

		return true;
	}

	void sumRtsPen() {

		Pc = 0;
		Ptw = 0;
		PtwNoWei = 0;

		for (int i = 0; i < rts.size(); ++i) {

			Route& r = rts[i];
			PtwNoWei += r.rPtw;
			Ptw += r.rWeight * r.rPtw;
			Pc += r.rPc;
		}
		penalty = alpha * Ptw + beta * Pc;
	}
		
	void sumRtsCost() {
		RoutesCost = 0;
		for (int i = 0; i < rts.cnt; ++i) {
			RoutesCost += rts[i].routeCost ;
		}
	}

	DisType updatePen(const DeltPen& delt) {

		Pc += delt.PcOnly;
		Ptw += delt.deltPtw;
		PtwNoWei += delt.PtwOnly;
		penalty = alpha * Ptw + beta * Pc;
		return penalty;
	}

	void reCalRtsCostSumCost() {

		RoutesCost = 0;
		for (int i = 0; i < rts.size(); ++i) {
			rReCalRCost(rts[i]);
			RoutesCost += rts[i].routeCost;
		}
	}
		
	Position findBestPosInSol(int w) {

		Vec<Position> posPool = { Position(),Position(),Position(),Position(), };
			
		// 惩罚最大的排在最前面
		auto cmp = [&](const Position a, const Position& b) {
			if (a.pen == b.pen) {
				return  a.year < b.year;
			}
			else {
				return a.pen < b.pen;
			}
		};

		std::priority_queue<Position,Vec<Position>,decltype(cmp)> qu(cmp);
		qu.push(Position());

		auto updatePool = [&](Position& pos) {
				
			if (pos.pen <= qu.top().pen) {
				qu.push(pos);
				if (qu.size() < 64) {
					;
				}
				else {
					qu.pop();
				}
			}
		};

		for (int i = 0; i < rts.size(); ++i) {
			//debug(i)
			Route& rt = rts[i];

			int v = rt.head;
			int vj = customers[v].next;

			DisType oldrPc = rts[i].rPc;
			DisType rPc =std::max<DisType>(0, rt.rQ + input.datas[w].DEMAND - input.Q);
			rPc = rPc - oldrPc;

			while (v != -1 && vj != -1) {

				DisType oldrPtw = rts[i].rPtw;
					
				DisType rPtw = customers[v].TW_X;
				rPtw += customers[vj].TWX_;

				DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
				rPtw +=std::max<DisType>(0, awp - input.datas[w].DUEDATE);
				DisType aw =
					awp <= input.datas[w].DUEDATE ?std::max<DisType>(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

				DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
				rPtw +=std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
				DisType avj =
					avjp <= input.datas[vj].DUEDATE ?std::max<DisType>(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
				rPtw +=std::max<DisType>(0, avj - customers[vj].zv);

				rPtw = rPtw - oldrPtw;
					
				DisType cost = input.disOf[reCusNo(w)][reCusNo(v)]
					+ input.disOf[reCusNo(w)][reCusNo(vj)]
					- input.disOf[reCusNo(vj)][reCusNo(v)];

				int vre = v > input.custCnt ? 0 : v;
				int vjre = vj > input.custCnt ? 0 : vj;

				LL year = (*yearTable)[vre][w] + (*yearTable)[w][vjre];
					
				Position pos;
				pos.rIndex = i;
				pos.cost = cost;
				pos.pen = rPtw + rPc;
				pos.pos = v;
				pos.year = year;
				pos.secDis = abs(input.datas[w].polarAngle- input.datas[v].polarAngle);
				updatePool(pos);

				v = vj;
				vj = customers[vj].next;
			}
		}

		int p0cnt = 1;
		int p1cnt = 1;
		int p2cnt = 1;
		int p3cnt = 1;
		auto q = qu;

		/*while (!q.empty()) {
			Position pos = q.top();
			q.pop();
			deOut(pos.pen)
		}
		cout << endl;*/

		while (!qu.empty()){

			Position pos = qu.top();
			qu.pop();

			if (pos.pen == DisInf) {
				continue;
			}

			if (pos.pen < posPool[0].pen) {
				p0cnt = 1;
				posPool[0] = pos;
			}
			else if (pos.pen == posPool[0].pen) {
				++p0cnt;
				if (myRand->pick(p0cnt)==0) {
					posPool[0] = pos;
				}
			}
				
			if (pos.year < posPool[1].year) {
				p1cnt = 1;
				posPool[1] = pos;
			}
			else if (pos.year == posPool[1].year) {
				++p1cnt;
				if (myRand->pick(p1cnt)==0) {
					posPool[1] = pos;
				}
			}
				
			if (pos.cost < posPool[2].cost) {
				p2cnt = 1;
				posPool[2] = pos;
			}
			else if (pos.cost == posPool[2].cost) {
				++p2cnt;
				if (myRand->pick(p2cnt)==0) {
					posPool[2] = pos;
				}
			}

			if (posPool[3].rIndex == -1) {
				posPool[3] = pos;
			}
			else {
				Route& rpos = rts[pos.rIndex];
				Route& rpool = rts[posPool[3].rIndex];

				if (rpos.rCustCnt > rpool.rCustCnt) {
					p3cnt = 1;
					posPool[3] = pos;
				}
				else if (rpos.rCustCnt == rpool.rCustCnt) {
					++p3cnt;
					if (myRand->pick(p3cnt) == 0) {
						posPool[3] = pos;
					}
				}
			}
		}
			
		if (posPool[0].pen > 0) {
			int index = myRand->pick(posPool.size());
			return posPool[index];
		}
		else {
			int cnt = 0;
			Position retP;
			for (int i = 0; i < posPool.size(); ++i) {
				if (posPool[i].pen <= 0) {
					++cnt;
					if (myRand->pick(cnt) == 0) {
						retP = posPool[i];
					}
				}
			}
			return retP;
		}
		return Position();
			
	}

	Position findBestPosInSolForInit(int w) {

		auto cmp = [=](const Position& a, const Position& b) ->bool {
			if (a.pen == b.pen) {
				return a.cost < a.cost;
			}
			return a.pen < b.pen;
		};

		std::priority_queue<Position,Vec<Position>,decltype(cmp)> qu(cmp);
		int quMax = cfg->initFindPosPqSize;

		auto quPush = [&qu,&quMax](Position& p) {
			qu.push(p);
			if (qu.size() > quMax) {
				qu.pop();
			}
		};

		for (int i = 0; i < rts.size(); ++i) {
			//debug(i)
			Route& rt = rts[i];

			int v = rt.head;
			int vj = customers[v].next;

			while (v != -1 && vj != -1) {

				DisType oldPtw = rts[i].rPtw;
				DisType oldPc = rts[i].rPc;

				DisType Ptw = 0;
				DisType Pc = 0;

				Ptw += customers[v].TW_X;
				Ptw += customers[vj].TWX_;


				DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
				Ptw +=std::max<DisType>(0, awp - input.datas[w].DUEDATE);
				DisType aw =
					awp <= input.datas[w].DUEDATE ?std::max<DisType>(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

				DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
				Ptw +=std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
				DisType avj =
					avjp <= input.datas[vj].DUEDATE ?std::max<DisType>(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
				Ptw +=std::max<DisType>(0, avj - customers[vj].zv);

				Pc =std::max<DisType>(0, rt.rQ + input.datas[w].DEMAND - input.Q);

				Ptw = Ptw - oldPtw;
				Pc = Pc - oldPc;

				DisType cost = input.disOf[reCusNo(w)][reCusNo(v)]
					+ input.disOf[reCusNo(w)][reCusNo(vj)]
					- input.disOf[reCusNo(vj)][reCusNo(v)];
					
				Position pt;
				pt.cost = cost;
				pt.pen = Ptw + Pc;
				pt.pos = v;
				pt.rIndex = i;

				quPush(pt);

				v = vj;
				vj = customers[vj].next;
			}
		}

		Position bestPos;
		int cnt = 0;
		while (!qu.empty()) {
			++cnt;
			if (myRand->pick(cnt) == 0) {
				bestPos = qu.top();
			}
			qu.pop();
		}
		return bestPos;
	}

	bool initMaxRoute() {

		Vec<int>que1;
		que1.reserve(input.custCnt);

		for (int i = 1; i <= input.custCnt; ++i) {
			que1.push_back(i);
		}
		unsigned shuseed = (env.seed % hust::Mod) + ((hust::myRand->pick(10000007))) % hust::Mod;
		std::shuffle(que1.begin(), que1.end(), std::default_random_engine(shuseed));

		/*auto cmp = [&](int x, int y) {
			return input.datas[x].DUEDATE < input.datas[y].DUEDATE;
		};
		sort(que1.begin(), que1.end(), cmp);*/

		int rid = 0;

		/*for (int i = 0; i < customers.size(); ++i) {
			customers[i].id = i;
		}*/

		do {

			int tp = -1;
			Position bestP;
			bool isSucceed = false;
			int eaIndex = -1;

			for (int i = 0; i < que1.size(); ++i) {
				int cus = que1[i];
				Position tPos = findBestPosInSolForInit(cus);

				if (tPos.rIndex != -1 && tPos.pen == 0) {
					if (tPos.cost < bestP.cost) {
						isSucceed = true;
						eaIndex = i;
						tp = cus;
						bestP = tPos;
					}
				}
			}

			if (isSucceed) {
				que1.erase(que1.begin() + eaIndex);
				rInsAtPos(rts[bestP.rIndex], bestP.pos, tp);
				rUpdateAvfrom(rts[bestP.rIndex], rts[bestP.rIndex].head);
				rUpdateZvfrom(rts[bestP.rIndex], rts[bestP.rIndex].tail);
				continue;
			}

			Route r1 = rCreateRoute(rid++);
			rInsAtPosPre(r1, r1.tail, que1[0]);
			que1.erase(que1.begin());
			rUpdateAvfrom(r1, r1.head);
			rUpdateZvfrom(r1, r1.tail);
			rts.push_back(r1);

		} while (!que1.empty());

		for (int i = 0; i < rts.size(); ++i) {
			Route& r = rts[i];

			rUpdateZvQfrom(r, r.tail);
			rUpdateAQfrom(r, r.head);
			rReCalRCost(r);
		}
		sumRtsPen();
		//patternAdjustment();
		return true;
	}

	bool initByArr2(Vec < Vec<int>> arr2) {
			
		int rid = 0;
		for(auto& arr:arr2){

			Route r = rCreateRoute(rid++);

			for (int cus : arr) {
				//cout << s << " ";
				rInsAtPosPre(r, r.tail, (cus));
			}

			rUpdateAvQfrom(r, r.head);
			rUpdateZvQfrom(r, r.tail);
			rts.push_back(r);
		}
		sumRtsPen();
		return true;
	}

	bool initSolution() {

		initMaxRoute();

		if (cfg->breakRecord) {
			ourTarget = input.sintefRecRN - 1;
		}
		else {
			ourTarget = input.sintefRecRN;
		}

		Log(Log::Level::Info) << "penalty: " << penalty << std::endl;
		Log(Log::Level::Info) << "ourTarget: " << ourTarget << std::endl;
		Log(Log::Level::Info) << "rts.size(): " << rts.size() << std::endl;

		return true;
	}

	bool EPrReset() {

		Route& r = EPr;
		r.rCustCnt = 0; //没有计算仓库
		//r.routeID = -1;
		r.routeCost = 0;
		r.rPc = 0;
		r.rPtw = 0;
		r.rWeight = 1;

		int pt = r.head;
		//debug(pt)
		while (pt != -1) {
			//debug(pt)
			int ptnext = customers[pt].next;
			customers[pt].reSet();
			pt = ptnext;
		}

		r.head = -1;
		r.tail = -1;

		return true;
	}

	bool EPrInsTail(int t) {

		Route& r = EPr;
		rInsAtPosPre(r, r.tail, t);

		return true;
	}

	bool EPrInsHead(int t) {

		Route& r = EPr;
		rInsAtPos(r, r.head, t);

		return true;
	}

	bool EPrInsAtPos(int pos, int node) {

		Route& r = EPr;
		rInsAtPos(r, pos, node);
		return true;
	}

	bool EPpush_back(int v) {

		rInsAtPosPre(EPr, EPr.tail, v);
		return true;
	}

	int EPsize() {
		return EPr.rCustCnt;
	}

	Vec<int> EPve() {

		Vec<int>ret = rPutCusInve(EPr);
		return ret;
	}

	bool EPrRemoveAtPos(int a) {
		rRemoveAtPos(EPr, a);
		return true;
	}

	bool EPremoveByVal(int val) {
		rRemoveAtPos(EPr, val);
		return true;
	}

	int EPrGetCusByIndex(int index) {

		int pt = EPr.head;
		pt = customers[pt].next;

		while (pt > 0 && pt <= input.custCnt && index > 0) {
			pt = customers[pt].next;
			index--;
		}
		return pt;
	}

	DeltPen estimatevw(int kind, int v, int w, int oneR = 0) {

		DeltPen bestM;

		if (kind == 0) {
			return _2optOpenvv_(v, w);
		}
		else if (kind == 1) {
			return _2optOpenvvj(v, w);
		}
		else if (kind == 2) {
			return outrelocatevToww_(v, w, 1);
		}
		else if (kind == 3) {
			return outrelocatevTowwj(v, w, 1);
		}
		else if (kind == 4) {
			return inrelocatevv_(v, w,1);
		}
		else if (kind == 5) {
			return inrelocatevvj(v, w,1);
		}
		else if (kind == 6) {
			return exchangevw_(v, w, 1);
		}
		else if (kind == 7) {
			return exchangevwj(v, w, 1);
		}
		else if (kind == 8) {
			return exchangevw(v, w, 1);
		}
		else if (kind == 9) {//3换2
			return exchangevvjvjjwwj(v, w,oneR);
		}
		else if (kind == 10) {//3换1
			return exchangevvjvjjw(v, w, oneR);
		}
		else if (kind == 11) {//2换1
			return exchangevvjw(v, w, oneR);
		}
		else if (kind == 12) {//1换2
			return exchangevwwj(v, w, oneR);
		}
		else if (kind == 13) {//扔两个
			return outrelocatevvjTowwj(v, w, oneR);
		}
		else if (kind == 14) {//扔两个
			return outrelocatevv_Toww_(v, w, oneR);
		}
		else if (kind == 15) {//扔两个
			return reversevw(v, w);
		}
		else {
			debug("estimate no this kind move");
		}

		return bestM;
	}

	DeltPen _2optOpenvv_(int v, int w) { //0
		//debug(0)

		DeltPen bestM;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (rv.routeID == rw.routeID) {
			return bestM;
		}

		int v_ = customers[v].pre;
		int wj = customers[w].next;

		if (v_ > input.custCnt && wj > input.custCnt) {
			return bestM;
		}

		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			// w->v
			DisType newwvPtw = 0;

			newwvPtw += customers[w].TW_X;
			newwvPtw += customers[v].TWX_;

			DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
			newwvPtw += std::max<DisType>(avp - customers[v].zv, 0);

			// (v-) -> (w+)
			DisType newv_wjPtw = 0;
			newv_wjPtw += customers[v_].TW_X;
			newv_wjPtw += customers[wj].TWX_;
			DisType awjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(wj)];
			newv_wjPtw += std::max<DisType>(awjp - customers[wj].zv, 0);

			bestM.PtwOnly = newwvPtw + newv_wjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rw.rWeight + newv_wjPtw * rv.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;

		};

		auto getDeltPc = [&]() {

			DisType rvQ = 0;
			DisType rwQ = 0;

			rwQ = customers[w].Q_X;
			rwQ += customers[v].QX_;

			rvQ += customers[v_].Q_X;
			rvQ += customers[wj].QX_;

			bestM.deltPc =std::max<DisType>(0, rwQ - input.Q) +
				std::max<DisType>(0, rvQ - input.Q) -
				rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		};

		auto getRcost = [&]() {

			DisType delt = 0;
			delt -= input.disOf[reCusNo(v_)][reCusNo(v)];
			delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

			delt += input.disOf[reCusNo(w)][reCusNo(v)];
			delt += input.disOf[reCusNo(v_)][reCusNo(wj)];
			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}

		return bestM;
	}

	DeltPen _2optOpenvvj(int v, int w) { //1

		DeltPen bestM;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (rv.routeID == rw.routeID) {
			return bestM;
		}

		int vj = customers[v].next;
		int w_ = customers[w].pre;

		if (vj > input.custCnt && w_ > input.custCnt) {
			return bestM;
		}

		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			// v->w
			DisType newwvPtw = 0;

			newwvPtw += customers[v].TW_X;
			newwvPtw += customers[w].TWX_;

			DisType awp = customers[v].av
				+ input.datas[v].SERVICETIME
				+ input.disOf[reCusNo(v)][reCusNo(w)];

			newwvPtw += std::max<DisType>(awp - customers[w].zv, 0);

			// (w-) -> (v+)
			DisType neww_vjPtw = 0;

			neww_vjPtw += customers[w_].TW_X;
			neww_vjPtw += customers[vj].TWX_;
			DisType avjp = customers[w_].av
				+ input.datas[w_].SERVICETIME
				+ input.disOf[reCusNo(w_)][reCusNo(vj)];
			neww_vjPtw += std::max<DisType>(avjp - customers[vj].zv, 0);

			bestM.PtwOnly = newwvPtw + neww_vjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rv.rWeight + neww_vjPtw * rw.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;
		};

		auto getDeltPc = [&]() {

			DisType rvwQ = 0;
			DisType rw_vjQ = 0;

			rvwQ += customers[v].Q_X;
			rvwQ += customers[w].QX_;

			// (w-) -> (v+)
			rw_vjQ += customers[w_].Q_X;
			rw_vjQ += customers[vj].QX_;

			bestM.deltPc = std::max<DisType>(0, rvwQ - input.Q)
				+ std::max<DisType>(0, rw_vjQ - input.Q)
				- rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		};

		auto getRcost = [&]() {

			DisType delt = 0;
			delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
			delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

			delt += input.disOf[reCusNo(v)][reCusNo(w)];
			delt += input.disOf[reCusNo(w_)][reCusNo(vj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}

		return bestM;
	}

	DeltPen outrelocatevToww_(int v, int w, int oneR) { //2

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		DeltPen bestM;

		int v_ = customers[v].pre;
		int vj = customers[v].next;
		int w_ = customers[w].pre;

		if (v_ > input.custCnt&& vj > input.custCnt) {
			return bestM;
		}

		if (rw.routeID == rv.routeID) {

			if (oneR==0) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			DisType newv_vjPtw = 0;
			DisType newvwPtw = 0;

			if (rw.routeID == rv.routeID) {

				if (v == w || v == w_) {
					return;
				}

				////////////////////////
				newv_vjPtw = newvwPtw = 0;

				int front = customers[rv.head].next;
				int back = customers[rv.tail].pre;
				while (front != -1) {
					if (front == v || front == w) {
						break;
					}
					front = customers[front].next;
				}

				while (back != -1) {
					if (back == v || back == w) {
						break;
					}
					back = customers[back].pre;
				}

				DisType lastav = 0;
				int lastv = 0;

				if (front == v) {

					DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];

					newvwPtw += std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
					newvwPtw += customers[v_].TW_X;

					DisType avj = avjp > input.datas[vj].DUEDATE ? input.datas[vj].DUEDATE :
						std::max<DisType>(avjp, input.datas[vj].READYTIME);

					lastav = avj;
					lastv = vj;

					if (vj == w_) {
						;
					}
					else {

						int pt = customers[vj].next;
						while (pt != -1) {

							if (pt == w) {
								break;
							}

							DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
							newvwPtw += std::max<DisType>(0, aptp - input.datas[pt].DUEDATE);

							DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
								std::max<DisType>(aptp, input.datas[pt].READYTIME);

							lastv = pt;
							lastav = apt;
							pt = customers[pt].next;
						}
					}

					DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(v)];
					newvwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

					DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					DisType awp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
					newvwPtw += std::max<DisType>(0, awp - customers[w].zv);

					newvwPtw += customers[w].TWX_;

				}
				else if (front == w) {

					DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

					newvwPtw += customers[w_].TW_X;
					newvwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

					DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					lastav = av;
					lastv = v;

					DisType awp = lastav + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];

					newvwPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);

					DisType aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
						std::max<DisType>(awp, input.datas[w].READYTIME);

					lastv = w;
					lastav = aw;

					int pt = customers[w].next;

					while (pt != -1) {

						DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
						newvwPtw += std::max<DisType>(0, aptp - input.datas[pt].DUEDATE);

						DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
							std::max<DisType>(aptp, input.datas[pt].READYTIME);

						lastv = pt;
						lastav = apt;

						pt = customers[pt].next;
						if (pt == v) {
							break;
						}
					}

					DisType avjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(vj)];
					newvwPtw += std::max<DisType>(0, avjp - customers[vj].zv);
					newvwPtw += customers[vj].TWX_;
				}
				else {

					//rNextDisp(rv);
					debug(front);
					debug(back);
					debug(v);
					debug(w);
					debug(env.seed);
					debug(rv.head);
					debug(rv.tail);
					debug("error 333");
				}

				newv_vjPtw = newvwPtw;
				///////////////////////////

				bestM.PtwOnly = newvwPtw - rw.rPtw;
				bestM.deltPtw = (newvwPtw - rw.rPtw) * rw.rWeight * alpha;
				bestM.deltPtw *= alpha;
					
			}
			else {

				newvwPtw += customers[w_].TW_X;
				newvwPtw += customers[w].TWX_;

				DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];
				DisType zvp = customers[w].zv - input.disOf[reCusNo(w)][reCusNo(v)] - input.datas[v].SERVICETIME;
				newvwPtw += std::max<DisType>(0, std::max<DisType>(avp, input.datas[v].READYTIME) - std::min<DisType>(input.datas[v].DUEDATE, zvp));

				newv_vjPtw += customers[v_].TW_X;
				newv_vjPtw += customers[vj].TWX_;

				DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];
				newv_vjPtw += std::max<DisType>(0, avjp - customers[vj].zv);

				bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
			return ;
		};
			
		auto getDeltPc = [&]() {

			if (rv.routeID == rw.routeID) {
				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {
				bestM.deltPc = std::max<DisType>(0, rw.rQ + input.datas[v].DEMAND - input.Q)
					+ std::max<DisType>(0, rv.rQ - input.datas[v].DEMAND - input.Q)
					- rv.rPc - rw.rPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};

		auto getRcost = [&]() {

			// inset v to w and (w-)

			DisType delt = 0;
			delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
			delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

			delt += input.disOf[reCusNo(v)][reCusNo(w_)];
			delt += input.disOf[reCusNo(v)][reCusNo(w)];
			delt += input.disOf[reCusNo(v_)][reCusNo(vj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}

		return bestM;
	}

	DeltPen outrelocatevTowwj(int v, int w, int oneR) { //3

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		DeltPen bestM;

		int v_ = customers[v].pre;
		int vj = customers[v].next;
		int wj = customers[w].next;

		if (v_ > input.custCnt&& vj > input.custCnt) {
			return bestM;
		}

		if (rw.routeID == rv.routeID) {

			if (oneR==0) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;
			DisType newv_vjPtw = 0;
			DisType newvwPtw = 0;

			if (rw.routeID == rv.routeID) {

				if (v == w || v == wj) {
					return ;
				}


				//int w_ = customers[w].pre;
				////////////////////////
				newv_vjPtw = newvwPtw = 0;

				int front = rv.head;
				int back = rv.tail;
				while (front != -1) {
					if (front == v || front == w) {
						break;
					}
					front = customers[front].next;
				}

				while (back != -1) {
					if (back == v || back == w) {
						break;
					}
					back = customers[back].pre;
				}

				DisType lastav = 0;
				int lastv = 0;

				if (front == v) {

					DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];

					newvwPtw += std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
					newvwPtw += customers[v_].TW_X;

					DisType avj = avjp > input.datas[vj].DUEDATE ? input.datas[vj].DUEDATE :
						std::max<DisType>(avjp, input.datas[vj].READYTIME);

					lastav = avj;
					lastv = vj;

					if (vj == w) {
						;
					}
					else {

						int pt = customers[vj].next;
						while (pt != -1) {

							DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
							newvwPtw += std::max<DisType>(0, aptp - input.datas[pt].DUEDATE);

							DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
								std::max<DisType>(aptp, input.datas[pt].READYTIME);

							lastv = pt;
							lastav = apt;

							if (pt == w) {
								break;
							}

							pt = customers[pt].next;
						}
					}

					DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(v)];
					newvwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

					DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
					newvwPtw += std::max<DisType>(0, awjp - customers[wj].zv);
					newvwPtw += customers[wj].TWX_;

				}
				else if (front == w) {

					DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];

					newvwPtw += customers[w].TW_X;
					newvwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

					DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
					newvwPtw += std::max<DisType>(0, awjp - input.datas[wj].DUEDATE);

					DisType awj = awjp > input.datas[wj].DUEDATE ? input.datas[wj].DUEDATE :
						std::max<DisType>(awjp, input.datas[wj].READYTIME);

					lastav = awj;
					lastv = wj;

					int pt = customers[lastv].next;
					while (pt != -1) {

						DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
						newvwPtw += std::max<DisType>(0, aptp - input.datas[pt].DUEDATE);

						DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
							std::max<DisType>(aptp, input.datas[pt].READYTIME);

						lastav = apt;
						lastv = pt;
						pt = customers[pt].next;

						if (pt == v) {
							break;
						}
					}

					DisType avjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(vj)];
					newvwPtw += std::max<DisType>(0, avjp - customers[vj].zv);
					newvwPtw += customers[vj].TWX_;

				}

				newv_vjPtw = newvwPtw;

				bestM.PtwOnly = newvwPtw - rw.rPtw;
				bestM.deltPtw = (newvwPtw - rw.rPtw) * rw.rWeight * alpha;

			}
			else {


				newvwPtw += customers[w].TW_X;
				newvwPtw += customers[wj].TWX_;
				DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
				DisType zvp = customers[wj].zv - input.disOf[reCusNo(wj)][reCusNo(v)] - input.datas[v].SERVICETIME;
				newvwPtw += std::max<DisType>(0, std::max<DisType>(avp, input.datas[v].READYTIME) - std::min<DisType>(input.datas[v].DUEDATE, zvp));

				// insert v to (w,w-)
				newv_vjPtw += customers[v_].TW_X;
				newv_vjPtw += customers[vj].TWX_;

				DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];
				newv_vjPtw += std::max<DisType>(0, avjp - customers[vj].zv);

				bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
			return ;
		};
		// insert v to w and (w+)
			
		auto getDeltPc = [&]() {

			if (rv.routeID == rw.routeID) {
				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {
				bestM.deltPc = std::max<DisType>(0, rw.rQ + input.datas[v].DEMAND - input.Q)
					+ std::max<DisType>(0, rv.rQ - input.datas[v].DEMAND - input.Q)
					- rv.rPc - rw.rPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};
			
		auto getRcost = [&]() {

			// inset v to w and (w+)
			DisType delt = 0;
			delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
			delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

			delt += input.disOf[reCusNo(v)][reCusNo(w)];
			delt += input.disOf[reCusNo(v)][reCusNo(wj)];
			delt += input.disOf[reCusNo(v_)][reCusNo(vj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen inrelocatevv_(int v, int w,int oneR) { //4

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		DeltPen bestM;

		int w_ = customers[w].pre;
		int wj = customers[w].next;
		int v_ = customers[v].pre;

		if (w_ > input.custCnt&& wj > input.custCnt) {
			return bestM;
		}

		// insert w to v and (v-)
		if (rw.routeID == rv.routeID) {
			if (oneR == 0) {
				return bestM;
			}

			if (w == v || w == v_) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType wPtw = rw.rPtw * rw.rWeight;
			DisType vPtw = rv.rPtw * rv.rWeight;

			DisType neww_wjPtw = 0;
			DisType newwvPtw = 0;

			// insert w to v and (v-)
			if (rw.routeID == rv.routeID) {

				rRemoveAtPos(rw, w);
				rInsAtPos(rw, v_, w);
				DisType oldPtwNoWei = rw.rPtw;
				newwvPtw = rUpdateAvfrom(rw, rw.head);

				bestM.PtwOnly = newwvPtw - oldPtwNoWei;
				bestM.deltPtw = newwvPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
					
				newwvPtw *= rw.rWeight;
				rRemoveAtPos(rw, w);
				rInsAtPos(rw, w_, w);
				rUpdateAvfrom(rw, rw.head);
			}
			else {
				newwvPtw += customers[v_].TW_X;
				newwvPtw += customers[v].TWX_;
				DisType awp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(w)];
				DisType zwp = customers[v].zv - input.disOf[reCusNo(w)][reCusNo(v)] - input.datas[w].SERVICETIME;
				newwvPtw += std::max<DisType>(0, std::max<DisType>(awp, input.datas[w].READYTIME) - std::min<DisType>(input.datas[w].DUEDATE, zwp));

				// insert w to (v,v-)
				neww_wjPtw += customers[w_].TW_X;
				neww_wjPtw += customers[wj].TWX_;
				DisType awjp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(wj)];
				neww_wjPtw += std::max<DisType>(0, awjp - customers[wj].zv);

				bestM.PtwOnly = newwvPtw + neww_wjPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwvPtw * rv.rWeight + neww_wjPtw * rw.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;

			}

		};

		auto getDeltPc = [&]() {

			if (rv.routeID == rw.routeID) {
				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {
				bestM.deltPc = std::max<DisType>(0, rw.rQ - input.datas[w].DEMAND - input.Q)
					+ std::max<DisType>(0, rv.rQ + input.datas[w].DEMAND - input.Q)
					- rv.rPc - rw.rPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
				
		};
			
		auto getRcost = [&]() {

			// inset w to v and (v-)
			DisType delt = 0;
			delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(w_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

			delt += input.disOf[reCusNo(w)][reCusNo(v)];
			delt += input.disOf[reCusNo(w)][reCusNo(v_)];
			delt += input.disOf[reCusNo(w_)][reCusNo(wj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen inrelocatevvj(int v, int w,int oneR) { //5

		// insert w to (v,v+)
		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		DeltPen bestM;

		int w_ = customers[w].pre;
		int wj = customers[w].next;
		int vj = customers[v].next;

		if (w_ > input.custCnt&& wj > input.custCnt) {
			return bestM;
		}

		if (rw.routeID == rv.routeID) {
			if (oneR == 0) {
				return bestM;
			}
			if (w == v || w == vj) {
				return bestM;
			}
		}
		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			DisType neww_wjPtw = 0;
			DisType newwvPtw = 0;

			if (rw.routeID == rv.routeID) {

				rRemoveAtPos(rw, w);
				rInsAtPos(rw, v, w);
				DisType oldPtwNoWei = rw.rPtw;
				newwvPtw = rUpdateAvfrom(rw, rw.head);
					
				bestM.PtwOnly = newwvPtw - oldPtwNoWei;
				newwvPtw *= rw.rWeight;
				bestM.deltPtw = newwvPtw - wPtw;
				bestM.deltPtw *= alpha;

				rRemoveAtPos(rw, w);
				rInsAtPos(rw, w_, w);
				rUpdateAvfrom(rw, rw.head);

			}
			else {

				newwvPtw += customers[v].TW_X;
				newwvPtw += customers[vj].TWX_;
				DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
				DisType zwp = customers[vj].zv - input.disOf[reCusNo(vj)][reCusNo(w)] - input.datas[w].SERVICETIME;
				newwvPtw += std::max<DisType>(0, std::max<DisType>(awp, input.datas[w].READYTIME) - std::min<DisType>(input.datas[w].DUEDATE, zwp));

				neww_wjPtw += customers[w_].TW_X;
				neww_wjPtw += customers[wj].TWX_;
				DisType awjp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(wj)];
				neww_wjPtw += std::max<DisType>(0, awjp - customers[wj].zv);

				bestM.PtwOnly = newwvPtw + neww_wjPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwvPtw * rv.rWeight + neww_wjPtw * rw.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
		};

		auto getDeltPc = [&]() {

			if (rv.routeID == rw.routeID) {
				bestM.PcOnly = 0;
				bestM.deltPc = 0;
			}
			else {
				// insert w to (v,v+)
				bestM.deltPc = std::max<DisType>(0, rw.rQ - input.datas[w].DEMAND - input.Q)
					+ std::max<DisType>(0, rv.rQ + input.datas[w].DEMAND - input.Q)
					- rv.rPc - rw.rPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
				

		};

		auto getRcost = [&]() {

			// insert w to (v,v+)
			DisType delt = 0;
			delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
			delt -= input.disOf[reCusNo(w)][reCusNo(w_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

			delt += input.disOf[reCusNo(w)][reCusNo(v)];
			delt += input.disOf[reCusNo(w)][reCusNo(vj)];
			delt += input.disOf[reCusNo(w_)][reCusNo(wj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;

	}

	DeltPen exchangevw(int v, int w, int oneR) { // 8

		DeltPen bestM;
		// exchange v and (w)
		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (w< 0 || v < 0) {
			println(w);
			println(v);
		}

		int wj = customers[w].next;
		int v_ = customers[v].pre;
		int vj = customers[v].next;
		int w_ = customers[w].pre;

		if (rv.routeID == rw.routeID) {

			if (oneR == 0) {
				return bestM;
			}
			if (v == w) {
				return bestM;
			}
		}

		if (wj < 0 || v_ < 0 || vj < 0 || w_ < 0) {
			println(wj < 0);
		}
		auto getDeltPtw = [&]()->void {

			DisType newwPtw = 0;
			DisType newvPtw = 0;
			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			if (rv.routeID == rw.routeID) {

				if (v == w) {
					return ;
				}

				if (v_ == w) {

					// (v--)->(v-)->(v)->(v+)
					if (v_ < 0) {
						println("v_", v_);
					}

					int v__ = customers[v_].pre;

					if (v__ < 0) {
						println("v__",v__);
						println("v__",v__);
					}

					DisType av = 0;
					newvPtw = 0;

					DisType avp = customers[v__].av + input.datas[v__].SERVICETIME + input.disOf[reCusNo(v__)][reCusNo(v)];
					newvPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);
					newvPtw += customers[v__].TW_X;
					av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					DisType awp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
					newvPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);

					DisType aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
						std::max<DisType>(awp, input.datas[w].READYTIME);

					DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
					newvPtw += std::max<DisType>(0, avjp - customers[vj].zv);
					newvPtw += customers[vj].TWX_;

					newwPtw = newvPtw;
				}
				else if (w_ == v) {

					// (w--)->(w-)->(w)->(w+)
					if (w_ < 0) {
						println("w_", w_);
					}

					int w__ = customers[w_].pre;
					if (w__ < 0) {
						println("w__", w__);
						println("w__", w__);
					}

					DisType aw = 0;
					newvPtw = 0;

					DisType awp = customers[w__].av + input.datas[w__].SERVICETIME + input.disOf[reCusNo(w__)][reCusNo(w)];
					newvPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);
					newvPtw += customers[w__].TW_X;
					aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
						std::max<DisType>(awp, input.datas[w].READYTIME);

					DisType avp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
					newvPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

					DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
						std::max<DisType>(avp, input.datas[v].READYTIME);

					DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
					newvPtw += std::max<DisType>(0, awjp - customers[wj].zv);
					newvPtw += customers[wj].TWX_;

					newwPtw = newvPtw;
				}
				else {

					// exchangevw
					newvPtw = 0;
					int front = rv.head;
					int back = rv.tail;
					while (front != -1) {
						if (front == v || front == w) {
							break;
						}
						front = customers[front].next;
					}

					while (back != -1) {
						if (back == v || back == w) {
							break;
						}
						back = customers[back].pre;
					}


					int fpre = customers[front].pre;
					DisType lastav = 0;
					int lastv = 0;

					DisType abkp = customers[fpre].av + input.datas[fpre].SERVICETIME + input.disOf[reCusNo(fpre)][reCusNo(back)];

					newvPtw += std::max<DisType>(0, abkp - input.datas[back].DUEDATE);
					newvPtw += customers[fpre].TW_X;

					DisType abk = abkp > input.datas[back].DUEDATE ? input.datas[back].DUEDATE :
						std::max<DisType>(abkp, input.datas[back].READYTIME);

					lastav = abk;
					lastv = back;

					int pt = customers[front].next;

					while (pt != -1) {

						if (pt == back) {
							break;
						}

						DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
						newvPtw += std::max<DisType>(0, avp - input.datas[pt].DUEDATE);

						DisType av = avp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
							std::max<DisType>(avp, input.datas[pt].READYTIME);

						lastv = pt;
						lastav = av;
						pt = customers[pt].next;
					}

					#if CHECKING
					lyhCheckTrue(lastv >= 0);
					lyhCheckTrue(front >= 0);
					#endif // CHECKING

					DisType afrvp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(front)];
					newvPtw += std::max<DisType>(0, afrvp - input.datas[front].DUEDATE);

					DisType afrv = afrvp > input.datas[front].DUEDATE ? input.datas[front].DUEDATE :
						std::max<DisType>(afrvp, input.datas[front].READYTIME);

					int bkn = customers[back].next;

					DisType abknp = afrv + input.datas[front].SERVICETIME + input.disOf[reCusNo(front)][reCusNo(bkn)];
					newvPtw += std::max<DisType>(0, abknp - customers[bkn].zv);
					newvPtw += customers[bkn].TWX_;

					newwPtw = newvPtw;
				}

				bestM.PtwOnly = newwPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
			}
			else {
					
				newvPtw += customers[v_].TW_X;
				newvPtw += customers[vj].TWX_;
				newwPtw += customers[wj].TWX_;
				newwPtw += customers[w_].TW_X;

				DisType awp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(w)];
				DisType zwp = customers[vj].zv - input.datas[w].SERVICETIME - input.disOf[reCusNo(w)][reCusNo(vj)];
				DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];
				DisType zvp = customers[wj].zv - input.datas[v].SERVICETIME - input.disOf[reCusNo(wj)][reCusNo(v)];

				newvPtw +=
					std::max<DisType>(0, std::max<DisType>(awp, input.datas[w].READYTIME) - std::min<DisType>(input.datas[w].DUEDATE, zwp));

				newwPtw +=
					std::max<DisType>(0, std::max<DisType>(avp, input.datas[v].READYTIME) - std::min<DisType>(input.datas[v].DUEDATE, zvp));
				bestM.PtwOnly = newwPtw + newvPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight + newvPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
			return ;
		};

		auto getDeltPc = [&]() {

			DisType vQ = rv.rQ;
			DisType wQ = rw.rQ;

			DisType vPc = rv.rPc;
			DisType wPc = rw.rPc;

			if (rv.routeID == rw.routeID) {
				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {

				bestM.deltPc = std::max<DisType>(0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND - input.Q)
					+ std::max<DisType>(0, vQ - input.datas[v].DEMAND + input.datas[w].DEMAND - input.Q)
					- vPc - wPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}

		};

		auto getRcost = [&]() {

			// exchange v and (w)
			DisType delt = 0;
			if (v_ == w) {
				// w-> v

				delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(w)][reCusNo(vj)];
			}
			else if (w_ == v) {

				// v w
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

				delt += input.disOf[reCusNo(v)][reCusNo(wj)];
				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
			}
			else {

				delt -= input.disOf[reCusNo(v)][reCusNo(vj)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

				delt += input.disOf[reCusNo(v)][reCusNo(wj)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(w)][reCusNo(vj)];

			}
			bestM.deltCost = delt * gamma;

		};
		//return bestM;
		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen exchangevw_(int v, int w, int oneR) { // 6

		// exchange v and (w_)
		/*Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);*/

		DeltPen bestM;

		int w_ = customers[w].pre;

		if (w_ > input.custCnt || v == w_) {
			return bestM;
		}
		return exchangevw(v, w_,oneR);

	}

	DeltPen exchangevwj(int v, int w, int oneR) { // 7

		// exchange v and (w+)
		/*Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);*/

		DeltPen bestM;

		int wj = customers[w].next;

		if (wj > input.custCnt || v == wj) {
			return bestM;
		}
		return exchangevw(v, wj, oneR);
	}

	DeltPen exchangevvjw(int v, int w,int oneR) { // 11 2换1

		// exchange vvj and (w)

		DeltPen bestM;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		int wj = customers[w].next;
		int vj = customers[v].next;

		if (vj > input.custCnt) {
			return bestM;
		}

		int vjj = customers[vj].next;

		int v_ = customers[v].pre;
		int w_ = customers[w].pre;

		if (rv.routeID == rw.routeID) {
			if (oneR == 0) {
				return bestM;
			}

			if (v == w || vj == w) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType newwPtw = 0;
			DisType newvPtw = 0;
			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			if (rv.routeID == rw.routeID) {

				if (v_ == w) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rw, w);

					// w -> v -> v+
					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rv, vj, w);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, w);

					rInsAtPos(rv, w_, w);
					rInsAtPos(rv, w, v);
					rInsAtPos(rv, v, vj);

					rUpdateAvfrom(rv, rv.head);
				}
				else if (w_ == vj) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rw, w);

					// v -> (v+) -> w 

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, v);
					rInsAtPos(rv, v, vj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, w);

					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, w);

					rUpdateAvfrom(rv, rv.head);
				}
				else {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rw, w);

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, w);

					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, w_, w);

					rUpdateAvfrom(rv, rv.head);
				}
				bestM.PtwOnly = newwPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
			}
			else {

				// (v-)->(w)->(vjj)
				newvPtw += customers[v_].TW_X;
				newvPtw += customers[vjj].TWX_;

				DisType awp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(w)];
				DisType zwp = customers[vjj].zv - input.datas[w].SERVICETIME - input.disOf[reCusNo(w)][reCusNo(vjj)];
				newvPtw +=
					std::max<DisType>(0, std::max<DisType>(awp, input.datas[w].READYTIME) - std::min<DisType>(input.datas[w].DUEDATE, zwp));

				// (w-) -> (v) -> (v+) -> (wj)
				newwPtw += customers[w_].TW_X;
				newwPtw += customers[wj].TWX_;

				DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

				newwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);
				DisType av =
					avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : std::max<DisType>(avp, input.datas[v].READYTIME);

				DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];
				DisType zvjp = customers[wj].zv - input.datas[vj].SERVICETIME - input.disOf[reCusNo(wj)][reCusNo(vj)];

				newwPtw +=
					std::max<DisType>(0, std::max<DisType>(avjp, input.datas[vj].READYTIME) - std::min<DisType>(input.datas[vj].DUEDATE, zvjp));
				bestM.PtwOnly = newwPtw + newvPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight + newvPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
		};

		auto getDeltPc = [&]() {

			DisType vQ = rv.rQ;
			DisType wQ = rw.rQ;

			DisType vPc = rv.rPc;
			DisType wPc = rw.rPc;

			if (rv.routeID == rw.routeID) {

				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {

				bestM.deltPc =
					std::max<DisType>(0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND - input.Q)
					+ std::max<DisType>(0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND + input.datas[w].DEMAND - input.Q)
					- vPc - wPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};

		auto getRcost = [&]() {

			// exchange vvj and (w)
			DisType delt = 0;
			if (v_ == w) {
				// w - v vj
				delt -= input.disOf[reCusNo(w)][reCusNo(v)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];
				delt -= input.disOf[reCusNo(vj)][reCusNo(vjj)];
				//w v vj -> v vj w
				delt += input.disOf[reCusNo(w)][reCusNo(vj)];
				delt += input.disOf[reCusNo(w)][reCusNo(vjj)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
			}
			else if (w_ == vj) {
				//v vj w -> w v vj
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];
				delt -= input.disOf[reCusNo(vj)][reCusNo(w)];
					
				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(w)][reCusNo(v)];
				delt += input.disOf[reCusNo(vj)][reCusNo(wj)];
			}
			else {

				delt -= input.disOf[reCusNo(vj)][reCusNo(vjj)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(w)][reCusNo(vjj)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(vj)][reCusNo(wj)];
			}
				

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen exchangevwwj(int v, int w,int oneR) { // 12 1换2

		// exchange v and (ww+)
		DeltPen bestM;

		int wj = customers[w].next;

		if (wj > input.custCnt) {
			return bestM;
		}
		return exchangevvjw(w, v,oneR);

	}

	DeltPen exchangevvjvjjwwj(int v, int w,int oneR) { // 9 3换2

		// exchange vvjvjj and (ww+)
		DeltPen bestM;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		int wj = customers[w].next;
		int vj = customers[v].next;

		if (vj > input.custCnt || wj > input.custCnt) {
			return bestM;
		}
		int vjj = customers[vj].next;

		if (vjj > input.custCnt) {
			return bestM;
		}

		int v3j = customers[vjj].next;

		int wjj = customers[wj].next;

		int v_ = customers[v].pre;
		int w_ = customers[w].pre;

		//exchange vvjvjj and (ww + )
		if (rw.routeID == rv.routeID) {
			if (oneR == 0) {
				return bestM;
			}
			// exchange vvjvjj and (ww+)
			if (v == w || v == wj || vj == w || vj == wj || vjj == w || vjj == wj) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType newwPtw = 0;
			DisType newvPtw = 0;

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			if (rv.routeID == rw.routeID) {

				if (v_ == wj) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);
					rRemoveAtPos(rw, wj);

					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rw, vj, vjj);

					rInsAtPos(rv, vjj, w);
					rInsAtPos(rv, w, wj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rv, w);
					rRemoveAtPos(rv, wj);

					rInsAtPos(rv, w_, w);
					rInsAtPos(rv, w, wj);

					rInsAtPos(rv, wj, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					rUpdateAvfrom(rv, rv.head);
				}
				else if (w_ == vjj) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);
					rRemoveAtPos(rw, wj);

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);
					rInsAtPos(rv, wj, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rv, w);
					rRemoveAtPos(rv, wj);

					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);
					rInsAtPos(rv, vjj, w);
					rInsAtPos(rv, w, wj);

					rUpdateAvfrom(rv, rv.head);

				}
				else {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);
					rRemoveAtPos(rw, wj);

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);

					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rv, w);
					rRemoveAtPos(rv, wj);

					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					rInsAtPos(rv, w_, w);
					rInsAtPos(rv, w, wj);
					rUpdateAvfrom(rv, rv.head);
				}
					
				bestM.PtwOnly = newwPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
			}
			else {
				//v vj vjj
				// (v-)->(w)->(w+)->(v3j)
				newvPtw += customers[v_].TW_X;
				newvPtw += customers[v3j].TWX_;

				DisType awp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(w)];

				// (v-)->(w)->(w+)->(v3j)
				newvPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);

				DisType aw =
					awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE : std::max<DisType>(input.datas[w].READYTIME, awp);

				DisType awjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(wj)];
				DisType zwjp = customers[v3j].zv - input.datas[wj].SERVICETIME - input.disOf[reCusNo(wj)][reCusNo(v3j)];

				newvPtw +=
					std::max<DisType>(0, std::max<DisType>(awjp, input.datas[wj].READYTIME) - std::min<DisType>(input.datas[wj].DUEDATE, zwjp));

				// (w-) -> (v) -> (vj) -> (vjj)-> (wjj)
				newwPtw += customers[w_].TW_X;
				newwPtw += customers[wjj].TWX_;

				DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

				newwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);
				DisType av =
					avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : std::max<DisType>(avp, input.datas[v].READYTIME);

				DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

				// (w-) -> (v) -> (vj) -> (vjj)-> (wjj)
				DisType zvjjp = customers[wjj].zv - input.datas[vjj].SERVICETIME - input.disOf[reCusNo(wjj)][vjj];

				newwPtw += std::max<DisType>(0, input.datas[vjj].READYTIME - zvjjp);

				DisType zvjj = zvjjp < input.datas[vjj].READYTIME ? input.datas[vjj].READYTIME : std::min<DisType>(input.datas[vjj].DUEDATE, zvjjp);
				DisType zvjp = zvjj - input.disOf[reCusNo(vjj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;

				newwPtw +=
					std::max<DisType>(0, std::max<DisType>(avjp, input.datas[vj].READYTIME) - std::min<DisType>(input.datas[vj].DUEDATE, zvjp));
					
				bestM.PtwOnly = newwPtw + newvPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight + newvPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;
			}
					
		};

		auto getDeltPc = [&]() {

			DisType vQ = rv.rQ;
			DisType wQ = rw.rQ;

			DisType vPc = rv.rPc;
			DisType wPc = rw.rPc;

			if (rv.routeID == rw.routeID) {
				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {
				bestM.deltPc =
					std::max<DisType>(0, wQ - input.datas[w].DEMAND - input.datas[wj].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND + input.datas[vjj].DEMAND - input.Q)
					+ std::max<DisType>(0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.datas[vjj].DEMAND + input.datas[w].DEMAND + input.datas[wj].DEMAND - input.Q)
					- vPc - wPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};

		auto getRcost = [&]() {

			// exchange v vj vjj and (ww+)
			DisType delt = 0;

			if (v3j == w) {
				delt -= input.disOf[reCusNo(vjj)][reCusNo(w)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(wj)][reCusNo(wjj)];

				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(wj)][reCusNo(v)];
				delt += input.disOf[reCusNo(vjj)][reCusNo(wjj)];
			}
			else if (wj == v_) {
			// (ww+) and v vj vjj and 

				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];
				delt -= input.disOf[reCusNo(vjj)][reCusNo(v3j)];
				delt -= input.disOf[reCusNo(wj)][reCusNo(v)];

				delt += input.disOf[reCusNo(vjj)][reCusNo(w)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(wj)][reCusNo(v3j)];
			}
			else {
				delt -= input.disOf[reCusNo(vjj)][reCusNo(v3j)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(wj)][reCusNo(wjj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(wj)][reCusNo(v3j)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(vjj)][reCusNo(wjj)];
			}
				
			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen exchangevvjvjjw(int v, int w,int oneR) { // 10 三换一

		// exchange vvjvjj and (w)
		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		DeltPen bestM;

		int wj = customers[w].next;
		int vj = customers[v].next;

		if (vj > input.custCnt) {
			return bestM;
		}
		int vjj = customers[vj].next;

		if (vjj > input.custCnt) {
			return bestM;
		}
		int v3j = customers[vjj].next;

		int v_ = customers[v].pre;
		int w_ = customers[w].pre;

		if (rv.routeID == rw.routeID) {

			if (oneR) {
				return bestM;
			}
			// exchange vvjvjj and (w)
			if (v == w || vj == w || vjj == w) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType newwPtw = 0;
			DisType newvPtw = 0;
			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			if (rv.routeID == rw.routeID) {
				//return bestM;
				// exchange vvjvjj and (w)

				if (v == wj) {
					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);

					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rw, vj, vjj);
					rInsAtPos(rv, vjj, w);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);

					rInsAtPos(rw, w_, w);

					rInsAtPos(rw, w, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rw, vj, vjj);

					rUpdateAvfrom(rv, rv.head);
				}
				else if (w_ == vjj) {
					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rv, w);


					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);
					rInsAtPos(rv, vjj, w);

					rUpdateAvfrom(rv, rv.head);

				}
				else {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rw, w);

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

					newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rv, vj);
					rRemoveAtPos(rv, vjj);
					rRemoveAtPos(rv, w);

					rInsAtPos(rv, v_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);
					rInsAtPos(rv, w_, w);

					rUpdateAvfrom(rv, rv.head);
				}
					
				bestM.PtwOnly = newwPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
			}
			else {
				//v vj vjj
				// (v-)->(w)->(v3j)
				newvPtw += customers[v_].TW_X;
				newvPtw += customers[v3j].TWX_;

				DisType awp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(w)];
				DisType zwp = customers[v3j].zv - input.datas[w].SERVICETIME - input.disOf[reCusNo(w)][reCusNo(v3j)];

				newvPtw +=
					std::max<DisType>(0, std::max<DisType>(awp, input.datas[w].READYTIME) - std::min<DisType>(input.datas[w].DUEDATE, zwp));

				// (w-) -> (v) -> (vj) -> (vjj)-> (wj)
				newwPtw += customers[w_].TW_X;
				newwPtw += customers[wj].TWX_;

				DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

				newwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);
				DisType av =
					avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : std::max<DisType>(avp, input.datas[v].READYTIME);

				DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

				// (w-) -> (v) -> (vj) -> (vjj)-> (wj)
				DisType zvjjp = customers[wj].zv - input.datas[vjj].SERVICETIME - input.disOf[reCusNo(wj)][(vjj)];

				newwPtw += std::max<DisType>(0, input.datas[vjj].READYTIME - zvjjp);

				DisType zvjj = zvjjp < input.datas[vjj].READYTIME ? input.datas[vjj].READYTIME : std::min<DisType>(input.datas[vjj].DUEDATE, zvjjp);
				DisType zvjp = zvjj - input.disOf[reCusNo(vjj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;

				newwPtw +=
					std::max<DisType>(0, std::max<DisType>(avjp, input.datas[vj].READYTIME) - std::min<DisType>(input.datas[vj].DUEDATE, zvjp));
				bestM.PtwOnly = newwPtw + newvPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight + newvPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;

			}
		};

		auto getDeltPc = [&]() {

			DisType vQ = rv.rQ;
			DisType wQ = rw.rQ;

			DisType vPc = rv.rPc;
			DisType wPc = rw.rPc;

			if (rv.routeID == rw.routeID) {

				bestM.deltPc = 0;
				bestM.PcOnly = 0;

			}
			else {

				bestM.deltPc =
					std::max<DisType>(0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND + input.datas[vjj].DEMAND - input.Q)
					+ std::max<DisType>(0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.datas[vjj].DEMAND + input.datas[w].DEMAND - input.Q)
					- vPc - wPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};

		auto getRcost = [&]() {

			// exchange v vj vjj and (w)
			DisType delt = 0;

			if (v == wj) {
				//w v vj vjj -> v vj vjj w
				delt -= input.disOf[reCusNo(w)][reCusNo(v)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];
				delt -= input.disOf[reCusNo(vjj)][reCusNo(v3j)];

				delt += input.disOf[reCusNo(w)][reCusNo(vjj)];
				delt += input.disOf[reCusNo(w)][reCusNo(v3j)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
			}
			else if (w_ == vjj) {

				//v vj vjj w -> w v vj vjj 
				//delt -= input.disOf[reCusNo(w)][reCusNo(vjj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];

				delt += input.disOf[reCusNo(w)][reCusNo(v)];
				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(vjj)][reCusNo(wj)];
			}
			else {
				delt -= input.disOf[reCusNo(vjj)][reCusNo(v3j)];
				delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
				delt -= input.disOf[reCusNo(w)][reCusNo(wj)];
				delt -= input.disOf[reCusNo(w)][reCusNo(w_)];

				delt += input.disOf[reCusNo(w)][reCusNo(v_)];
				delt += input.disOf[reCusNo(w)][reCusNo(v3j)];
				delt += input.disOf[reCusNo(v)][reCusNo(w_)];
				delt += input.disOf[reCusNo(vjj)][reCusNo(wj)];
			}
				
			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen outrelocatevvjTowwj(int v, int w,int oneR) {  //13 扔两个

		DeltPen bestM;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		int v_ = customers[v].pre;
		int vj = customers[v].next;

		if (vj > input.custCnt || vj==w) {
			return bestM;
		}

		int vjj = customers[vj].next;
		int wj = customers[w].next;

		if (v_ > input.custCnt && vjj > input.custCnt) {
			return bestM;
		}
		if (wj == v) {
			return bestM;
		}

		if (rw.routeID == rv.routeID) {
			if (oneR == 0) {
				return bestM;
			}
			//
			if (v == w || vj == w) {
				return bestM;
			}
		}

		auto getDeltPtw = [&]()->void {

			DisType vPtw = rv.rPtw * rv.rWeight;
			DisType wPtw = rw.rPtw * rw.rWeight;

			DisType newvPtw = 0;
			DisType newwPtw = 0;

			// insert (v v+) to w and (w+)
			if (rw.routeID == rv.routeID) {

				//return bestM;
				if (v == w || vj == w) {
					return ;
				}
				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);

				rInsAtPos(rv, w, v);
				rInsAtPos(rv, v, vj);

				newvPtw = newwPtw = rUpdateAvfrom(rv, rv.head);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);

				rInsAtPos(rv, v_, v);
				rInsAtPos(rv, v, vj);

				rUpdateAvfrom(rv, rv.head);

				bestM.PtwOnly = newwPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
				bestM.deltPtw *= alpha;
			}
			else {

				// insert (v,vj) to between w and wj
				// w -> v -> vj -> (wj)

				newwPtw += customers[w].TW_X;
				DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
				newwPtw += std::max<DisType>(0, avp - input.datas[v].DUEDATE);

				DisType av =
					avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : std::max<DisType>(input.datas[v].READYTIME, avp);

				DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

				newwPtw += customers[wj].TWX_;
				DisType zvjp = customers[wj].zv - input.disOf[reCusNo(wj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;
				//}

				newwPtw +=
					std::max<DisType>(0, std::max<DisType>(avjp, input.datas[vj].READYTIME) - std::min<DisType>(input.datas[vj].DUEDATE, zvjp));

				// link v- and vjj
				newvPtw += customers[v_].TW_X;
				newvPtw += customers[vjj].TWX_;
				DisType avjjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vjj)];
				newvPtw += std::max<DisType>(0, avjjp - customers[vjj].zv);

				bestM.PtwOnly = newwPtw + newvPtw - rv.rPtw - rw.rPtw;
				bestM.deltPtw = newwPtw * rw.rWeight + newvPtw * rv.rWeight - vPtw - wPtw;
				bestM.deltPtw *= alpha;

			}
			return ;
		};
			
		auto getDeltPc = [&]() {

			DisType vQ = rv.rQ;
			DisType wQ = rw.rQ;

			DisType vPc = rv.rPc;
			DisType wPc = rw.rPc;

			if (rv.routeID == rw.routeID) {

				bestM.deltPc = 0;
				bestM.PcOnly = 0;
			}
			else {

				bestM.deltPc =
					std::max<DisType>(0, wQ + input.datas[v].DEMAND + input.datas[vj].DEMAND - input.Q)
					+ std::max<DisType>(0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.Q)
					- vPc - wPc;
				bestM.PcOnly = bestM.deltPc;
				bestM.deltPc *= beta;
			}
		};
			
		auto getRcost = [&]() {

			// outrelocate v vj To w wj
			DisType delt = 0;
			delt -= input.disOf[reCusNo(vj)][reCusNo(vjj)];
			delt -= input.disOf[reCusNo(v)][reCusNo(v_)];
			delt -= input.disOf[reCusNo(w)][reCusNo(wj)];

			delt += input.disOf[reCusNo(v)][reCusNo(w)];
			delt += input.disOf[reCusNo(vj)][reCusNo(wj)];
			delt += input.disOf[reCusNo(v_)][reCusNo(vjj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;
	}

	DeltPen outrelocatevv_Toww_(int v, int w,int oneR) {  //14 扔两个左边

		DeltPen bestM;

		int v_ = customers[v].pre;
		if (v_ > input.custCnt) {
			return bestM;
		}

		/*Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);*/

		int w_ = customers[w].pre;
		int v__ = customers[v_].pre;
		int vj = customers[v].next;

		if (v__ > input.custCnt && vj > input.custCnt) {
			return bestM;
		}

		//v- v || w_ w
		return outrelocatevvjTowwj(v_,w_,oneR);

	}

	DeltPen reversevw(int v, int w) {//15 翻转

		DeltPen bestM;

		int vId = customers[v].routeID;
		int wId = customers[w].routeID;

		if (vId > input.custCnt || wId > input.custCnt || vId != wId || v == w) {
			return bestM;
		}

		Route& r = rts.getRouteByRid(customers[v].routeID);

		int front = r.head;
		int back = r.tail;

		while (front != -1) {
			if (front == v || front == w) {
				break;
			}
			front = customers[front].next;
		}

		while (back != -1) {
			if (back == v || back == w) {
				break;
			}
			back = customers[back].pre;
		}

		#if CHECKING
		lyhCheckTrue(back!=-1);
		#endif // CHECKING

		int f_ = customers[front].pre;
		int bj = customers[back].next;
			
		auto getDeltPtw = [&]()->void {

			int fj = customers[front].next;
			if (fj == back) {
				bestM = exchangevw(v, w, 1);
				return ;
			}
			int fjj = customers[fj].next;
			if (fjj == back) {
				bestM = exchangevw(v, w, 1);
				return ;
			}

			DisType newPtw = 0;
			DisType lastav = 0;
			int lastv = 0;

			lastv = back;
			DisType lastavp = customers[f_].av + input.datas[f_].SERVICETIME + input.disOf[reCusNo(f_)][reCusNo(back)];
			newPtw += customers[f_].TW_X;
			newPtw += std::max<DisType>(0, lastavp - input.datas[lastv].DUEDATE);
			lastav = lastavp > input.datas[lastv].DUEDATE ? input.datas[lastv].DUEDATE :
				std::max<DisType>(lastavp, input.datas[lastv].READYTIME);

			int pt = customers[lastv].pre;
			while (pt != -1) {

				DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
				newPtw += std::max<DisType>(0, aptp - input.datas[pt].DUEDATE);

				DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
					std::max<DisType>(aptp, input.datas[pt].READYTIME);
				lastv = pt;
				lastav = apt;
				if (pt == front) {
					break;
				}

				pt = customers[pt].pre;
			}

			DisType abjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(bj)];
			newPtw += std::max<DisType>(0, abjp - customers[bj].zv);
			newPtw += customers[bj].TWX_;

			bestM.PtwOnly = newPtw - r.rPtw;
			bestM.deltPtw = (newPtw - r.rPtw) * r.rWeight * alpha;
			return ;
		};

		auto getDeltPc = [&]() {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		};
			
		auto getRcost = [&]() {

			// outrelocate  v_ vTo w_ w 
			DisType delt = 0;
			delt -= input.disOf[reCusNo(front)][reCusNo(f_)];
			delt -= input.disOf[reCusNo(back)][reCusNo(bj)];

			delt += input.disOf[reCusNo(back)][reCusNo(f_)];
			delt += input.disOf[reCusNo(front)][reCusNo(bj)];

			bestM.deltCost = delt * gamma;

		};

		if (alpha > 0) {
			getDeltPtw();
		}

		if (beta > 0) {
			getDeltPc();
		}

		if (gamma > 0) {
			getRcost();
		}
		return bestM;

	}

	DeltPen _Nopt(Vec<int>& nodes) { //16 Nopt*

		DeltPen bestM;

		DisType newPtwNoWei = 0;
		DisType newPtw = 0;
		DisType newPc = 0;

		auto linkvw = [&](int vt, int wt) {

			DisType newwvPtw = 0;
			newwvPtw += vt > 0 ? customers[vt].TW_X : 0;
			newwvPtw += wt > 0 ? customers[wt].TWX_ : 0;
			DisType awp = (vt > 0 ? customers[vt].av + input.datas[vt].SERVICETIME : 0)
				+ input.disOf[reCusNo(vt)][reCusNo(wt)];

			newwvPtw += std::max<DisType>(awp - (wt > 0 ? customers[wt].zv : input.datas[0].DUEDATE), 0);
			newPtwNoWei += newwvPtw;

			newPc += std::max<DisType>(0,
				(vt > 0 ? customers[vt].Q_X : 0) + (wt > 0 ? customers[wt].QX_ : 0) - input.Q);
			return newwvPtw;
		};

		DisType oldPtwNoWei = 0;
		DisType oldPtw = 0;
		DisType oldPc = 0;
		for (std::size_t i = 0; i < nodes.size(); i += 2) {
			int rId = customers[(nodes[i] == 0 ? nodes[i + 1] : nodes[i])].routeID;
			Route& r = rts.getRouteByRid(rId);
			oldPtwNoWei += r.rPtw;
			oldPtw += r.rPtw * r.rWeight;
			oldPc += r.rPc;
			DisType npnw = linkvw(nodes[i], nodes[(i + 3) % nodes.size()]);
			newPtw += npnw * r.rWeight;
		}

		bestM.PtwOnly = newPtwNoWei - oldPtwNoWei;
		bestM.deltPtw = newPtw - oldPtw;
		bestM.deltPtw *= alpha;
		bestM.deltPc = newPc - oldPc;
		bestM.PcOnly = bestM.deltPc;
		bestM.deltPc *= beta;
		return bestM;

	}

	bool doMoves(TwoNodeMove& M) {

		if (M.kind == 0 || M.kind == 1) {
			twoOptStar(M);
		}
		else if (M.kind == 2 || M.kind == 3 || M.kind == 13 || M.kind == 14) {
			outRelocate(M);
		}
		else if (M.kind == 4 || M.kind == 5) {
			inRelocate(M);
		}
		else if (M.kind == 6 || M.kind == 7 || M.kind == 8
			|| M.kind == 9 || M.kind == 10 || M.kind == 11 || M.kind == 12) {
			exchange(M);
		}
		else if (M.kind == 15) {
			doReverse(M);
		}
		else if (M.kind == 16) {
			//doNopt(M);
			Log(Log::Level::Error) << "doNopt(M) error" << std::endl;
		}
		else {
			debug("no this kind of move!");
			return false;
		}
		return true;
	}

	bool twoOptStar(TwoNodeMove& M) {

		int v = M.v;
		int w = M.w;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (M.kind == 0) {

			int vpre = customers[v].pre;
			int wnext = customers[w].next;

			int pt = v;

			customers[w].next = v;
			customers[v].pre = w;

			customers[vpre].next = wnext;
			customers[wnext].pre = vpre;

			rv.rCustCnt = 0;
			pt = customers[rv.head].next;
			while (pt != -1) {
				++rv.rCustCnt;
				customers[pt].routeID = rv.routeID;
				pt = customers[pt].next;
			}
			rv.rCustCnt--;

			pt = customers[rw.head].next;
			rw.rCustCnt = 0;
			while (pt != -1) {
				++rw.rCustCnt;
				customers[pt].routeID = rw.routeID;
				pt = customers[pt].next;
			}
			rw.rCustCnt--;

			std::swap(rv.tail, rw.tail);

			rUpdateAvQfrom(rw, w);
			rUpdateZvQfrom(rw, v);

			rUpdateAvQfrom(rv, vpre);
			rUpdateZvQfrom(rv, wnext);

			return true;

		}
		else if (M.kind == 1) {

			int vnext = customers[v].next;
			int wpre = customers[w].pre;

			customers[v].next = w;
			customers[w].pre = v;
			customers[wpre].next = vnext;
			customers[vnext].pre = wpre;

			rv.rCustCnt = 0;
			int pt = customers[rv.head].next;
			while (pt != -1) {
				++rv.rCustCnt;
				customers[pt].routeID = rv.routeID;
				pt = customers[pt].next;
			}
			rv.rCustCnt--;

			rw.rCustCnt = 0;
			pt = customers[rw.head].next;
			while (pt != -1) {
				++rw.rCustCnt;
				customers[pt].routeID = rw.routeID;
				pt = customers[pt].next;
			}
			rw.rCustCnt--;

			std::swap(rv.tail, rw.tail);

			rUpdateAvQfrom(rv, v);
			rUpdateZvQfrom(rv, w);

			rUpdateAvQfrom(rw, wpre);
			rUpdateZvQfrom(rw, vnext);

			return true;

		}
		else {
			debug("no this two opt * move!");
			return false;
		}
		return false;
	}

	bool outRelocate(TwoNodeMove& M) {

		int v = M.v;
		int w = M.w;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (M.kind == 2) {

			int vpre = customers[v].pre;
			int vnext = customers[v].next;

			rRemoveAtPos(rv, v);
			rInsAtPos(rw, customers[w].pre, v);

			if (rv.routeID == rw.routeID) {

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);

			}
			else {

				rUpdateAvQfrom(rv, vpre);
				rUpdateZvQfrom(rv, vnext);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);
			}
		}
		else if (M.kind == 3) {

			int vpre = customers[v].pre;
			int vnext = customers[v].next;

			rRemoveAtPos(rv, v);
			rInsAtPos(rw, w, v);

			if (rv.routeID == rw.routeID) {

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {

				rUpdateAvQfrom(rv, vpre);
				rUpdateZvQfrom(rv, vnext);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);
			}
		}
		else if (M.kind == 13) {
			// outrelocatevvjTowwj
			int vj = customers[v].next;

			if (rw.routeID == rv.routeID) {

				if (v == w || vj == w) {
					return false;
				}

				rRemoveAtPos(rw, v);
				rRemoveAtPos(rw, vj);
				rInsAtPos(rw, w, v);
				rInsAtPos(rw, v, vj);

				rUpdateAvQfrom(rw, rw.head);
				rUpdateZvQfrom(rw, rw.tail);

			}
			else {
				// outrelocatevvjTowwj
				int vjj = customers[vj].next;
				int v_ = customers[v].pre;

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);

				rInsAtPos(rw, w, v);
				rInsAtPos(rw, v, vj);

				rUpdateAvQfrom(rv, v_);
				rUpdateZvQfrom(rv, vjj);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, vj);
			}
		}
		else if (M.kind == 14) {

			int v_ = customers[v].pre;
			int v__ = customers[v_].pre;
			int vj = customers[v].next;

			if (rw.routeID == rv.routeID) {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, v_);

				rInsAtPosPre(rv, w, v);
				rInsAtPosPre(rv, v, v_);

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, v_);

				rInsAtPosPre(rw, w, v);
				rInsAtPosPre(rw, v, v_);

				rUpdateAvQfrom(rv, v__);
				rUpdateZvQfrom(rv, vj);

				rUpdateAvQfrom(rw, v_);
				rUpdateZvQfrom(rw, v);
			}
		}
		else {
		debug("no this inrelocate move");
		return false;
		}
		return true;

		return true;
	}

	bool inRelocate(TwoNodeMove& M) {

		int v = M.v;
		int w = M.w;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (M.kind == 4) {
			// inset w to (v-)->(v)

			int wnext = customers[w].next;
			int wpre = customers[w].pre;

			rRemoveAtPos(rw, w);
			rInsAtPos(rv, customers[v].pre, w);

			if (rv.routeID == rw.routeID) {

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, w);

				rUpdateAvQfrom(rw, wpre);
				rUpdateZvQfrom(rw, wnext);

			}

		}
		else if (M.kind == 5) {

			int wnext = customers[w].next;
			int wpre = customers[w].pre;

			rRemoveAtPos(rw, w);
			rInsAtPos(rv, v, w);

			if (rv.routeID == rw.routeID) {

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);

			}
			else {

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, w);

				rUpdateAvQfrom(rw, wpre);
				rUpdateZvQfrom(rw, wnext);
			}
		}
		return true;

	}

	bool exchange(TwoNodeMove& M) {

		int v = M.v;
		int w = M.w;

		Route& rv = rts.getRouteByRid(customers[v].routeID);
		Route& rw = rts.getRouteByRid(customers[w].routeID);

		if (M.kind == 6) {
			// exchange v and (w_)
			int w_ = customers[w].pre;
			if (w_ > input.custCnt || v == w_) {
				return false;
			}

			int w__ = customers[w_].pre;
			int v_ = customers[v].pre;

			if (rv.routeID == rw.routeID) {

				if (v == w__) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rw, w_);

					rInsAtPos(rv, v_, w_);
					rInsAtPos(rw, w_, v);
				}
				else {
					rRemoveAtPos(rv, v);
					rRemoveAtPos(rw, w_);

					rInsAtPos(rw, w__, v);
						
					// exchange v and (w_)
					if (w == v) {
						rInsAtPos(rv, v, w_);
					}
					else {
						rInsAtPos(rv, v_, w_);
					}
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);

			}
			else {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rw, w_);

				rInsAtPos(rw, w__, v);
				rInsAtPos(rv, v_, w_);

				rUpdateAvQfrom(rv, w_);
				rUpdateZvQfrom(rv, w_);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);

			}
			return true;
		}
		else if (M.kind == 7) {
			// exchange v and (w+)
			int wj = customers[w].next;
			/*if (wj > input.custCnt || v == wj) {
				return false;
			}*/

			int wjj = customers[wj].next;
			int v_ = customers[v].pre;

			if (rv.routeID == rw.routeID) {
				if (v == wj) {
					return false;
				}
				else if (v == wjj) {

					rRemoveAtPos(rv, v);
					rRemoveAtPos(rw, wj);

					rInsAtPos(rv, w, v);
					rInsAtPos(rw, v, wj);
				}
				else {
					rRemoveAtPos(rv, v);
					rRemoveAtPos(rw, wj);

					rInsAtPos(rv, v_, wj);
						
					if (v == w) {
						rInsAtPos(rw, wj, v);
					}
					else {
						rInsAtPos(rw, w, v);
					}
				}
				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
				return true;
			}
			else {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rw, wj);

				rInsAtPos(rw, w, v);
				rInsAtPos(rv, v_, wj);

				rUpdateAvQfrom(rv, wj);
				rUpdateZvQfrom(rv, wj);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);

				return true;
			}
			return true;
		}
		else if (M.kind == 8) {

			int v_ = customers[v].pre;
			int w_ = customers[w].pre;

			rRemoveAtPos(rv, v);
			rRemoveAtPos(rw, w);

			if (rv.routeID == rw.routeID) {
				//return bestM;

				if (v_ == w) {
					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, w);
				}
				else if (w_ == v) {
					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, v);
				}
				else {
					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w_, v);
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {

				rInsAtPos(rv, v_, w);
				rInsAtPos(rw, w_, v);

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, w);
				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);

			}
		}
		else if (M.kind == 9) {

			//exchangevvjvjjwwj 3换2
			int wj = customers[w].next;
			int vj = customers[v].next;
			int vjj = customers[vj].next;
			int v_ = customers[v].pre;
			int w_ = customers[w].pre;

			if (rv.routeID == rw.routeID) {

				/*if (v == w || v == wj || vj == w || vj == wj || vjj == w || vjj == wj) {
					return false;
				}*/

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, wj);

				if (v_ == wj) {

					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rw, vj, vjj);

					rInsAtPos(rv, vjj, w);
					rInsAtPos(rv, w, wj);

				}
				else if (w_ == vjj) {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);
					rInsAtPos(rv, wj, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

				}
				else {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);

					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);

			}
			else {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, wj);

				rInsAtPos(rv, v_, w);
				rInsAtPos(rv, w, wj);
				rInsAtPos(rw, w_, v);
				rInsAtPos(rw, v, vj);
				rInsAtPos(rw, vj, vjj);

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
				rUpdateAvQfrom(rw, rw.head);
				rUpdateZvQfrom(rw, rw.tail);
			}
		}
		else if (M.kind == 10) {

			//exchangevvjvjjw 3换1
			int v_ = customers[v].pre;
			int vj = customers[v].next;
			int vjj = customers[vj].next;
			int wj = customers[w].next;
			int w_ = customers[w].pre;

			if (rv.routeID == rw.routeID) {

				// exchange vvjvjj and (w)
				/*if (v == w || vj == w || vjj == w) {
					return false;
				}*/

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);

				if (v == wj) {

					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rw, vj, vjj);
					rInsAtPos(rv, vjj, w);

				}
				else if (w_ == vjj) {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);

				}
				else {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);
					rInsAtPos(rv, vj, vjj);
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);

				rInsAtPos(rv, v_, w);
				rInsAtPos(rw, w_, v);
				rInsAtPos(rw, v, vj);
				rInsAtPos(rw, vj, vjj);

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, w);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, vjj);
			}
		}
		else if (M.kind == 11) {
			//exchangevvjw 2换1
			int v_ = customers[v].pre;
			int vj = customers[v].next;
			int w_ = customers[w].pre;

			if (rv.routeID == rw.routeID) {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rw, w);

				if (v_ == w) {
					// w -> v -> v+
					rInsAtPos(rw, w_, v);
					rInsAtPos(rw, v, vj);
					rInsAtPos(rv, vj, w);
				}
				else if (w_ == vj) {
					// v -> (v+) -> w
					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, v);
					rInsAtPos(rv, v, vj);
				}
				else {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w_, v);
					rInsAtPos(rv, v, vj);
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {
				//exchangevvjw
				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rw, w);

				rInsAtPos(rv, v_, w);

				rInsAtPos(rw, w_, v);
				rInsAtPos(rw, v, vj);

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, w);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, vj);

			}
		}
		else if (M.kind == 12) {
			// exchangevwwj
			int v_ = customers[v].pre;
			int wj = customers[w].next;
			int w_ = customers[w].pre;

			if (rv.routeID == rw.routeID) {

				/*if (v == w || v == wj) {
					return false;
				}*/

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, wj);

				// (w) -> (w+)-> (v) 
				if (v_ == wj) {

					rInsAtPos(rw, w_, v);
					rInsAtPos(rv, v, w);
					rInsAtPos(rv, w, wj);

				}
				else if (w_ == v) {

					// (v) -> (w) -> (w+)
					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);
					rInsAtPos(rv, wj, v);

				}
				else {

					rInsAtPos(rv, v_, w);
					rInsAtPos(rv, w, wj);
					rInsAtPos(rv, w_, v);
				}

				rUpdateAvQfrom(rv, rv.head);
				rUpdateZvQfrom(rv, rv.tail);
			}
			else {
				// exchangevwwj
				rRemoveAtPos(rv, v);
				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, wj);

				rInsAtPos(rv, v_, w);
				rInsAtPos(rv, w, wj);
				rInsAtPos(rw, w_, v);

				rUpdateAvQfrom(rv, w);
				rUpdateZvQfrom(rv, wj);

				rUpdateAvQfrom(rw, v);
				rUpdateZvQfrom(rw, v);

			}
		}
		else {
			debug("no this inrelocate move");
			return false;
		}
		return true;
	}

	bool doReverse(TwoNodeMove& M) {

		int v = M.v;
		int w = M.w;

		Route& r = rts.getRouteByRid(customers[v].routeID);

		int front = r.head;
		int back = r.tail;

		while (front != -1) {
			if (front == v || front == w) {
				break;
			}
			front = customers[front].next;
		}

		while (back != -1) {
			if (back == v || back == w) {
				break;
			}
			back = customers[back].pre;
		}

		int f_ = customers[front].pre;
		int bj = customers[back].next;

		Vec<int> ve;
		ve.reserve(r.rCustCnt);

		int pt = front;
		while (pt != -1) {
			ve.push_back(pt);
			if (pt == back) {
				break;
			}
			pt = customers[pt].next;
		}

		for (int pt : ve) {
			std::swap(customers[pt].next, customers[pt].pre);
		}

		customers[front].next = bj;
		customers[back].pre = f_;

		customers[f_].next = back;
		customers[bj].pre = front;

		rUpdateAvQfrom(r, back);
		rUpdateZvQfrom(r, front);

		return true;
	}

	bool updateYearTable(TwoNodeMove& t) {

		int v = t.v;
		int w = t.w;

		if (t.kind == 0) {
			//_2optOpenvv_
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 1) {
			//_2optOpenvvj
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 2) {
			// outrelocatevToww_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 3) {
			// outrelocatevTowwj
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 4) {

			// inrelocatevv_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 5) {

			// inrelocatevvj
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 6) {

			// exchangevw_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int w__ = customers[w_].pre > input.custCnt ? 0 : customers[w_].pre;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v == w__) {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}
				else {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w__][w_] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}

			}
			else {

				(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[w__][w_] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

			}

		}
		else if (t.kind == 7) {
			// exchangevwj
			//int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v == wj) {
					return false;
				}
				else if (v == wjj) {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}
				else {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[wj][wjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}

			}
			else {

				(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[wj][wjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

			}

		}
		else if (t.kind == 8) {

			// exchangevw
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v_ == w) {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}
				else if (w_ == v) {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}
				else {

					(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
					(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				}

			}
			else {

				(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

			}

		}
		else if (t.kind == 9) {

			//exchangevvjvjjwwj(v, w); 三换二 v v+ v++ | w w+

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
			int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[vjj][v3j] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[wj][wjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);


		}
		else if (t.kind == 10) {

			//exchangevvjvjjw(v, w); 三换一 v v+ v++ | w

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
			int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[vjj][v3j] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 11) {
			//exchangevvjw(v, w); 二换一 v v +  | w
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[vj][vjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 12) {

			//exchangevwwj(v, w); 一换二 v  | w w+

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[wj][wjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 13) {

			//outrelocatevvjTowwj(v, w); 扔两个 v v+  | w w+
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

			//int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			//int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[vj][vjj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 14) {
			// v- v | w_ w
			//outrelocatevv_Toww_  | w-  v- v w

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int v__ = customers[v_].pre > input.custCnt ? 0 : customers[v_].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;

			(*yearTable)[v__][v_] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[v][vj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

		}
		else if (t.kind == 15) {

			//reverse [v,w]
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;


			(*yearTable)[v_][v] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

			int pt = v;
			int ptn = customers[pt].next;
			while (pt != w) {

				(*yearTable)[pt][ptn] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);

				pt = ptn;
				ptn = customers[ptn].next;
			}

		}
		/*else if (t.kind == 16) {

			for (int i = 0; i < t.ve.size(); i += 2) {
				int v = t.ve[i];
				int w = t.ve[(i + 3) % t.ve.size()];
				sumYear += (*yearTable)[v][w];
			}
			sumYear = sumYear / t.ve.size() * 2;

		}*/
		else {
			debug("sol tabu dont include this move");
		}


		return true;
	}

	Vec<int> getPtwNodes(Route& r, int ptwKind = 0) {

		Vec<int> ptwNodes;
		ptwNodes.reserve(r.rCustCnt);

		int v = 0;

		#if CHECKING
		lyhCheckTrue(r.rPtw>0);
		#endif // CHECKING

		auto getPtwNodesByFirstPtw = [&]() {

			int startNode = customers[r.tail].pre;
			int endNode = customers[r.head].next;

			int pt = customers[r.head].next;
			while (pt != -1) {
				if (customers[pt].avp > input.datas[pt].DUEDATE) {
					endNode = pt;
					break;
				}
				pt = customers[pt].next;
			}
			if (endNode > input.custCnt) {
				endNode = customers[r.tail].pre;
			}

			pt = customers[r.tail].pre;
			while (pt != -1) {
				if (customers[pt].zvp < input.datas[pt].READYTIME) {
					startNode = pt;
					break;
				}
				pt = customers[pt].pre;
			}
			if (startNode > input.custCnt) {
				startNode = customers[r.head].next;
			}

			v = startNode;
			while (v <= input.custCnt) {
				ptwNodes.push_back(v);
				if (v == endNode) {
					break;
				}
				v = customers[v].next;
			}

			if (v != endNode) {
				v = customers[r.head].next;
				while (v <= input.custCnt) {
					ptwNodes.push_back(v);
					if (v == endNode) {
						break;
					}
					v = customers[v].next;
				}
			}

			if (v != endNode) {
				debug(v != endNode);
			}
		};

		auto getPtwNodesByLastPtw = [&]() {

			int startNode = customers[r.tail].pre;
			int endNode = customers[r.head].next;

			int pt = customers[r.head].next;
			while (pt !=-1) {
				if (customers[pt].avp > input.datas[pt].DUEDATE) {
					endNode = pt;
				}
				pt = customers[pt].next;
			}
			if (endNode > input.custCnt) {
				endNode = customers[r.tail].pre;
			}

			pt = customers[r.tail].pre;
			while (pt != -1) {
				if (customers[pt].zvp < input.datas[pt].READYTIME) {
					startNode = pt;
				}
				pt = customers[pt].pre;
			}

			if (startNode > input.custCnt ) {
					
				startNode = customers[r.head].next;
			}

			v = startNode;
			while (v <= input.custCnt) {
				ptwNodes.push_back(v);
				if (v == endNode) {
					break;
				}
				v = customers[v].next;
			}
		};

		auto findRangeCanDePtw1 = [&]() {

			int startNode = customers[r.tail].pre;
			int endNode = customers[r.head].next;

			int pt = customers[r.head].next;
			bool stop = false;
			while (pt != -1) {

				if (customers[pt].avp > input.datas[pt].DUEDATE) {
					endNode = pt;
					stop = true;
					break;
				}
				pt = customers[pt].next;
			}
			if (stop == false) {
				endNode = customers[r.tail].pre;
			}

			pt = customers[r.tail].pre;
			stop = false;
			while (pt <= input.custCnt) {

				if (customers[pt].zvp < input.datas[pt].READYTIME) {
					startNode = pt;
					stop = true;
					break;
				}
				pt = customers[pt].pre;
			}

			if (stop == false) {
				startNode = customers[r.head].next;
			}

			if (customers[startNode].pre <= input.custCnt) {
				startNode = customers[startNode].pre;
			}

			if (customers[endNode].next <= input.custCnt) {
				endNode = customers[endNode].next;
			}

			pt = startNode;
			while (pt <= input.custCnt) {
				ptwNodes.push_back(pt);
				if (pt == endNode) {
					break;
				}
				pt = customers[pt].next;
			}
		};

		if (ptwKind == -1) {
			findRangeCanDePtw1();
		}
		else if (ptwKind == 0) {
			getPtwNodesByFirstPtw();
		}
		else if (ptwKind == 1) {
			getPtwNodesByLastPtw();
		}

#if CHECKING
		lyhCheckTrue(ptwNodes.size()>0);
#endif // CHECKING

		return ptwNodes;
	}

	LL getYearOfMove(TwoNodeMove& t) {

		int v = t.v;
		int w = t.w;

		LL sumYear = 0;

		if (t.kind == 0) {
			//_2optOpenvv_
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			sumYear = ((*yearTable)[w][v] + (*yearTable)[v_][wj]) / 2;
		}
		else if (t.kind == 1) {
			//_2optOpenvvj
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			sumYear = ((*yearTable)[v][w] + (*yearTable)[w_][vj]) / 2;
		}
		else if (t.kind == 2) {
			//outrelocatevToww_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			sumYear = ((*yearTable)[v_][vj] + (*yearTable)[w_][v] + (*yearTable)[v][w]) / 3;

		}
		else if (t.kind == 3) {
			//outrelocatevTowwj
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			sumYear = ((*yearTable)[v_][vj] + (*yearTable)[w][v] + (*yearTable)[v][wj]) / 3;
		}
		else if (t.kind == 4) {
			//inrelocatevv_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

			sumYear = ((*yearTable)[w_][wj] + (*yearTable)[v_][w] + (*yearTable)[w][v]) / 3;
		}
		else if (t.kind == 5) {
			//inrelocatevvj
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			sumYear = ((*yearTable)[w_][wj] + (*yearTable)[v][w] + (*yearTable)[w][vj]) / 3;
		}
		else if (t.kind == 6) {
			//exchangevw_
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int w__ = customers[w_].pre > input.custCnt ? 0 : customers[w_].pre;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v == w__) {

					sumYear = ((*yearTable)[v_][w_] + (*yearTable)[w_][v]
						+ (*yearTable)[v][w]) / 3;
				}
				else {
					// v- v vj | w-- w- w
					sumYear = ((*yearTable)[w__][v] + (*yearTable)[v][w]
						+ (*yearTable)[v_][w_] + (*yearTable)[w_][vj]) / 4;

				}

			}
			else {

				sumYear = ((*yearTable)[w__][v] + (*yearTable)[v][w]
					+ (*yearTable)[v_][w_] + (*yearTable)[w_][vj]) / 4;
			}
		}
		else if (t.kind == 7) {
			//exchangevwj
			//int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v == wj) {
					return false;
				}
				else if (v == wjj) {

					sumYear = ((*yearTable)[w][v] + (*yearTable)[v][wj]
						+ (*yearTable)[wj][vj]) / 3;
				}
				else {
					// v- v vj |  w wj wjj
					sumYear = ((*yearTable)[w][v] + (*yearTable)[v][wjj]
						+ (*yearTable)[v_][wj] + (*yearTable)[wj][vj]) / 4;
				}

			}
			else {

				sumYear = ((*yearTable)[w][v] + (*yearTable)[v][wjj]
					+ (*yearTable)[v_][wj] + (*yearTable)[wj][vj]) / 4;
			}
		}
		else if (t.kind == 8) {
			//exchangevw
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			int rvId = customers[v].routeID;
			int rwId = customers[w].routeID;

			if (rvId == rwId) {

				if (v_ == w) {

					sumYear = ((*yearTable)[w_][v] + (*yearTable)[w][vj]
						+ (*yearTable)[v][w]) / 3;
				}
				else if (w_ == v) {

					sumYear = ((*yearTable)[v_][w] + (*yearTable)[w][v]
						+ (*yearTable)[v][wj]) / 3;
				}
				else {
					// v- v vj |  w- w wj
					sumYear = ((*yearTable)[w_][v] + (*yearTable)[v][wj]
						+ (*yearTable)[v_][w] + (*yearTable)[w][vj]) / 4;
				}

			}
			else {

				sumYear = ((*yearTable)[w_][v] + (*yearTable)[v][wj]
					+ (*yearTable)[v_][w] + (*yearTable)[w][vj]) / 4;
			}

		}
		else if (t.kind == 9) {

			//exchangevvjvjjwwj(v, w); 三换二 v v+ v++ | w w+

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
			int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			sumYear = ((*yearTable)[v][w_] + (*yearTable)[vjj][wjj]
				+ (*yearTable)[v_][w] + (*yearTable)[wj][v3j]) / 4;

		}
		else if (t.kind == 10) {

			//exchangevvjvjjw(v, w); 三换一 v v + v++ | w

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
			int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			sumYear = ((*yearTable)[v][w_] + (*yearTable)[vjj][wj]
				+ (*yearTable)[v_][w] + (*yearTable)[w][v3j]) / 4;

		}
		else if (t.kind == 11) {
			//exchangevvjw(v, w); 二换一 v v +  | w

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
				
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			//TODO[lyh][bug]:
			sumYear = ((*yearTable)[v][w_] + (*yearTable)[vj][wj]
				+ (*yearTable)[v_][w] + (*yearTable)[w][vjj]) / 4;
		}
		else if (t.kind == 12) {

			//exchangevwwj(v, w); 一换二 v  | w w+

			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			sumYear = ((*yearTable)[w_][v] + (*yearTable)[v][wjj]
				+ (*yearTable)[v_][w] + (*yearTable)[wj][vj]) / 4;

		}
		else if (t.kind == 13) {

			//outrelocatevvjTowwj(v, w); 扔两个 v v+  | w w+

			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			//int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
			//int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

			sumYear = ((*yearTable)[v][w] + (*yearTable)[vj][wj] + (*yearTable)[v][vj]) / 3;
		}
		else if (t.kind == 14) {

			//outrelocatevv_Toww_  | w-  v- v w
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int v__ = customers[v_].pre > input.custCnt ? 0 : customers[v_].pre;
			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
			sumYear = ((*yearTable)[w_][v_] + (*yearTable)[v][w] + (*yearTable)[v__][vj]) / 3;
		}
		else if (t.kind == 15) {

			//reverse [v,w]
			int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
			int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

			sumYear = ((*yearTable)[v][wj] + (*yearTable)[w][v_]) / 2;
		}
		else if (t.kind == 16) {

			for (int i = 0; i < t.ve.size(); i += 2) {
				int v = t.ve[i];
				int w = t.ve[(i + 3) % t.ve.size()];
				sumYear += (*yearTable)[v][w];
			}
			sumYear = sumYear / t.ve.size() * 2;

		}
		else {
			debug("get year of none");
		}
		return sumYear;
	}

	TwoNodeMove getMovesRandomly(std::function<bool(TwoNodeMove & t,TwoNodeMove & bestM)>updateBestM) {

		TwoNodeMove bestM;

		auto _2optEffectively = [&](int v) {

			int vid = customers[v].routeID;

			int v_ = customers[v].pre;
			if (v_ > input.custCnt) {
				v_ = 0;
			}

			double broaden = cfg->broaden;

			int v_pos = input.addSTJIsxthcloseOf[v][v_];
			//int v_pos = input.jIsxthcloseOf[v][v_];
			if (v_pos == 0) {
				v_pos += cfg->broadenWhenPos_0;
			}
			else {
				v_pos *= broaden;
			}
			v_pos = std::min<int>(v_pos, input.custCnt);

			for (int wpos = 0; wpos < v_pos; ++wpos) {

				int w = input.addSTclose[v][wpos];
				//int w = input.allCloseOf[v][wpos];
				int wid = customers[w].routeID;

				if (wid == -1 || wid == vid) {
					continue;
				}

				if (customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)] >= customers[v].avp) {
					continue;
				}

				TwoNodeMove m0(v, w, 0, _2optOpenvv_(v, w));
				updateBestM(m0,bestM);
			}

			int vj = customers[v].next;
			if (vj > input.custCnt) {
				vj = 0;
			}
			int vjpos = input.addSTJIsxthcloseOf[v][vj];
			//int vjpos = input.jIsxthcloseOf[v][vj];
			if (vjpos == 0) {
				vjpos += cfg->broadenWhenPos_0;
			}
			else {
				vjpos *= broaden;
			}
			vjpos = std::min<int>(vjpos, input.custCnt);

			for (int wpos = 0; wpos < vjpos; ++wpos) {

				int w = input.addSTclose[v][wpos];
				//int w = input.allCloseOf[v][wpos];
				int wid = customers[w].routeID;

				if (wid == -1 || wid == vid) {
					continue;
				}

				if (customers[w].zv - input.disOf[reCusNo(v)][reCusNo(w)] - input.datas[v].SERVICETIME <= customers[v].zvp) {
					continue;
				}

				TwoNodeMove m1(v, w, 1, _2optOpenvvj(v, w));
				updateBestM(m1, bestM);
			}
		};

		auto exchangevwEffectively = [&](int v) {

			int vpre = customers[v].pre;
			if (vpre > input.custCnt) {
				vpre = 0;
			}

			int vpos1 = input.addSTJIsxthcloseOf[vpre][v];

			double broaden = cfg->broaden;

			int devided = 7;

			if (vpos1 == 0) {
				vpos1 += cfg->broadenWhenPos_0;
			}
			else {
				vpos1 *= broaden;
			}

			vpos1 = std::min<int>(vpos1, input.custCnt);

			if (vpos1 > 0) {

				int N = vpos1;
				int m = std::max<int>(1, N / devided);
				Vec<int>& ve = myRandX->getMN(N, m);
				for (int i = 0; i < m; ++i) {
					int wpos = ve[i];

					int w = input.addSTclose[vpre][wpos];
					//int w = input.addSTclose[vpre][wpos];
					if (customers[w].routeID == -1) {
						continue;
					}
					TwoNodeMove m8(v, w, 8, exchangevw(v, w,0));
					updateBestM(m8, bestM);

					/*TwoNodeMove m9(v, w, 9, exchangevvjvjjwwj(v, w));
					updateBestM(m9,bestM);*/

					/*TwoNodeMove m10(v, w, 10, exchangevvjvjjw(v, w));
					updateBestM(m10,bestM);

					TwoNodeMove m11(v, w, 11, exchangevvjw(v, w));
					updateBestM(m11,bestM);*/

					/*TwoNodeMove m12(v, w, 12, exchangevwwj(v, w));
					updateBestM(m12,bestM);*/
				}
			}


			int vnext = customers[v].next;
			if (vnext > input.custCnt) {
				vnext = 0;
			}
			int vpos2 = input.addSTJIsxthcloseOf[vnext][v];

			if (vpos2 == 0) {
				vpos2 += cfg->broadenWhenPos_0;
			}
			else {
				vpos2 *= broaden;
			}

			vpos2 = std::min<int>(vpos2, input.custCnt);

			if (vpos2 > 0) {

				int N = vpos2;
				int m = std::max<int>(1, N / devided);

				m = std::max<int>(1, m);
				myRandX->getMN(N, m);
				Vec<int>& ve = myRandX->mpLLArr[N];

				for (int i = 0; i < m; ++i) {
					int wpos = ve[i];
					wpos %= N;
					int w = input.addSTclose[vnext][wpos];
					if (customers[w].routeID == -1) {
						continue;
					}

					TwoNodeMove m8(v, w, 8, exchangevw(v, w,0));
					updateBestM(m8, bestM);

					/*TwoNodeMove m9(v, w, 9, exchangevvjvjjwwj(v, w));
					updateBestM(m9,bestM);

					TwoNodeMove m10(v, w, 10, exchangevvjvjjw(v, w));
					updateBestM(m10,bestM);*/

					/*TwoNodeMove m11(v, w, 11, exchangevvjw(v, w));
					updateBestM(m11,bestM);*/

					/*TwoNodeMove m12(v, w, 12, exchangevwwj(v, w));
					updateBestM(m12,bestM);*/
				}
			}

		};

		auto outrelocateEffectively = [&](int v) {

			int devided = 20;
			Vec<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[v];

			int N = relatedToV.size();
			int m = std::max<DisType>(1, N / devided);

			Vec<int>& ve = myRandX->getMN(N, m);
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];
				int w = relatedToV[wpos];

				if (customers[w].routeID == -1) {
					continue;
				}

				TwoNodeMove m2(v, w, 2, outrelocatevToww_(v, w,0));
				updateBestM(m2, bestM);

				//if (customers[w].TW_X == 0) {

					/*TwoNodeMove m14(v, w, 14, outrelocatevv_Toww_(v, w));
					updateBestM(m14);*/
					//}

				TwoNodeMove m3(v, w, 3, outrelocatevTowwj(v, w,0));
				updateBestM(m3, bestM);

				//if (customers[w].TWX_ == 0) {

					/*TwoNodeMove m13(v, w, 13, outrelocatevvjTowwj(v, w));
					updateBestM(m13,bestM);*/
					//}
			}
		};

#if _3optEffectively
		auto _3optEffectively = [&](int v) {

			double broaden = cfg->broaden[cfg->broadIndex];

			int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
			int vjpos = input.jIsxthcloseOf[v][vj];

			if (vjpos < 0) {
				debug(vj)

			}
			if (vjpos == 0) {
				vjpos += 50;
			}
			else {
				vjpos *= broaden;
			}

			vjpos = std::min<int>(vjpos, input.custCnt);
			int vrId = customers[v].routeID;

			int N = vjpos;
			int m = std::max<DisType>(1, sqrt(N));

			myRandX->getMN(N, m);
			Vec<int>& ve = myRandX.mpLLArr[N];
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				int w = input.allCloseOf[v][wpos];
				int wrId = customers[w].routeID;

				if (wrId == -1 || wrId == vrId) {
					continue;
				}

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wIsw_pos = input.jIsxthcloseOf[w_][w];

				if (wIsw_pos == 0) {
					wIsw_pos += 50;
				}
				else {
					wIsw_pos *= broaden;
				}

				wIsw_pos = std::min<int>(wIsw_pos, input.custCnt);

				int mpos = myRand->pick(wIsw_pos);
				int m = input.allCloseOf[w_][mpos];
				int mrId = customers[m].routeID;

				int cnt = mpos;
				while (mrId == -1 || mrId == vrId || mrId == wrId) {
					m = input.allCloseOf[w_][mpos];
					mrId = customers[m].routeID;
					if (--cnt < 0) {
						break;
					}
				}

				if (cnt < 0) {
					continue;
				}

				int m_ = customers[m].pre > input.custCnt ? 0 : customers[m].pre;
				if (v + w == 0 || w_ + m == 0 || m_ + vj == 0) {
					continue;
				}

				Vec<int>ve = { v,vj,w_,w,m_,m };
				TwoNodeMove m16(ve, 16, _Nopt(ve));
				updateBestM(m16, bestM);
			}

		};

#endif // _3optEffectively

		bestM.reSet();

		int rId = -1;
		if (PtwConfRts.cnt > 0) {
			int index = -1;
			index = myRand->pick(PtwConfRts.cnt);
			rId = PtwConfRts.ve[index];
		}
		else if (PcConfRts.cnt > 0) {
			int index = -1;
			index = myRand->pick(PcConfRts.cnt);
			rId = PcConfRts.ve[index];
		}
		else {
			debug("error on conf route");
		}

		/*int confRCnt = PtwConfRts.cnt + PcConfRts.cnt;
		int index = -1;
		if (confRCnt > 0) {
			index = myRand->pick(confRCnt);
		}
		else {
			debug("no conf route")
			debug("no conf route")
		}

		int rId = 0;
		if (index < PtwConfRts.cnt) {
			rId = PtwConfRts.ve[index];
		}
		else {
			index -= PtwConfRts.cnt;
			rId = PcConfRts.ve[index];
		}*/

		Route& r = rts.getRouteByRid(rId);

#if CHECKING
		lyhCheckTrue(r.rPc > 0 || r.rPtw > 0);
#endif // CHECKING

		if (r.rPtw > 0) {

			Vec<int> ptwNodes = getPtwNodes(r);
				
			int v = ptwNodes[0];
			int w = v;
			for (int i = 0; i < 5; ++i) {
				w = customers[w].pre;
				if (w > input.custCnt) {
					break;
				}
				TwoNodeMove m8(v, w, 8, exchangevw(v, w, 1));
				updateBestM(m8, bestM);
				if (i > 0) {
					TwoNodeMove m2(v, w, 2, outrelocatevToww_(v, w, 1));
					updateBestM(m2, bestM);
				}
				if (i >= 2) {
					TwoNodeMove m15(w, v, 15, reversevw(w, v));
					updateBestM(m15, bestM);
				}
			}

			for (int v : ptwNodes) {

				_2optEffectively(v);
				exchangevwEffectively(v);
				outrelocateEffectively(v);

				int w = v;
				int maxL = std::max<int>(5, r.rCustCnt / 5);
				//int maxL = 5;
				//debug(r.rCustCnt)

				if (myRand->pick(3) == 0 ) {
					for (int i = 1; i <= maxL; ++i) {
						w = customers[w].next;
						if (w > input.custCnt) {
							break;
						}

						TwoNodeMove m8(v, w, 8, exchangevw(v, w, 1));
						updateBestM(m8, bestM);

						if (i >= 2) {
							TwoNodeMove m3(v, w, 3, outrelocatevTowwj(v, w, 1));
							updateBestM(m3, bestM);
						}

						if (i >= 3) {
							TwoNodeMove m15(v, w, 15, reversevw(v, w));
							updateBestM(m15, bestM);
						}
					}
				}

			}
		}
		else {

			for (int v : rPutCusInve(r)) {

				Vec<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[v];
				int N = relatedToV.size();
				int m = N / 7;
				m = std::max<int>(1, m);

				Vec<int>& ve = myRandX->getMN(N, m);
				for (int i = 0; i < m; ++i) {
					int wpos = ve[i];

					int w = relatedToV[wpos];
					if (customers[w].routeID == -1) {
						continue;
					}

					TwoNodeMove m0(v, w, 0, _2optOpenvv_(v, w));
					updateBestM(m0, bestM);
					TwoNodeMove m1(v, w, 1, _2optOpenvvj(v, w));
					updateBestM(m1, bestM);

					TwoNodeMove m2(v, w, 2, outrelocatevToww_(v, w, 0));
					updateBestM(m2, bestM);
					TwoNodeMove m3(v, w, 3, outrelocatevTowwj(v, w, 0));
					updateBestM(m3, bestM);

					/*TwoNodeMove m4(v, w, 4, inrelocatevv_(v, w));
					updateBestM(m4,bestM);
					TwoNodeMove m5(v, w, 5, inrelocatevvj(v, w));
					updateBestM(m5,bestM);*/

					TwoNodeMove m6(v, w, 6, exchangevw_(v, w, 0));
					updateBestM(m6, bestM);
					TwoNodeMove m7(v, w, 7, exchangevwj(v, w, 0));
					updateBestM(m7, bestM);

					TwoNodeMove m8(v, w, 8, exchangevw(v, w, 0));
					updateBestM(m8, bestM);

					if (customers[v].routeID != customers[w].routeID) {

						/*TwoNodeMove m9(v, w, 9, exchangevvjvjjwwj(v, w,0));
						updateBestM(m9,bestM);*/
						TwoNodeMove m10(v, w, 10, exchangevvjvjjw(v, w, 0));
						updateBestM(m10, bestM);

						TwoNodeMove m11(v, w, 11, exchangevvjw(v, w, 0));
						updateBestM(m11, bestM);

						/*TwoNodeMove m12(v, w, 12, exchangevwwj(v, w, 0));
						updateBestM(m12,bestM);*/

						TwoNodeMove m13(v, w, 13, outrelocatevvjTowwj(v, w, 0));
						updateBestM(m13, bestM);
						/*TwoNodeMove m14(v, w, 14, outrelocatevv_Toww_(v, w, 0));
						updateBestM(m14,bestM);*/
					}
				}
			}

		}

		return bestM;

	}

	bool resetConfRts() {

		PtwConfRts.reSet();
		PcConfRts.reSet();

		for (int i = 0; i < rts.size(); ++i) {
			if (rts[i].rPtw > 0) {
				PtwConfRts.ins(rts[i].routeID);
			}
			else if (rts[i].rPc > 0) {
				PcConfRts.ins(rts[i].routeID);
			}
		}
		return true;
	};

	bool resetConfRtsByOneMove(Vec<int> ids) {

		for (int id : ids) {
			PtwConfRts.removeVal(id);
			PcConfRts.removeVal(id);
		}

		for (int id : ids) {
			Route& r = rts.getRouteByRid(id);
			if (r.rPtw > 0) {
				PtwConfRts.ins(r.routeID);
			}
			else if (r.rPc > 0) {
				PcConfRts.ins(r.routeID);
			}
		}
			
		//resetConfRts();
		return true;
	};

	bool doEject(Vec<eOneRNode>& XSet) {

#if CHECKING

		int cnt = 0;
		for (int i : PtwConfRts.pos) {
			if (i >= 0) {
				++cnt;
			}
		}
		lyhCheckTrue(cnt == PtwConfRts.cnt);
		for (int i = 0; i < PtwConfRts.cnt; ++i) {
			Route& r = rts.getRouteByRid(PtwConfRts.ve[i]);
			lyhCheckTrue(r.rPtw > 0);
		}

		for (int i = 0; i < PcConfRts.cnt; ++i) {
			Route& r = rts.getRouteByRid(PcConfRts.ve[i]);
			lyhCheckTrue(r.rPc > 0);
		}

		for (eOneRNode& en : XSet) {
			lyhCheckTrue(en.ejeVe.size()>0);
		}

#endif // CHECKING

		for (eOneRNode& en : XSet) {

			Route& r = rts.getRouteByRid(en.rId);

			for (int node : en.ejeVe) {
				rRemoveAtPos(r, node);
				EPpush_back(node);
			}
			rUpdateAvQfrom(r, r.head);
			rUpdateZvQfrom(r, r.tail);

#if CHECKING
			lyhCheckTrue(r.rPc == 0 && r.rPtw == 0);
#endif // CHECKING
		}

		resetConfRts();
		sumRtsPen();

#if CHECKING
		lyhCheckTrue(penalty==0);
		lyhCheckTrue(PtwConfRts.cnt == 0);
		lyhCheckTrue(PcConfRts.cnt == 0);
#endif // CHECKING

		return true;
	}

	bool EPNodesCanEasilyPut() {

		for (int EPIndex = 0; EPIndex < EPsize();) {
#if CHECKING
			DisType oldpenalty = PtwNoWei + Pc;
#endif // CHECKING

			Vec<int> arr = EPve();
			int top = arr[EPIndex];

			Position bestP = findBestPosInSol(top);

			if (bestP.pen == 0) {

				Route& r = rts[bestP.rIndex];

				P[top] += cfg->Pwei0;
				//EP(*yearTable)[top] = EPIter + cfg->EPTabuStep + myRand->pick(cfg->EPTabuRand);
				EPremoveByVal(top);

				rInsAtPos(r, bestP.pos, top);
				rUpdateAvQfrom(r, top);
				rUpdateZvQfrom(r, top);

				sumRtsPen();
				resetConfRts();

#if CHECKING
				lyhCheckTrue(oldpenalty + bestP.pen == PtwNoWei + Pc);
#endif // CHECKING

			}
			else {
				++EPIndex;
			}

		}
		return true;
	}

	bool managerCusMem(Vec<int>& releaseNodes) {

		//printve(releaseNodes);
		int useEnd = input.custCnt + 2 + (rts.cnt+1) * 2 + 1;

		for (int i : releaseNodes) {
			
			for (int j = useEnd -1; j > i; --j) {

				if (customers[j].routeID != -1) {

					customers[i] = customers[j];

					Route& r = rts.getRouteByRid(customers[j].routeID);

					int jn = customers[j].next;
					if (jn != -1) {
						customers[jn].pre = i;
						r.head = i;
					}

					int jp = customers[j].pre;
					if (jp != -1) {
						customers[jp].next = i;
						r.tail = i;
					}

					customers[j].reSet();
					break;
					--useEnd;
				}
			}
		}
		return true;
	}

	bool removeOneRouteRandomly(int index = -1) {

		// delete one route randomly
		/*for (int i = 0; i <= input.custCnt; ++i) {
			P[i] = 1;
		}*/

		Vec<CircleSector> angs(rts.cnt);
		Vec<int> range(rts.cnt);
		for (int i = 0; i < rts.cnt; ++i) {
			auto ve = rPutCusInve(rts[i]);
			angs[i].initialize(input.datas[ve[0]].polarAngle);
			for (int j = 1; j < ve.size(); ++j) {
				angs[i].extend(input.datas[ve[j]].polarAngle);
			}
			range[i] = CircleSector::positive_mod(angs[i].end - angs[i].start);
		}

		index = 0;
		for (int i = 1; i < rts.cnt; ++i) {
			if (range[i] > range[index]) {
				index = i;
			}
		}

		if (index == -1) {
			index = myRand->pick(rts.size());
		}

		Route& rt = rts[index];

		Vec<int> rtVe = rPutCusInve(rt);
		Vec<int> releasedNodes = { rt.head,rt.tail };
		rReset(rt);
		rts.removeIndex(index);

		for (int pt : rtVe) {
			EPpush_back(pt);
		}

		sumRtsPen();
		resetConfRts();
		managerCusMem(releasedNodes);

		return true;
	}
		
	DisType verify() {

		Vec<int> visitCnt(input.custCnt + 1, 0);

		int cusCnt = 0;
		DisType routesCost = 0;

		Ptw = 0;
		Pc = 0;

		for (int i = 0; i < rts.size(); ++i) {

			Route& r = rts[i];

			//cusCnt += r.rCustCnt;
			Ptw += rUpdateAvfrom(r, r.head);
			Pc += rUpdatePc(r);

			Vec<int> cusve = rPutCusInve(r);
			for (int pt : cusve) {
				++cusCnt;
				++visitCnt[pt];
				if (visitCnt[pt] > 1) {
					return -1;
				}
			}
			rReCalRCost(r);
			routesCost += r.routeCost;
			//debug(routesCost)
		}

		for (int i = 0; i < rts.cnt; ++i) {
			rts[i].rWeight = 1;
		}

		sumRtsPen();

		if (Ptw > 0 && Pc == 0) {
			return -2;
		}
		if (Ptw == 0 && Pc > 0) {
			return -3;
		}
		if (Ptw > 0 && Pc > 0) {
			return -4;
		}
		if (cusCnt != input.custCnt) {
			return -5;
		}
		return routesCost;
	}

	DeltPen getDeltIfRemoveOneNode(Route&r,int pt) {

		int pre = customers[pt].pre;
		int next = customers[pt].next;

		DeltPen d;

		DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(next)];
		d.PtwOnly = std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[pre].TW_X;
		d.PtwOnly = d.PtwOnly - r.rPtw;
		d.PcOnly = std::max<DisType>(0,r.rQ - input.datas[pt].DEMAND - input.Q);
		d.PcOnly = d.PcOnly - r.rPc;
		return d;

	};

	bool addWeightToRoute(TwoNodeMove& bestM) {

		if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly >= 0) {

			for (int i = 0; i < PtwConfRts.cnt; ++i) {
				Route& r = rts.getRouteByRid(PtwConfRts.ve[i]);

				//auto cus = rPutCusInve(r);
				//for (int c: cus) {
				//	auto d = getDeltIfRemoveOneNode(r, c);
				//	if (d.PtwOnly < 0) {
				//		int cpre = customers[c].pre > input.custCnt ? 0 : customers[c].pre;
				//		int cnext = customers[c].next > input.custCnt ? 0 : customers[c].next;
				//		/*debug(c)
				//		debug(d.PtwOnly)
				//		debug(d.PcOnly)*/
				//		(*yearTable)[c][cnext] = squIter + cfg->yearTabuLen;
				//		(*yearTable)[cpre][c] = squIter + cfg->yearTabuLen;
				//	}
				//}

				r.rWeight += cfg->weightUpStep;
				Ptw += r.rPtw;
			}
			penalty = alpha * Ptw + beta * Pc;

				
		}
		return true;
	};

	Vec<eOneRNode> ejeNodesAfterSqueeze;

	bool squeeze() {

		Vec<SolClone> bestPool;
		bestPool.reserve(2);

		SolClone sClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty,RoutesCost);

		bestPool.push_back(sClone);

		auto updateBestPool = [&](DisType Pc, DisType PtwNoWei) {

			bool isUpdate = false;
			bool dominate = false;

			Vec<SolClone> ::iterator it = bestPool.begin();
			for (it = bestPool.begin(); it != bestPool.end();) {

				if (Pc < (*it).Pc && PtwNoWei < (*it).PtwNoWei
					|| Pc <= (*it).Pc && PtwNoWei < (*it).PtwNoWei
					|| Pc < (*it).Pc && PtwNoWei <= (*it).PtwNoWei) {

					isUpdate = true;
					it = bestPool.erase(it);
				}
				else if (Pc >= (*it).Pc && PtwNoWei >= (*it).PtwNoWei) {
					dominate = true;
					break;
				}
				else {
					++it;
				}
			}

			if (!dominate && bestPool.size() < 50) {

				SolClone s(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty,RoutesCost);

				bestPool.push_back(s);
			}

			return isUpdate;
		};

		//auto getBestSolIndex = [&]() {
		//	DisType min1 = DisInf;
		//	int index = 0;
		//	for (int i = 0; i < bestPool.size(); ++i) {
		//		//debug(i)
		//		DisType temp = bestPool[i].Pc + bestPool[i].PtwNoWei;
		//		if (temp < min1) {
		//			min1 = temp;
		//			index = i;
		//		}
		//	}
		//	return index;
		//};

		auto getMinPsumSolIndex = [&]() {

			int min1 = IntInf;
			int index = 0;

			for (int i = 0; i < bestPool.size(); ++i) {
				customers = bestPool[i].customers;
				rts = bestPool[i].rts;
				resetConfRts();
				sumRtsPen();

				Vec<eOneRNode> ret = ejectFromPatialSol();

				int bestCnt = 1;
				int sum = 0;
				//int ejeNum = 0;

				for (eOneRNode& en : ret) {
					sum += en.Psum;
					//ejeNum += en.ejeVe.size();
					/*for (int c : en.ejeVe) {
						sum = std::max<DisType>(sum,P[c]);
					}*/
				}

				//sum *= ejeNum;

				if (sum < min1) {
					bestCnt = 1;
					min1 = sum;
					index = i;
					ejeNodesAfterSqueeze = ret;
				}
				else if (sum == min1) {
					++bestCnt;
					if (myRand->pick(bestCnt) == 0) {
						index = i;
						ejeNodesAfterSqueeze = ret;
					}
				}
			}

			return index;
		};

		alpha = 1;
		beta = 1;
		gamma = 0;

		for (int i = 0; i < rts.cnt; ++i) {
			rts[i].rWeight = 1;
		}

		resetConfRts();
		sumRtsPen();

		//int deTimeOneTurn = 0;
		//int contiTurnNoDe = 0;

		squIter += cfg->yearTabuLen + cfg->yearTabuRand;

		cfg->squContiIter = cfg->squMinContiIter;
		DisType pBestThisTurn = DisInf;
		int contiNotDe = 0;

		auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

			/*LL tYear = getYearOfMove(t);
			bool isTabu = (squIter <= tYear);
			bool isTabu = false;
			if (isTabu) {
				if (t.deltPen.deltPc + t.deltPen.deltPtw
					< golbalTabuBestM.deltPen.deltPc + golbalTabuBestM.deltPen.deltPtw) {
					golbalTabuBestM = t;
				}
				else if (t.deltPen.deltPc + t.deltPen.deltPtw
					== golbalTabuBestM.deltPen.deltPc + golbalTabuBestM.deltPen.deltPtw) {

					if (getYearOfMove(t) < getYearOfMove(golbalTabuBestM)) {
						golbalTabuBestM = t;
					}
				}
			}
			else {*/

			if (t.deltPen.deltPc == DisInf || t.deltPen.deltPtw == DisInf) {
				return false;
			}

			if (t.deltPen.deltPc + t.deltPen.deltPtw
				< bestM.deltPen.deltPc + bestM.deltPen.deltPtw) {
				bestM = t;
			}
			else if ((t.deltPen.deltPc + t.deltPen.deltPtw
				== bestM.deltPen.deltPc + bestM.deltPen.deltPtw)) {
				/*if (t.deltPen.deltCost < golbalBestM.deltPen.deltCost) {
					golbalBestM = t;
				}*/
				if (getYearOfMove(t) < getYearOfMove(bestM)) {
					bestM = t;
				}
			}

			return true;
		};

		while (penalty > 0 && !lyhTimer.isTimeOut()) {

			TwoNodeMove bestM = getMovesRandomly(updateBestM);

			if (bestM.deltPen.PcOnly == DisInf || bestM.deltPen.PtwOnly == DisInf) {
				debug("squeeze fail find move");
				debug(squIter);
				++contiNotDe;
				continue;
			}
			/*else if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly <= 0) {
				;
			}
			else if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
				;
			}*/

#if CHECKING

			Vec<Vec<int>> oldRoutes;
			Vec<int> oldrv;
			Vec<int> oldrw;

			DisType oldpenalty = penalty;
			DisType oldPtw = Ptw;
			DisType oldPc = Pc;
			DisType oldPtwNoWei = PtwNoWei;
			DisType oldPtwOnly = PtwNoWei;
			DisType oldPcOnly = Pc;
			DisType oldRcost = RoutesCost;

			Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
			Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
			oldrv = rPutCusInve(rv);
			oldrw = rPutCusInve(rw);

			for (int i = 0; i < rts.cnt; ++i) {
				Route& r = rts[i];
				if (rPutCusInve(r).size() != r.rCustCnt) {
					lyhCheckTrue(rPutCusInve(r).size() == r.rCustCnt);
					lyhCheckTrue(rPutCusInve(r).size() == r.rCustCnt);
				}
			}

#endif // CHECKING

			updateYearTable(bestM);
			int rvId = customers[bestM.v].routeID;
			int rwId = customers[bestM.w].routeID;

			doMoves(bestM);

			++squIter;
			/*solTabuTurnSolToBitArr();
			solTabuUpBySolToBitArr();*/
				
			updatePen(bestM.deltPen);

#if CHECKING
			DisType penaltyaferup = penalty;
			sumRtsPen();
			lyhCheckTrue(penaltyaferup==penalty)
			bool penaltyWeiError = 
				!DISlfeq(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty);
			bool penaltyError =
				!DISlfeq(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly, PtwNoWei + Pc);
				
			lyhCheckTrue(!penaltyWeiError);
			lyhCheckTrue(!penaltyError);

			for (int i = 0; i < rts.cnt; ++i) {
				Route& r = rts[i];
				lyhCheckTrue(rPutCusInve(r).size() == r.rCustCnt);

				int pt = r.head;
				while (pt != -1) {
					lyhCheckTrue(pt <= customers.size());
					pt = customers[pt].next;
				}
			}

			if (penaltyWeiError|| penaltyError){

				debug("squeeze penalty update error!");

				debug(bestM.v);
				debug(bestM.w);
				debug(bestM.kind);

				debug(rv.routeID == rw.routeID);

				std::cout << "oldrv: ";

				for (auto i : oldrv) {
					std::cout << i << " ,";
				}
				std::cout << std::endl;
				std::cout << "oldrw: ";
				for (auto i : oldrw) {
					std::cout << i << " ,";
				}
				std::cout << std::endl;

				rNextDisp(rv);
				rNextDisp(rw);
			}
#endif // CHECKING

			//resetConfRts();
			resetConfRtsByOneMove({ rvId,rwId });
			addWeightToRoute(bestM);

			if (penalty < pBestThisTurn) {
				contiNotDe = 0;
				pBestThisTurn = penalty;
			}
			else {
				++contiNotDe;
			}

			//bool isDown = updateBestPool(Pc, PtwNoWei);
			updateBestPool(Pc, PtwNoWei);
				
			//if (contiNotDe == squCon.squContiIter / 2) {
				//if (PtwNoWei < Pc) {
				//	beta *= 1.01;
				//	//beta *= 1.1;
				//	if ( beta > 10000) {
				//		 beta = 10000;
				//	}
				//}
				//else if (PtwNoWei > Pc) {
				//	 beta *= 0.99;
				//	//beta *= 0.9;
				//	if ( beta < 1) {
				//		 beta = 1;
				//	}
				//}
				//penalty =  alpha*Ptw +  beta*Pc;
				//LL minW = squCon.inf;
				//for (int i = 0; i < rts.size(); ++i) {
				//	minW = std::min<int>(minW, rts[i].rWeight);
				//}
				//minW = minW/3 + 1;
				//for (int i = 0; i < rts.size(); ++i) {
				//	//rts[i].rWeight = rts[i].rWeight - minW +1;
				//	rts[i].rWeight = rts[i].rWeight/ minW +1;
				//}
				//sumRtsPen();
			//}

			if (contiNotDe == cfg->squContiIter) {

				if (penalty < 1.1 * pBestThisTurn && cfg->squContiIter < cfg->squMaxContiIter) {
					cfg->squContiIter += cfg->squIterStepUp;
					cfg->squContiIter = std::min<int>(cfg->squMaxContiIter, cfg->squContiIter);
				}
				else {
					break;
				}
			}

			/*out(squCon.squContiIter)
			out(deTimeOneTurn);
			out(bestPool[index].PtwNoWei+ bestPool[index].Pc)
			out(PtwNoWei)
			debug(Pc)*/

		}

		if (penalty == 0) {
			for (int i = 0; i < rts.cnt; ++i) {
				rts[i].rWeight = 1;
			}

			resetConfRts();
			sumRtsPen();

			return true;
		}
		else {

			bestPool.push_back(sClone);
			int index = getMinPsumSolIndex();
			//debug(index)

			setCurSolBy(bestPool[index]);
				
#if CHECKING
			DisType oldp = penalty;
			sumRtsPen();
			lyhCheckTrue(oldp == penalty);
#endif // CHECKING		

			return false;
		}
		return true;
	}
#if 1
	Position findBestPosForRuin(int w) {

		// 惩罚最大的排在最前面
		auto cmp = [&](const Position a, const Position& b) {
			/*if (a.pen == b.pen) {
				return  a.cost < b.cost;
			}
			else {
				return a.pen < b.pen;
			}*/
			return  a.pen < b.pen;
			//return a.pen < b.pen;
			//return a.cost  < b.cost;
			//return a.year < b.year;
		};

		std::priority_queue<Position, Vec<Position>, decltype(cmp)> qu(cmp);

		auto updatePool = [&](Position& pos) {

			qu.push(pos);
			
			if (qu.size() <= cfg->findBestPosForRuinPqSize) {
				;
			}
			else {
				qu.pop();
			}
		};
		// TODO[lyh][-1]:bug vpos上限小了 可能找不到合适的位置
		for (int vpos = 0; vpos  + 1 < input.custCnt;vpos++) {

			int v = input.allCloseOf[w][vpos]; 
			int vrId = customers[v].routeID;
			if (vrId != -1) {

				Route& rt = rts.getRouteByRid(vrId);
				int vj = customers[v].next;

				DisType oldrPc = rt.rPc;
				DisType rPc = std::max<DisType>(0, rt.rQ + input.datas[w].DEMAND - input.Q);
				rPc = rPc - oldrPc;

				DisType oldrPtw = rt.rPtw;

				DisType rPtw = customers[v].TW_X;
				rPtw += customers[vj].TWX_;

				DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
				rPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);
				DisType aw =
					awp <= input.datas[w].DUEDATE ? std::max<DisType>(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

				DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
				rPtw += std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
				DisType avj =
					avjp <= input.datas[vj].DUEDATE ? std::max<DisType>(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
				rPtw += std::max<DisType>(0, avj - customers[vj].zv);

				rPtw = rPtw - oldrPtw;

				DisType cost = input.disOf[reCusNo(w)][reCusNo(v)] 
					+ input.disOf[reCusNo(w)][reCusNo(vj)]
					- input.disOf[reCusNo(v)][reCusNo(vj)];

				LL year = (*yearTable)[reCusNo(w)][reCusNo(v)] + (*yearTable)[reCusNo(w)][reCusNo(vj)];
					
				Position posTemp;
				posTemp.rIndex = rts.posOf[rt.routeID];
				posTemp.cost = cost;
				posTemp.pen = rPtw + rPc;
				posTemp.pos = v;
				posTemp.year = year;
				posTemp.secDis = abs(input.datas[w].polarAngle - input.datas[v].polarAngle);
				updatePool(posTemp);
				if (qu.size() == cfg->findBestPosForRuinPqSize) {
					break;
				}
			}
		}

		Position ret;
		int retNum = 0;
		while (!qu.empty()) {
			++retNum;
			if (qu.top().cost < ret.cost) {
				ret = qu.top();
			}
			qu.pop();
		}
		return ret;
	}

#else
	Position findBestPosForRuin(int w) {

		// 惩罚最大的排在最前面
		auto cmp = [&](const Position a, const Position& b) {
			/*if (a.pen == b.pen) {
				return  a.cost < b.cost;
			}
			else {
				return a.pen < b.pen;
			}*/
			//return a.cost + a.pen < b.cost + b.pen;
			return a.pen < b.pen;
			//return a.cost  < b.cost;
			//return a.year < b.year;
		};

		std::priority_queue<Position, Vec<Position>, decltype(cmp)> qu(cmp);

		auto updatePool = [&](Position& pos) {

			qu.push(pos);
			if (qu.size() < cfg->findBestPosForRuinPqSize) {
				;
			}
			else {
				qu.pop();
			}
		};

		for (int i = 0; i < rts.size(); ++i) {
			//debug(i)
			Route& rt = rts[i];

			int v = rt.head;
			int vj = customers[v].next;

			DisType oldrPc = rts[i].rPc;
			DisType rPc = std::max<DisType>(0, rt.rQ + input.datas[w].DEMAND - input.Q);
			rPc = rPc - oldrPc;

			while (v != -1 && vj != -1) {

				DisType oldrPtw = rts[i].rPtw;

				DisType rPtw = customers[v].TW_X;
				rPtw += customers[vj].TWX_;

				DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
				rPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);
				DisType aw =
					awp <= input.datas[w].DUEDATE ? std::max<DisType>(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

				DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
				rPtw += std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
				DisType avj =
					avjp <= input.datas[vj].DUEDATE ? std::max<DisType>(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
				rPtw += std::max<DisType>(0, avj - customers[vj].zv);

				rPtw = rPtw - oldrPtw;

				DisType cost = input.disOf[reCusNo(w)][reCusNo(v)]
					+ input.disOf[reCusNo(w)][reCusNo(vj)]
					- input.disOf[reCusNo(v)][reCusNo(vj)];
				LL year = (*yearTable)[reCusNo(w)][reCusNo(v)] + (*yearTable)[reCusNo(w)][reCusNo(vj)];
				year >>= 1;

				Position pos;
				pos.rIndex = i;
				pos.cost = cost;
				pos.pen = rPtw + rPc;
				pos.pos = v;
				pos.year = year;
				pos.secDis = abs(input.datas[w].polarAngle - input.datas[v].polarAngle);

				updatePool(pos);

				v = vj;
				vj = customers[vj].next;
			}
		}

		Position ret;
		int retNum = 0;
		while (!qu.empty()) {
			++retNum;
			if (qu.top().cost < ret.cost) {
				ret = qu.top();
			}
			qu.pop();
		}
		return ret;
	}

#endif // 1

		
	void ruinClearEP() {

		// 保存放入节点的路径，放入结束之后只更新这些路径的cost值
		std::unordered_set<int> insRts;
		while (EPr.rCustCnt > 0) {
			Vec<int> EPrVe = rPutCusInve(EPr);
			int pt = EPrVe[myRand->pick(EPrVe.size())];
			EPrRemoveAtPos(pt);
			//debug(pt);
			auto bestFitPos = findBestPosForRuin(pt);
			Route& r = rts[bestFitPos.rIndex];
			insRts.insert(r.routeID);
			rInsAtPos(r, bestFitPos.pos, pt);
			rUpdateAvQfrom(r, pt);
			rUpdateZvQfrom(r, pt);
			sumRtsPen();
		}
		for (auto rId : insRts) {
			Route& r = rts.getRouteByRid(rId);
			rReCalRCost(r);
		}
		sumRtsCost();
		sumRtsPen();
	}

	Vec<int> ruinGetRuinCusBySting(int runCusNum) {

		int ruinRtNum = runCusNum;
		ruinRtNum = std::min<int>(rts.cnt, ruinRtNum);

		std::set<int> ridSet;
		Vec<int> ruinBeginCusArr;
		ruinBeginCusArr.reserve(ruinRtNum);

		Vec<CircleSector> angs(rts.cnt);
		Vec<int> range(rts.cnt);
		for (int i = 0; i < rts.cnt; ++i) {
			auto ve = rPutCusInve(rts[i]);
			angs[i].initialize(input.datas[ve[0]].polarAngle);
			for (int j = 1; j < ve.size(); ++j) {
				angs[i].extend(input.datas[ve[j]].polarAngle);
			}
			range[i] = CircleSector::positive_mod(angs[i].end - angs[i].start);
		}

		int index = 0;
		for (int i = 1; i < rts.cnt; ++i) {
			if (range[i] > range[index]) {
				index = i;
			}
		}

		Route& firstR = rts[index];
		auto cusInArr = rPutCusInve(firstR);

		int v = cusInArr[myRand->pick(firstR.rCustCnt)];
		ridSet.insert(firstR.routeID);
		ruinBeginCusArr.push_back(v);

		while (ridSet.size() < ruinRtNum) {

			v = ruinBeginCusArr[myRand->pick(ruinBeginCusArr.size())];

			for (int wpos = 0; wpos < (input.custCnt - 1) && ridSet.size() < ruinRtNum; ++wpos) {
				int w = input.allCloseOf[v][wpos];
				int wrId = customers[w].routeID;
				if (wrId != -1) {
					if (ridSet.count(wrId) > 0) {
						;
					}
					else {
						ruinBeginCusArr.push_back(w);
						ridSet.insert(wrId);
						break;
					}
				}
			}
		}

		Vec<int> runCus;
		runCus.reserve(ruinRtNum/rts.cnt*input.custCnt);

		for (int rcus : ruinBeginCusArr) {

			Route& r = rts.getRouteByRid(customers[rcus].routeID);
			int rCusNum = r.rCustCnt;

			//获取ruin一条路径上要ruin的cus的数量
			int ruinCusNumInOneRoute = std::max<int>(rCusNum * 0.3, 1);
			ruinCusNumInOneRoute = std::min<int>(rCusNum - 1, ruinCusNumInOneRoute);

			int pt = rcus;
			int isruinedNum = 0;
			//rNextDisp(r);
			while (pt <= input.custCnt && ++isruinedNum < ruinCusNumInOneRoute) {
				int ptn = customers[pt].next;
				runCus.push_back(pt);
				if (runCus.size() >= runCusNum) {
					return runCus;
				}
				pt = ptn;
			}
		}
		return runCus;
	}

	Vec<int> ruinGetRuinCusByRound(int runCusNum) {

		/*Vec<CircleSector> angs(rts.cnt);
		Vec<int> range(rts.cnt);
		for (int i = 0; i < rts.cnt; ++i) {
			auto ve = rPutCusInve(rts[i]);
			angs[i].initialize(input.datas[ve[0]].polarAngle);
			for (int j = 1; j < ve.size(); ++j) {
				angs[i].extend(input.datas[ve[j]].polarAngle);
			}
			range[i] = CircleSector::positive_mod(angs[i].end - angs[i].start);
		}

		int index = 0;
		for (int i = 1; i < rts.cnt; ++i) {
			if (range[i] > range[index]) {
				index = i;
			}
		}

		Route& firstR = rts[index];
		auto cusInArr = rPutCusInve(firstR);*/

		//int v = cusInArr[myRand->pick(firstR.rCustCnt/2)];
		int v = myRand->pick(input.custCnt)+1;
		//int v = 0;

		Vec<int> runCus;
		runCus.reserve(runCusNum);

		//auto& wposArr = myRandX->getMN(50, runCusNum);
		for (int i = 0;i< runCusNum;++i) {
			//int w = input.sectorClose[v][wpos];
			int wpos = i;
			int w = input.allCloseOf[v][wpos];
			if (customers[w].routeID != -1) {
				runCus.push_back(w);
			}
		}
		return runCus;
	}
		
	Vec<int> ruinGetRuinCusBySec(int runCusNum) {

		int v = myRand->pick(input.custCnt)+1;
		//int v = 0;

		Vec<int> runCus;
		runCus.reserve(runCusNum);

		for (int wpos = 0;wpos< runCusNum;++wpos) {
			int w = input.sectorClose[v][wpos];
			//int w = input.allCloseOf[v][wpos];
			if (customers[w].routeID != -1) {
				runCus.push_back(w);
			}
		}
		return runCus;
	}

	bool ruinLocalSearch(int ruinCusNum) {
			
		SolClone sClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty,RoutesCost);
		gamma = 1;

		//TODO[lyh][1]:一种选择ruin cus的方式
		/*Vec<int> rIndexArr(rts.cnt, 0);
		std::iota(rIndexArr.begin(), rIndexArr.end(), 0);
		auto cmp = [&](int i,int j) {
			return rts[i].routeCost / rts[i].rCustCnt 
			< rts[j].routeCost / rts[j].rCustCnt;
		};
		sort(rIndexArr.begin(), rIndexArr.end(), cmp);*/

		//auto& rIndexArr = myRandX->getMN(rts.cnt, min(rts.cnt, ruinRtNum));
			
		//TODO[lyh][1]:这里可能可以去掉，如果之前每一条路径的cost都维护的话
		//TODO[lyh][1]:但是接到扰动后面就不太行了
		reCalRtsCostSumCost();

		Vec<int> cuses;
		cuses.resize(input.custCnt);

		Vec<int> ruinCus;
		int kind = myRand->pick(3);
		if (kind == 0) {
			ruinCus = ruinGetRuinCusByRound(ruinCusNum);
		}
		else if(kind==1){
			ruinCus = ruinGetRuinCusBySting(ruinCusNum);
		}
		else {
			ruinCus = ruinGetRuinCusBySec(ruinCusNum);
		}

		std::unordered_set<int> rIds;
		for (int cus: ruinCus) {
			Route& r = rts.getRouteByRid(customers[cus].routeID);
			rIds.insert(r.routeID);
			rRemoveAtPos(r, cus);
			cuses.push_back(cus);
			EPpush_back(cus);
		}

		for (auto rid : rIds) {
			Route& r = rts.getRouteByRid(rid);
			rUpdateAvQfrom(r,r.head);
			rUpdateZvQfrom(r,r.tail);
			rReCalRCost(r);
		}
		sumRtsCost();
		sumRtsPen();

		// 局部搜索下一次的邻域
		ConfSet cs(input.custCnt);
		for (int v : cuses) {
			cs.ins(v);
			for (int i = 0; i < cfg->ruinLocalSearchNextNeiBroad; ++i) {
				cs.ins(input.allCloseOf[v][i]);
			}
		}
		cuses = std::move(Vec<int>(cs.ve.begin(), cs.ve.begin() + cs.cnt));

		//TODO[lyh][0]:更新惩罚值可以更快

		// hust::Log(Log::Info) << "ruinCusNum: " << EPr.rCustCnt <<" ruinRtNum: " << ruinRtNum << std::endl;
		//TODO[lyh][0]:将EP中的点拿出来放进解里面
		ruinClearEP();

		//reCalRtsCostSumCost(); return true;

		bool ruinIsRepair = repair();
			
		if (ruinIsRepair) {
			//mRLLocalSearch({});
			mRLLocalSearch(cuses);

			if (RoutesCost < sClone.RoutesCost) {
				return true;
			}
			else {
				setCurSolBy(sClone);
				return false;
			}
		}
		else {
			setCurSolBy(sClone);
			//reCalRtsCostAndPen();
			return false;
		}
		//debug(RoutesCost);

		return false;
	}
		
	bool ejectLocalSearch() {

		minEPcus = IntInf;
		squIter += cfg->yearTabuLen + cfg->yearTabuRand;
		LL maxOfPval = -1;
		gamma = 0;

		#ifdef SAVE_LOCAL_BEST_SOL
		SolClone solClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
		SolClone gBestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
		auto cloneP = P;

		LL maxContiNode = 1024;

		LL contiNoReduce = 1;
		int minRPSolCnt = 1;

		auto updateGBestSolPool = [&]() {

			auto& curBestSol = gBestSol;

			if (EPr.rCustCnt < curBestSol.EPr.rCustCnt) {
				minRPSolCnt = 1;
				gBestSol = SolClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
				return true;
			}
			else if (EPr.rCustCnt == curBestSol.EPr.rCustCnt) {
				++minRPSolCnt;
				if (myRand->pick(minRPSolCnt) == 0) {
					gBestSol = SolClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
				}
				return false;
			}
			return false;
		};

		int curUse = 0;// 0 global 1 pool

		#endif // SAVE_LOCAL_BEST_SOL

		LL EpCusNoDown = 1;

		while (!lyhTimer.isTimeOut()) {

			++EPIter;

			EPNodesCanEasilyPut();

			if (EPr.rCustCnt < minEPcus) {
				minEPcus = EPr.rCustCnt;
				EpCusNoDown = 1;
			}else{

				++EpCusNoDown;
				if (EpCusNoDown % 10000 == 0) {

					//cfg->minKmax = 1;
					// 调整 minKmax 在1 2 之间切换
					cfg->minKmax = 3 - cfg->minKmax;
					debug(cfg->minKmax);
				}
			}
				
			minEPcus = std::min<int>(minEPcus, EPr.rCustCnt);
			if (EPsize() == 0 && penalty == 0) {
				return true;
			}

				
			#ifdef SAVE_LOCAL_BEST_SOL

			auto isUp = updateGBestSolPool();
			if (isUp) {
				contiNoReduce = 1;
			}
			else {
				++contiNoReduce;
			}
			//debug(minRPSolCnt)

			if (contiNoReduce % maxContiNode == 0) {

				/*deOut(minEPcus)
				debug(contiNoReduce)*/
				//debug(minRPSolCnt)
				minRPSolCnt = 1;

				if (curUse == 0) {
					solClone = SolClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
				}
					
				if (myRand->pick(5) < 4 ) {
					if (curUse == 1) {
						P = cloneP;
						setCurSolBy(solClone);
						//debug("sclo")
						curUse = 0;
					}
				}
				else {
					if (curUse == 0) {
						cloneP = P;

						debug("glob")
						debug(EPsize())

						setCurSolBy(gBestSol);
						ejectPatternAdjustment();
						debug(EPsize())
						//ejectPatternAdjustment();
						//patternAdjustment(-1, 2*input.custCnt);
							
						curUse = 1;
					}
				}

				patternAdjustment(-1, input.custCnt*2);
				squIter += cfg->yearTabuLen + cfg->yearTabuRand;

				continue;
			}
			#endif // SAVE_LOCAL_BEST_SOL

			Vec<int> EPrVe = rPutCusInve(EPr);
			int top = EPrVe[myRand->pick(EPrVe.size())];
			Position bestP = findBestPosInSol(top);
			Route& r = rts[bestP.rIndex];
			EPremoveByVal(top);

			P[top] += cfg->Pwei0;
			maxOfPval = std::max<int>(P[top], maxOfPval);

			if (maxOfPval >= 1000) {
				maxOfPval = 0;
				for (auto& i : P) {
					i = i * 0.7 + 1;
					maxOfPval = std::max<DisType>(maxOfPval,i);
				}
				//cout << "p";
			}

			#if CHECKING
			DisType oldpenalty = PtwNoWei + Pc;
			#endif

			rInsAtPos(r, bestP.pos, top);
			rUpdateAvQfrom(r, top);
			rUpdateZvQfrom(r, top);
			sumRtsPen();
			resetConfRts();

			#if CHECKING
			lyhCheckTrue((oldpenalty + bestP.pen == PtwNoWei + Pc));
			#endif // CHECKING

			if (bestP.pen == 0) {
				continue;
			}
			else {

				bool squ = squeeze();
				//t1.disp();

				if (squ == true) {
					continue;
				}
				else {

					auto& XSet = ejeNodesAfterSqueeze;

					for (eOneRNode& en : XSet) {
						for (int c : en.ejeVe) {
							/*int cpre = customers[c].pre > input.custCnt ? 0 : customers[c].pre;
							int cnext = customers[c].next > input.custCnt ? 0 : customers[c].next;
							(*yearTable)[cpre][c] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);
							(*yearTable)[c][cnext] = squIter + cfg->yearTabuLen + myRand->pick(cfg->yearTabuRand);*/
							P[c] += cfg->Pwei1;
							maxOfPval = std::max<DisType>(P[c], maxOfPval);
								
							//P[c] += std::max<DisType>(log(P[c]), cfg->Pwei1);
							//debug(std::max<DisType>(log(P[c]), cfg->Pwei1))
						}
					}

					doEject(XSet);
					int Irand = input.custCnt / EPr.rCustCnt/4;
					Irand = std::max<int>(Irand,100);
					patternAdjustment(Irand);
					//system("pause");
					//saveOutAsSintefFile();
					//patternMakeBigBigger();
					//system("pause");
					//saveOutAsSintefFile();

					//end our method 2
				}
			}
		}
		return false;
	}

	bool patternAdjustment(int Irand = -1) {

		int I1000 = myRand->pick(cfg->Irand);
		if (Irand > 0) {
			I1000 = Irand;
		}

		LL iter = 0;
		gamma = 0;

		Timer t1(1);

		Vec<int> kindSet = { 0,1,6,7,/*8,9,10,2,3,4,5*/ };

		int N = cfg->patternAdjustmentNnei;
		int m = std::min<int>(cfg->patternAdjustmentGetM,N);

		auto getDelt0MoveRandomly = [&]() {

			TwoNodeMove ret;

			while (!t1.isTimeOut()) {

				int v = myRand->pick(input.custCnt) + 1;

				if (customers[v].routeID == -1) {
					continue;
				}

				m = std::max<DisType>(1, m);
				myRandX->getMN(N, m);
				Vec<int>& ve = myRandX->mpLLArr[N];
				for (int i = 0; i < m; ++i) {
					int wpos = ve[i];

					int w = input.allCloseOf[v][wpos];
					if (customers[w].routeID == -1
						//|| customers[w].routeID == customers[v].routeID
						) {
						continue;
					}

					for (int kind : kindSet) {

						DeltPen d;
						if (kind == 6 || kind == 7) {
							d = estimatevw(kind, v, w, 1);
						}
						else {
							d = estimatevw(kind, v, w);
						}

						#ifdef ATTRIBUTETABU
						if (d.deltPc + d.deltPtw == 0) {
							TwoNodeMove m(v, w, kind, d);
							ret = m;
							if (squIter >= getYearOfMove(m)) {
								return m;
							}
						}
						#else

						#endif // ATTRBUTETABU

					}
				}
			}

			return ret;
		};

		++squIter;

		do {

			TwoNodeMove bestM = getDelt0MoveRandomly();
			//if (myRand->pick(3) != 0) {

			/*while (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0 && !t1.isTimeOut()) {
				bestM = getDelt0MoveRandomly();
			}*/
				
			if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {
				break;
			}

			updateYearTable(bestM);
			doMoves(bestM);
			++squIter;

		} while (++iter < I1000 && !t1.isTimeOut());
		//debug(iter)
		sumRtsPen();
		return true;
	}

	bool perturb(Vec<int> cusArr) {

		Vec<int> kindSet = { 0,1,6,7,/*8,9,10,2,3,4,5*/ };

		int N = cfg->patternAdjustmentNnei;
		int m = std::min<int>(cfg->patternAdjustmentGetM, N);

		auto getDelt0MoveForCus = [&](int v)-> TwoNodeMove {

			TwoNodeMove ret;

			if (customers[v].routeID == -1) {
				return ret;
			}

			for (int wpos = 0; wpos < N; ++wpos) {

				int w = input.allCloseOf[v][wpos];
				if (customers[w].routeID == -1
					//|| customers[w].routeID == customers[v].routeID
					) {
					continue;
				}

				for (int kind : kindSet) {

					DeltPen d;
					if (kind == 6 || kind == 7) {
						d = estimatevw(kind, v, w, 1);
					}
					else {
						d = estimatevw(kind, v, w);
					}

					#ifdef ATTRIBUTETABU
					if (d.deltPc + d.deltPtw == 0) {
						TwoNodeMove m(v, w, kind, d);
						ret = m;
						if (squIter >= getYearOfMove(m)) {
							return m;
						}
					}
					#else

					#endif // ATTRBUTETABU

				}
			}
			return ret;
		};

		for(int c:cusArr) {

			TwoNodeMove bestM = getDelt0MoveForCus(c);
			if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw == 0) {
				updateYearTable(bestM);
				doMoves(bestM);
			}
			++squIter;
		}

		sumRtsPen();

		return true;
	}

	Vec<eOneRNode> ejectFromPatialSol() {

		Vec<eOneRNode>ret;

		Vec<int>confRSet;

		confRSet.reserve(PtwConfRts.cnt + PcConfRts.cnt);
		for (int i = 0; i < PtwConfRts.cnt; ++i) {
			confRSet.push_back(PtwConfRts.ve[i]);
		}
		for (int i = 0; i < PcConfRts.cnt; ++i) {
			confRSet.push_back(PcConfRts.ve[i]);
		}

		for (int id : confRSet) {

			Route& r = rts.getRouteByRid(id);

			eOneRNode retNode;
			int tKmax = cfg->minKmax;
			//tKmax = cfg->maxKmax;

			while (tKmax <= cfg->maxKmax) {

				auto en = ejectOneRouteOnlyP(r, 2, tKmax);

				if (retNode.ejeVe.size() == 0) {
					retNode = en;
				}
				else {

					//if (retNode.ejeVe.size() > 1) {
					//if (en.ejeVe.size()<=3) {

						//bool s1 = en.getMaxEle() < retNode.getMaxEle();
						// 
						bool s3 = en.Psum * retNode.ejeVe.size() < retNode.Psum * en.ejeVe.size();
						bool s1 = en.Psum < retNode.Psum;
						bool s2 = en.ejeVe.size() * en.Psum < retNode.Psum * retNode.ejeVe.size();
							
						if (s1 && s2 && s3) {
						//if (satisfy1) {
							/*deOut(en.Psum)debug(en.ejeVe.size())
							deOut(retNode.Psum)debug(retNode.ejeVe.size())*/
							//debug("satisfy12")
							retNode = en;
						}
					//}
				}
				++tKmax;
			}

			//debug(retNode.ejeVe.size())
			//eOneRNode retNode = ejectOneRouteOnlyP(r, 2, cfg->maxKmax);
			//auto en = ejectOneRouteMinPsumGreedy(r);

			if (retNode.ejeVe.size() == 0) {
				// cout << "kmax fail ";
				retNode = ejectOneRouteMinPsumGreedy(r);
			}
			//else {

			//	if (en.Psum < retNode.Psum) {
			//		//bool satisfy2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();
			//		bool satisfy2 = en.ejeVe.size() > cfg->maxKmax;
			//		//bool satisfy2 = true;
			//		if (satisfy2) {
			//			deOut(en.Psum)debug(en.ejeVe.size())
			//			deOut(retNode.Psum)debug(retNode.ejeVe.size())
			//			debug(satisfy2)
			//			retNode = en;
			//		}
			//	}
			//}

#if CHECKING
			lyhCheckTrue(retNode.ejeVe.size()>0);
#endif // CHECKING

			ret.push_back(retNode);
		}

		return ret;
	}

	eOneRNode ejectOneRouteOnlyHasPcMinP(Route& r,int Kmax) {

		eOneRNode noTabuN(r.routeID);
		noTabuN.Psum = 0;

		Vec<int> R = rPutCusInve(r);

		auto cmpMinP = [&](const int& a, const int& b) {

			if (P[a] == P[b]) {
				return input.datas[a].DEMAND > input.datas[b].DEMAND;
			}
			else {
				return P[a] > P[b];
			}
			return false;
		};

		auto cmpMinD = [&](const int& a, const int& b) {

			if (input.datas[a].DEMAND == input.datas[b].DEMAND) {
				return P[a] > P[b];
			}
			else {
				return input.datas[a].DEMAND > input.datas[b].DEMAND; 
			}
			return false;
		};

		std::priority_queue<int, Vec<int>, decltype(cmpMinD)> qu(cmpMinD);
			
		for (const int& c : R) {
			qu.push(c);
		}

		DisType rPc = r.rPc;
		DisType rQ = r.rQ;

		while (rPc > 0) {

			int ctop = qu.top();

			qu.pop();
			rQ -= input.datas[ctop].DEMAND;
			rPc = std::max<DisType>(0, rQ - input.Q);
			noTabuN.ejeVe.push_back(ctop);
			noTabuN.Psum += P[ctop];
		}

		return noTabuN;

	}

	eOneRNode ejectOneRouteOnlyP(Route& r, int kind, int Kmax) {

		eOneRNode noTabuN(r.routeID);
			
		//int sameCnt = 1;
		eOneRNode etemp(r.routeID);
		etemp.Psum = 0;

#if CHECKING
		DisType rQStart = r.rQ;
		DisType PtwStart = r.rPtw;
		DisType PcStart = r.rPc;
		DisType depot0TW_XStart = customers[r.head].TWX_;
		DisType depotN1TW_XStart = customers[r.tail].TW_X;
#endif // CHECKING

		DisType rQ = r.rQ;

		auto updateEje = [&]() {
			if (noTabuN.ejeVe.size() == 0) {
				noTabuN = etemp;
			}
			else {
				if (etemp.Psum < noTabuN.Psum) {
					if (etemp.ejeVe.size() * etemp.Psum < noTabuN.Psum * noTabuN.ejeVe.size()) {
					//if (etemp.ejeVe.size() <= noTabuN.ejeVe.size()) {
						noTabuN = etemp;
					}
				}
			}
				
			//else if (etemp.Psum == noTabuN.Psum) {
			//	++sameCnt;
			//	if (etemp.ejeVe.size() < noTabuN.ejeVe.size()) {
			//		noTabuN = etemp;
			//		debug(11)
			//	}
			//	/*if (myRand->pick(sameCnt) == 0) {
			//		
			//	}*/
			//}
		};

		auto getAvpOf = [&](int v) {

			int pre = customers[v].pre;
			DisType avp = customers[pre].av +
				input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(v)];
			return avp;
		};

		auto updateAvfromSubRoute = [&](int n) {

			int pre = customers[n].pre;

			while (n != -1) {

				customers[n].avp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(n)];
				customers[n].av = customers[n].avp > input.datas[n].DUEDATE ? input.datas[n].DUEDATE
					:std::max<DisType>(customers[n].avp, input.datas[n].READYTIME);
				customers[n].TW_X =std::max<DisType>(0, customers[n].avp - input.datas[n].DUEDATE);
				customers[n].TW_X += customers[pre].TW_X;
				pre = n;
				n = customers[n].next;
			}
		};

		auto delOneNodeInPreOrder = [&](int n) {

			rQ -= input.datas[n].DEMAND;
			etemp.ejeVe.push_back(n);
			etemp.Psum += P[n];

			int next = customers[n].next;
			customers[next].pre = customers[n].pre;
			updateAvfromSubRoute(next);
		};

		// 使用公式二 利用v的avp计算Ptw
		auto getPtwUseEq2 = [&](int v) {

			DisType avp = getAvpOf(v);
			DisType ptw =std::max<DisType>(0, avp - customers[v].zv) + customers[v].TWX_;
			int pre = customers[v].pre;
			ptw += customers[pre].TW_X;
			return ptw;
		};

		auto restoreOneNodePreOrder = [&](int n) {

			rQ += input.datas[n].DEMAND;
			etemp.ejeVe.pop_back();
			etemp.Psum -= P[n];

			int next = customers[n].next;
			customers[next].pre = n;
			updateAvfromSubRoute(n);
		};

		auto getPtwIfRemoveOneNode = [&](int head) {

			int pt = head;
			int pre = customers[pt].pre;
			int next = customers[pt].next;

#if CHECKING
			lyhCheckTrue(pre !=-1);
			lyhCheckTrue(next != -1);
#endif // CHECKING

			DisType ptw = 0;

			while (next != -1) {

				//debug(pt)
				DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(next)];
				ptw =std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[pre].TW_X;

				if (customers[pre].TW_X > 0) { // 剪枝 删除ik之后 前面的时间窗没有消除
					return etemp;
				}

				rQ -= input.datas[pt].DEMAND;
				etemp.ejeVe.push_back(pt);
				etemp.Psum += P[pt];

				if (ptw == 0 && rQ - input.Q <= 0) {
					updateEje();
				}

				rQ += input.datas[pt].DEMAND;
				etemp.ejeVe.pop_back();
				etemp.Psum -= P[pt];

				pre = pt;
				pt = next;
				next = customers[next].next;
			}

			return etemp;

		};

		auto restoreWholeR = [&]() {

			int pt = r.head;
			int ptn = customers[pt].next;

			while (pt != -1 && ptn != -1) {

				customers[ptn].pre = pt;
				pt = ptn;
				ptn = customers[ptn].next;
			}
			rUpdateAvfrom(r, r.head);
		};

		Vec<int> R = rPutCusInve(r);

		Vec<int> ptwArr;

		if (r.rPtw > 0) {

			ptwArr = R;
			Kmax = std::min<int>(Kmax, ptwArr.size() - 1);
		}
		else if (r.rPc > 0) {

			return ejectOneRouteOnlyHasPcMinP(r, Kmax);
			ptwArr = R;
			Kmax = std::min<int>(Kmax, ptwArr.size() - 1);
				
		}
		else {
			return {};
		}

		int k = 0;
		int N = ptwArr.size() - 1;
		Vec<int> ve(Kmax + 1, -1);

		/*etemp = getPtwIfRemoveOneNode(r.head);
		updateEje();*/

		//debug(ptwArr.size())

		do {

			if (k < Kmax && ve[k] < N) { // k increase

				++k;
				ve[k] = ve[k - 1] + 1;

				int delv = ptwArr[ve[k]];

				// 考虑相同所有Psum 的方案 >
				// 不考虑相同所有Psum 的方案 >=
				while (etemp.Psum + P[delv] > noTabuN.Psum && ve[k] < N) {
					++ve[k];
					delv = ptwArr[ve[k]];
				}

				delOneNodeInPreOrder(delv);

				DisType ptw = getPtwUseEq2(customers[delv].next);

				if (ptw == 0 && rQ - input.Q <= 0) {
					updateEje();
				}

				if (ve[k] == N) {
					restoreOneNodePreOrder(delv);
					if (k - 1 >= 1) {
						restoreOneNodePreOrder(ptwArr[ve[k - 1]]);
					}
				}
				else if (k == Kmax) {
					restoreOneNodePreOrder(delv);
				}

			}
			else if (k == Kmax && ve[k] < N) { // i(k) increase

				++ve[k];

				// 考虑相同所有Psum 的方案 >
				// 不考虑相同所有Psum 的方案 >=
				if (etemp.Psum > noTabuN.Psum) {
					;
				}
				else {
					eOneRNode ret = getPtwIfRemoveOneNode(ptwArr[ve[k]]);
				}

				ve[k] = N;

				if (k - 1 >= 1) {
					restoreOneNodePreOrder(ptwArr[ve[k - 1]]);
				}

			}
			else if (ve[k] == N) {

				k--;
				++ve[k];

				int delv = ptwArr[ve[k]];
				delOneNodeInPreOrder(delv);

				DisType ptw = getPtwUseEq2(customers[delv].next);

				if (ptw == 0 && rQ - input.Q <= 0) {
					updateEje();
				}

				if (ve[k] == N) {
					restoreOneNodePreOrder(delv);

					if (k - 1 >= 1) {
						restoreOneNodePreOrder(ptwArr[ve[k - 1]]);
					}
				}
			}

		} while (!(k == 1 && ve[k] == N));

			
#if CHECKING

		lyhCheckTrue(r.rQ == rQStart);
		lyhCheckTrue(PcStart == r.rPc);
		lyhCheckTrue(PtwStart == r.rPtw);
		lyhCheckTrue(PtwStart == customers[r.head].TWX_);
		lyhCheckTrue(PtwStart == customers[r.tail].TW_X);
		lyhCheckTrue(customers[r.head].QX_ == rQStart);
		lyhCheckTrue(customers[r.tail].Q_X == rQStart);

		Vec<int> v1 = rPutCusInve(r);
		Vec<int> v2;

		for (int pt = customers[r.tail].pre; pt <= input.custCnt; pt = customers[pt].pre) {
			v2.push_back(pt);
		}
		lyhCheckTrue(v1.size() == v2.size());
#endif // CHECKING

		return noTabuN;
	}

	eOneRNode ejectOneRouteMinPsumGreedy
	(Route& r, eOneRNode cutBranchNode = eOneRNode()) {

		eOneRNode ret(r.routeID);
		ret.Psum = 0;
		Vec<int> R = rPutCusInve(r);

		auto cmp = [&](const int& a,const int& b) {

			if (P[a] == P[b]) {
				/*return input.datas[a].DUEDATE - input.datas[a].READYTIME <
					input.datas[b].DUEDATE - input.datas[b].READYTIME;*/
					return input.datas[a].DEMAND > input.datas[b].DEMAND;
			}
			else {
				return P[a] > P[b];
			}
			return false;
		};

		std::priority_queue<int, Vec<int>, decltype(cmp)> qu(cmp);

		for (const int& c : R) {
			qu.push(c);
		}

		DisType curPtw = r.rPtw;
			
		Vec<int> noGoodToPtw;
		noGoodToPtw.reserve(R.size());

		while (curPtw > 0) {
				
			int ctop = qu.top();
			qu.pop();

			if (ret.Psum >= cutBranchNode.Psum) {
				return eOneRNode();
			}

			int pre = customers[ctop].pre;
			int nex = customers[ctop].next;
			DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(nex)];
			DisType ptw =std::max<DisType>(0, avnp - customers[nex].zv) + customers[nex].TWX_ + customers[pre].TW_X;

			if (ptw < curPtw) {

				rRemoveAtPos(r,ctop);
				rUpdateAvfrom(r, pre);
				rUpdateZvfrom(r, nex);
				curPtw = ptw;
				ret.ejeVe.push_back(ctop);
				ret.Psum += P[ctop];

				for (auto c : noGoodToPtw) {
					qu.push(c);
				}
				noGoodToPtw.clear();
			}
			else {
				noGoodToPtw.push_back(ctop);
			}
		}

		while (r.rPc > 0) {
			//debug(r.rPc)

			if (ret.Psum >= cutBranchNode.Psum) {
				return eOneRNode();
			}

			int ctop = qu.top();
			qu.pop();
			rRemoveAtPos(r, ctop);
			ret.ejeVe.push_back(ctop);
			ret.Psum += P[ctop];
		}

		//debug(r.rCustCnt)
		rRemoveAllCusInR(r);
		//debug(r.rCustCnt)

		for (int v : R) {
			rInsAtPosPre(r, r.tail, v);
		}
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);

		return ret;
	}
		
	bool setCurSolBy(SolClone& sClone) {

		customers = sClone.customers;
		rts = sClone.rts;
		PtwNoWei = sClone.PtwNoWei;
		Ptw = sClone.Ptw;
		Pc = sClone.Pc;
		penalty = sClone.penalty;
		EPr = sClone.EPr;
		RoutesCost = sClone.RoutesCost;
		resetConfRts();

		return true;
	}
		
	bool resetSol() {

		alpha = 1;
		beta = 1;
		gamma = 1;
		//squIter = 0;
		penalty = 0;
		Ptw = 0;
		PtwNoWei = 0;
		Pc = 0;
		//minEPSize = inf;
		RoutesCost = 0;
			
		for (int i = 0; i < rts.cnt; ++i) {
			rReset(rts[i]);
		}
		rts.reSet();
		rReset(EPr);
		initEPr();
		return true;
	}

	bool minimizeRN() {

		gamma = 0;
		initSolution();
		bool isSucceed = false;
		lyhTimer.reStart();

		while (!lyhTimer.isTimeOut()&& rts.cnt > ourTarget) {

			removeOneRouteRandomly();

			fill(P.begin(), P.end(), 1);
			bool isDelete = ejectLocalSearch();
			Log(Log::Level::Warning) << "rts.size(): " << rts.size() << std::endl;
			if (rts.size() <= ourTarget) {
				if ((EPsize() == 0 && penalty == 0)) {
					isSucceed = true;
				}
				break;
			}
		} 

		return isSucceed;
	}

	TwoNodeMove naRepairGetMoves(std::function<bool(TwoNodeMove & t, TwoNodeMove & bestM)>updateBestM) {

		TwoNodeMove bestM;

		Vec<int> confRts;

		confRts.insert(confRts.begin(),PtwConfRts.ve.begin(), PtwConfRts.ve.begin()+ PtwConfRts.cnt);
		confRts.insert(confRts.begin(), PcConfRts.ve.begin(), PcConfRts.ve.begin()+ PcConfRts.cnt);
		//debug(confRts.size())
		for (int rId : confRts) {
			Route& r = rts.getRouteByRid(rId);
			Vec<int> cusV = rPutCusInve(r);
			for (int v : cusV) {

				if (customers[v].routeID == -1) {
					continue;
				}
				for (int wpos = 0; wpos < cfg->naRepairGetMovesNei; ++wpos) {

					int w = input.allCloseOf[v][wpos];
					if (customers[w].routeID == -1) {
						continue;
					}
					for (int i = 0; i < 9; ++i) {

						TwoNodeMove m;
						m = TwoNodeMove(v, w, i, estimatevw(i, v, w, 0));
						updateBestM(m, bestM);
					}
				}
			}
		}
		return bestM;
	}

	bool naRepair() {

		for (int i = 0; i < rts.cnt; ++i) {
			rts[i].rWeight = 1;
		}

		resetConfRts();
		sumRtsPen();

		gamma = 1;

		squIter += cfg->yearTabuLen + cfg->yearTabuRand;

		auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

			if (t.deltPen.deltPc == DisInf || t.deltPen.deltPtw == DisInf) {
				return false;
			}
			/*if (t.deltPen.deltPc + t.deltPen.deltPtw < 0) {
				if (t.deltPen.deltPc + t.deltPen.deltPtw + t.deltPen.deltCost
					< bestM.deltPen.deltPc + bestM.deltPen.deltPtw + bestM.deltPen.deltCost) {
					bestM = t;
				}
			}*/
			if (t.deltPen.PcOnly + t.deltPen.PtwOnly < 0) {
				if (t.deltPen.PcOnly + t.deltPen.PtwOnly + t.deltPen.deltCost
					< bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly + bestM.deltPen.deltCost) {
					bestM = t;
				}
			}
			return true;
		};

		LL contiNotDe = 0;

		SolClone sClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty, RoutesCost);

		while (penalty > 0 && !lyhTimer.isTimeOut()) {
				
			if (contiNotDe > cfg->repairExitStep) {
				break;
			}

			TwoNodeMove bestM = getMovesRandomly(updateBestM);
			/*if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
				bestM = naRepairGetMoves(updateBestM);
			}*/

			if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
			//if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {

				++contiNotDe;
				continue;
				if (bestM.v == 0 && bestM.w == 0) {
					return false;
				}
			}

			//update(*yearTable)(bestM);
			int rvId = customers[bestM.v].routeID;
			int rwId = customers[bestM.w].routeID;

			doMoves(bestM);

			++squIter;

			sumRtsPen();

			RoutesCost += bestM.deltPen.deltCost;

			resetConfRtsByOneMove({ rvId,rwId });
			//addWeightToRoute(bestM);

			if (penalty < sClone.penalty) {
				sClone = SolClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty, RoutesCost);
				//lyhCheckTrue(contiNotDe<40);
				contiNotDe = 1;
			}
			else {
				++contiNotDe;
			}
		}

		if (penalty == 0) {
			setCurSolBy(sClone);
			return true;
		}
		else {
			setCurSolBy(sClone);
			return false;
		}
		return false;
	}

	bool repair() {

		gamma = 1;
		return naRepair();
	}

	bool mRLLocalSearch(Vec<int> newCus) {

		gamma = 1;
		reCalRtsCostSumCost();

		TwoNodeMove MRLbestM;

		auto MRLUpdateM = [&](TwoNodeMove& m) {

			if (m.deltPen.deltPc <= 0 && m.deltPen.deltPtw <= 0) {
				if (m.deltPen.deltCost < 0) {
					if (m.deltPen.deltCost < MRLbestM.deltPen.deltCost) {
						MRLbestM = m;
					}
				}else{
					if (getYearOfMove(m) < squIter) {
						MRLbestM = m;
					}
				}
			}
		};
			
		if (newCus.size() == 0) {
			newCus = myRandX->getMN(input.custCnt+1, input.custCnt+1);
			unsigned shuseed = (env.seed % hust::Mod) + ((hust::myRand->pick(10000007))) % hust::Mod;
			std::shuffle(newCus.begin(), newCus.end(), std::default_random_engine(shuseed));
		}
			
		auto getMovesGivenRange = [&](int range) {

			MRLbestM.reSet();
				
			for (int v : newCus) {
				if (customers[v].routeID == -1) {
					continue;
				}
				for (int wpos = 0; wpos < range; ++wpos) {

					int w = input.allCloseOf[v][wpos];
					//int w = input.sectorClose[v][wpos];
					if (customers[w].routeID == -1) {
						continue;
					}
					for (int i = 0; i < 15; ++i) {

						if (MRLbestM.deltPen.deltCost < 0) {
							return MRLbestM;
						}

						TwoNodeMove m;
						m = TwoNodeMove(v, w, i, estimatevw(i, v, w,0));
						MRLUpdateM(m);
					}
				}
			}
			return MRLbestM;
		};

		auto getAllRange = [&]() {

			MRLbestM.reSet();
				
			for (int v = 1;v<=input.custCnt;++v) {

				if (customers[v].routeID == -1) {
					continue;
				}
				for (int wpos = 0; wpos < cfg->mRLLSgetAllRange; ++wpos) {

					int w = input.allCloseOf[v][wpos];
					//int w = input.sectorClose[v][wpos];
					if (customers[w].routeID == -1) {
						continue;
					}
					for (int i = 0; i < 15; ++i) {

						if (MRLbestM.deltPen.deltCost < 0) {
							return MRLbestM;
						}

						TwoNodeMove m;
						m = TwoNodeMove(v, w, i, estimatevw(i, v, w,1));
						MRLUpdateM(m);
					}
				}
			}
			return MRLbestM;
		};
			
		SolClone bestS(customers, rts, 
			EPr, Pc, Ptw, PtwNoWei, penalty,RoutesCost);
			
		alpha = 1;
		beta = 1;
		for (int i = 0; i < rts.cnt; ++i) {
			rts[i].rWeight = 1;
		}
		squIter += cfg->yearTabuLen + cfg->yearTabuRand;

		Vec<int>& ranges = cfg->mRLLocalSearchRange;

		while (!lyhTimer.isTimeOut()) {

			TwoNodeMove bestM;
			//TwoNodeMove bestM 
			for (int i = 0; i < ranges.size();++i) {
				if (bestM.deltPen.PtwOnly > 0
					|| bestM.deltPen.PcOnly > 0
					|| bestM.deltPen.deltCost >= 0
					) {
					bestM = getMovesGivenRange(ranges[i]);
				}
				else {
					break;
				}
			}

			/*if (bestM.deltPen.PtwOnly > 0
				|| bestM.deltPen.PcOnly > 0
				|| bestM.deltPen.deltCost >= 0
				) {
				bestM = getAllRange();
			}*/
				
			if (bestM.deltPen.PtwOnly > 0
				|| bestM.deltPen.PcOnly > 0
				|| bestM.deltPen.deltCost >= 0
				) {
				break;
			}
				
#if CHECKING
			Vec<Vec<int>> oldRoutes;
			Vec<int> oldrv;
			Vec<int> oldrw;

			DisType oldpenalty = penalty;
			DisType oldPtw = Ptw;
			DisType oldPc = Pc;
			DisType oldPtwNoWei = PtwNoWei;
			DisType oldPtwOnly = PtwNoWei;
			DisType oldPcOnly = Pc;
			DisType oldRcost = RoutesCost;

			Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
			Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
			oldrv = rPutCusInve(rv);
			oldrw = rPutCusInve(rw);

			int rvId = customers[bestM.v].routeID;
			int rwId = customers[bestM.w].routeID;
#endif // CHECKING

			updateYearTable(bestM);
			doMoves(bestM);
			updatePen(bestM.deltPen);
			RoutesCost += bestM.deltPen.deltCost;
#if CHECKING
			DisType penaltyafterupdatePen = penalty;
			DisType costafterplus = RoutesCost;

			sumRtsPen();
			reCalRtsCostSumCost();
			lyhCheckTrue(penaltyafterupdatePen==penalty);
			lyhCheckTrue(costafterplus == RoutesCost);

			lyhCheckTrue(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw==penalty);
			lyhCheckTrue(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly==PtwNoWei + Pc);
			lyhCheckTrue(oldRcost + bestM.deltPen.deltCost==RoutesCost);
			bool ver = verify();
			lyhCheckTrue(ver>0)
#endif // CHECKING

		}
		//TODO[lyh][1]:这个更新必须有 因为搜索工程中没有更新每一条路径的routeCost
		reCalRtsCostSumCost();
		return true;
	}

	Output saveToOutPut() {

		Output output;

		DisType rc = RoutesCost;
		DisType state = verify();

		output.runTime = lyhTimer.getRunTime();
		output.EP = rPutCusInve(EPr);
		output.minEP = minEPcus;
		output.PtwNoWei = PtwNoWei;
		output.Pc = Pc;

		output.rts.clear();
		for (int i = 0; i < rts.size(); ++i) {
			output.rts.push_back(rPutCusInve(rts[i]));
		}
		output.state = state;

		return output;
	}

	bool printDimacs() {

		for (int i = 0; i < rts.size(); ++i) {
			Route& r = rts[i];

			printf("Route #%d:",i+1);
			int pt = customers[r.head].next;
			while (pt <= input.custCnt) {
				printf(" %d", pt);
				pt = customers[pt].next;
			}
			printf("\n");
		}
		// TODO[lyh][0]:这里的倍数需要处理一下
		printf("Cost %.1lf\n",double(RoutesCost/10));
		fflush(stdout);
		return true;
	}

	bool saveOutAsSintefFile(std::string opt = "") {
			
		std::string s = "";
			
		MyString ms;

		s += "Instance name : " + input.example + "\n";
		s += "Authors : <the name of the authors>\n";
		s += "Date : <dd - mm - yy>\n";
		s += "Reference : <reference to publication of method>\n";
		s += "Solution\n";
		for (int i = 0; i < rts.cnt; i++) {
			auto cus = rPutCusInve(rts[i]);
			std::string r = "Route " + ms.int_str(i+1) +" :";

			for (auto c : cus) {
				r += " " + ms.int_str(c);
			}
			r += "\n";
			s += r;
		}

		reCalRtsCostSumCost();

		std::ofstream rgbData;
		std::string wrPath = env.outputPath + opt 
				+ input.example + "L" + ms.int_str(RoutesCost) + ".txt";

		rgbData.open(wrPath, std::ios::app | std::ios::out);

		if (!rgbData) {
			Log(Log::Level::Warning) << "sintefoutput file open errno" << std::endl;
			return false;
		}

		rgbData << s;

		//debug(s);
		rgbData.close();

		return true;
	}
		
	~Solver() {};

public:
	Timer lyhTimer;
};

};

#endif // !vrptwNew_SOLVER_H
