#ifndef vrptwNew_SOLVER_H
#define vrptwNew_SOLVER_H

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <math.h>  
#include <stack>
#include <algorithm>
#include <queue>
#include <sstream>
#include <cstdlib>
#include <random>
#include <chrono>
#include <limits.h>
#include <map>
#include <stdio.h>
#include <ctime>
#include <thread>
#include <future>
#include <iomanip>
#include <bitset>
#include <numeric>

#include "./Environment.h"
#include "./Utility.h"
#include "./Problem.h"
#include "./Flag.h"

namespace vrptwNew {

#define reCusNo(x) (((x)<=(input.custCnt+1))?(x):(input.custCnt+1))

	struct Route {

	public:

		int routeID = -1;
		DisType rCustCnt = 0; //没有计算仓库
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

		bool operator = (const Route& r) {

			this->routeID = r.routeID;
			this->rCustCnt = r.rCustCnt;
			this->rQ = r.rQ;
			this->head = r.head;
			this->tail = r.tail;

			this->rPc = r.rPc;
			this->rPtw = r.rPtw;
			this->rWeight = r.rWeight;
			this->routeCost = r.routeCost;
			return true;
		}

	};

	struct RTS {

		vector<Route> ve;
		vector<int> posOf;
		int cnt = 0;

		int size() {
			return cnt;
		}

		RTS() {
			cnt = 0;
			for (int i = 0; i < 200; ++i) {
				Route r;
				ve.push_back(r);
			}
			posOf = vector<int>(200, -1);
		}

		bool operator = (const RTS& r) {
			cnt = r.cnt;
			ve = r.ve;
			posOf = r.posOf;
			return true;
		}

		~RTS() {
			//debug("~RTS")
			ve.clear();
			posOf.clear();
		}

		Route& operator [](int index) {

#if CHECKING
			if (index >= ve.size() || index < 0) {
				vector<Customer> cus;

				debug("Route & operator [] error!")
					Route r1;
				return r1;
			}
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
			if (rid < 0 || posOf[rid] < 0) {
				return false;
			}
#endif // CHECKING

			removeIndex(posOf[rid]);
			return true;
		}

		Route& getRouteByRid(int rid) {

#if CHECKING
			if (rid == -1 || rid >= posOf.size()) {
				vector<Customer>cus;
				Route r1;
				return r1;
			}
#endif // CHECKING

			int index = posOf[rid];

#if CHECKING
			if (index < 0 || index >= cnt) {
				vector<Customer>cus;
				Route r1;
				deOut(rid)deOut(index)debug(" rts array out of bound!")
					return r1;
			}
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

		vector<int> ve;
		vector<int> pos;
		int cnt = 0;

		ConfSet(int maxSize) {
			cnt = 0;
			ve = vector<int>(maxSize + 1, -1);
			pos = vector<int>(maxSize + 1, -1);
		}

		ConfSet() {
			cnt = 0;
			ve = vector<int>(200, -1);
			pos = vector<int>(200, -1);
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
				/*debug(" conf already in")
				debug(" conf already in")*/
				return false;
			}

			ve[cnt] = val;
			pos[val] = cnt;
			++cnt;
			return true;
		}

		int posAt(int val) {

			if (val < 0 && val >= pos.size()) {
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

		void disp() {
			for (int i = 0; i < cnt; ++i) {
				cout << ve[i] << " ";
			}
			cout << endl;
		}

	};

	struct ShuffleCards {

		vector<int> ve;

		Random myRand;

		vector<int> disorder(int range) {
			ve = vector<int>(range, 0);
			for (int i = 0; i < range; ++i) {
				ve[i] = i;
			}
			for (int i = range - 1; i > 1; i--) {
				int index = myRand.pick(i);
				swap(ve[i], ve[index]);
			}
			return ve;
		}

		bool makeItDisorder(vector<int>& it) {

			if (it.size() <= 1) {
				return false;
			}

			for (int i = it.size() - 1; i > 1; i--) {
				int index = myRand.pick(i);
				swap(it[i], it[index]);
			}
			return true;
		}

		vector<string> makeStringVeDisorder(vector<string>& it) {

			if (it.size() <= 1) {
				return it;
			}

			for (int i = it.size() - 1; i > 1; i--) {
				int index = myRand.pick(i);
				swap(it[i], it[index]);
			}
			return it;
		}

		ShuffleCards(int seed) :myRand(seed) {}
		ShuffleCards() :myRand() {}

	};

	struct RandomX {

	public:

		using Generator = std::mt19937;

		RandomX(LL seed) : rgen(seed) { initMpLLArr(); }
		RandomX() : rgen(generateSeed()) { initMpLLArr(); }

		vector< vector<int> > mpLLArr;
		LL maxRange = 10001;

		bool initMpLLArr() {
			mpLLArr = vector< vector<int> >(maxRange);

			for (int n = 1; n < mpLLArr.size(); ++n) {
				mpLLArr[n] = vector<int>(n, 0);
				for (int i = 0; i < n; ++i) {
					mpLLArr[n][i] = i;
				}
			}
			return true;
		}

		static LL generateSeed() {
			unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

			return static_cast<LL>(seed);
		}

		Generator::result_type operator()() { return rgen(); }

		// pick with probability of (numerator / denominator).
		bool isPicked(unsigned numerator, unsigned denominator) {
			return ((rgen() % denominator) < numerator);
		}

		// pick from [min, max).
		LL pick(LL min, LL max) {
			return ((rgen() % (max - min)) + min);
		}
		// pick from [0, max).
		LL pick(LL max) {
			return (rgen() % max);
		}

		bool getMN(LL M, int N) {

			vector<int>& ve = mpLLArr[M];
			for (int i = 0; i < N; ++i) {
				int index = pick(i, M);
				swap(ve[i], ve[index]);
			}
			return true;
		}

		Generator rgen;
	};

	struct NextPermutation {

		bool hasNext(vector<int>& ve, int Kmax, int& k, int N) {

			if (k == 1 && ve[k] == N) {
				return false;
			}
			else {
				return true;
			}
			return false;
		}

		bool nextPer(vector<int>& ve, int Kmax, int& k, int N) {

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

	struct Combination
	{
	public:

		Combination() {};
		~Combination() {};

		vector<vector<int>>ret;
		vector<vector<int>> get(int NN, int MM) {

			int Kmax = MM;
			int k = Kmax;
			int N = NN;

			vector<int> ve(Kmax + 1, 0);

			for (int i = 1; i <= Kmax; ++i) {
				ve[i] = i - 1;
			}

			do {

				ret.push_back(vector<int>(ve.begin() + 1, ve.begin() + Kmax + 1));
				/*for (int i = 1; i <= Kmax; ++i) {
					cout << ve[i] << " ";
				}
				cout << endl;*/

				++ve[k];
				if (ve[k] >= N) {
					while (k >= 1 && ve[k] >= N - (Kmax - k)) {
						k--;
						++ve[k];
					}
					++k;
					while (k <= Kmax) {
						ve[k] = ve[k - 1] + 1;
						++k;
					}
					k = Kmax;
				}

			} while (ve[0] == 0);

			return ret;
		}
	private:

	};

	class Solver
	{
	public:

		Input& input;
		Output output;
		Environment& env;
		Configuration& cfg;

		vector<Customer> customers;
		vector<vector<LL>> yearTable;

		int ourTarget = 0;
		DisType penalty = 0;
		DisType Ptw = 0;
		DisType PtwNoWei = 0;
		DisType Pc = 0;

		DisType alpha = 1;
		DisType beta = 1;
		DisType gamma = 0;

		DisType RoutesCost = 0;
		LL squIter = 1;

		RTS rts;

		ConfSet PtwConfRts, PcConfRts;

		vector<LL> P;

		LL EPIter = 1;
		int minEPcus = IntInf;

		Route EPr;

		struct DeltPen {

			DisType deltPtw;
			DisType deltPc;
			DisType PcOnly;
			DisType PtwOnly;
			DisType deltCost;

			DeltPen() {

				deltPtw = DisInf;
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

			int v;
			int w;
			vector<int> ve;
			int kind;

			DeltPen deltPen;

			TwoNodeMove(int v, int w, int kind, DeltPen d) :
				v(v), w(w), kind(kind), deltPen(d) {
			}

			TwoNodeMove(vector<int> ve, int kind, DeltPen d) :
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
			int rIndex;
			int pos;
			DisType pen;
			DisType cost;
			LL year;
			Position() :rIndex(-1), pos(-1), pen(DisInf), cost(DisInf) ,year(DisInf) {}
			Position(int rIndex,int pos,DisType pen,DisType cost,LL year) :
				rIndex(rIndex), pos(pos), pen(pen), cost(cost), year(year) {}
		};

		struct eOneRNode {

			vector<int> ejeVe;
			int Psum = 0;
			int rId = -1;

			int getMaxEle() {
				int ret = -1;
				for (int i : ejeVe) {
					ret = max(ret,i);
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

		struct bestSol
		{
			vector<Customer> customers;
			RTS rts;
			DisType Pc = 0;
			DisType Ptw = 0;
			DisType PtwNoWei = 0;
			DisType RouteCost = 0;
			DisType penalty = 0;
			DisType RoutesCost = 0;
			Route EPr;

			bestSol(vector<Customer> customers,
				RTS rts,
				Route EPr,
				DisType Pc,
				DisType Ptw,
				DisType PtwNoWei,
				DisType penalty
			) :
				customers(customers),
				rts(rts),
				Pc(Pc),
				Ptw(Ptw),
				PtwNoWei(PtwNoWei),
				penalty(penalty)
				, EPr(EPr) {
			}

			bestSol(vector<Customer> customers,
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

			bool operator = (const bestSol& s) {
				
				this->customers = s.customers;
				this->rts = s.rts;
				this->Pc = s.Pc;
				this->Ptw = s.Ptw;
				this->PtwNoWei = s.PtwNoWei;
				this->penalty = s.penalty;
				this->RoutesCost = s.RoutesCost;
				this->EPr = s.EPr;

				return true;
			}
			bestSol() {
				Pc = 0;
				PtwNoWei = 0;
			}

			~bestSol() {
				customers.clear();
			}
		};

		DisType pBestThisTurn = DisInf;

		bool initEPr() {

			Route& r = EPr;

			r.head = input.custCnt + 1;
			r.tail = input.custCnt + 2;

			customers.push_back(Customer());
			customers.push_back(Customer());

			customers[r.head].next = r.tail;
			customers[r.tail].pre = r.head;

			customers[r.head].routeID = -1;
			customers[r.tail].routeID = -1;

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

			return true;
		}

		Solver(Input& input, Configuration& cfg, Environment& env) :
			input(input), env(env), cfg(cfg),
			lyhTimer(cfg.runTimer), myRand(env.seed), myRandX(env.seed)
		{

			P = vector<LL>(input.custCnt + 1, 1);
			customers = vector<Customer>(input.custCnt + 1);
			yearTable = vector<vector<LL>>(input.custCnt + 1, vector<LL>(input.custCnt + 1, 0));
			//EPYearTable = vector<int> (input.custCnt + 1,  1);
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
		}

		Solver(const Solver& s) :
			input(s.input), env(s.env), cfg(s.cfg),
			myRand(s.myRand), lyhTimer(s.lyhTimer), myRandX(s.myRandX) {

			this->output = s.output;
			this->P = s.P;
			this->customers = s.customers;
			this->yearTable = s.yearTable;
			//this->EPYearTable = s.EPYearTable;
			this->rts = s.rts;
			this->PtwConfRts = s.PtwConfRts;
			this->PcConfRts = s.PcConfRts;
			this->penalty = s.penalty;
			this->Ptw = s.Ptw;
			this->PtwNoWei = s.PtwNoWei;
			this->Pc = s.Pc;
			this->alpha = s.alpha;
			this->beta = s.beta;
			this->gamma = s.gamma;
			this->RoutesCost = s.RoutesCost;
			this->squIter = s.squIter;
			this->EPr = s.EPr;
			this->EPIter = s.EPIter;
			this->ourTarget = s.ourTarget;
			this->cfg = s.cfg;

			//this->minEPSize = s.minEPSize;

		}

		void operator = (const Solver& s) {

			this->P = s.P;
			this->customers = s.customers;
			this->yearTable = s.yearTable;

			this->rts = s.rts;
			this->PtwConfRts = s.PtwConfRts;
			this->PcConfRts = s.PcConfRts;
			this->penalty = s.penalty;
			this->Ptw = s.Ptw;
			this->PtwNoWei = s.PtwNoWei;
			this->Pc = s.Pc;
			this->alpha = s.alpha;
			this->beta = s.beta;
			this->gamma = s.gamma;
			this->RoutesCost = s.RoutesCost;
			this->squIter = s.squIter;
			this->EPr = s.EPr;
			this->EPIter = s.EPIter;
			this->ourTarget = s.ourTarget;
			this->myRandX = s.myRandX;
		}

		// route function
		Route rCreateRoute(int id) {

			Route r(id);
			int index = (rts.cnt + 1) * 2 + input.custCnt + 1;

			r.head = index;
			r.tail = index + 1;

			customers.push_back(Customer());
			customers.push_back(Customer());

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
			r.rPc = max((DisType)0, r.rQ - input.Q);
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

		bool rRemoveAllCusInR(Route& r) {

			r.routeCost = 0;
			r.rWeight = 1;

			vector<int> ve = rPutCusInve(r);
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
			r.rPc = max((DisType)0, r.rQ - input.Q);

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
			r.rPc = max((DisType)0, r.rQ - input.Q);

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
			r.rPc = max((DisType)0, r.rQ - input.Q);

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
			cout << "predisp: ";
			while (pt != -1) {
				cout << pt << " ";
				cout << ",";
				pt = customers[pt].pre;
			}
			cout << endl;

		}

		void rNextDisp(Route& r) {

			int pt = r.head;

			cout << "nextdisp: ";
			while (pt != -1) {
				//out( mem[pt].val );
				cout << pt << " ";
				cout << ",";
				pt = customers[pt].next;
			}
			cout << endl;
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
				customers[pt].TW_X += max(customers[pt].avp - input.datas[pt].DUEDATE, (DisType)0);

				if (customers[pt].avp <= input.datas[pt].DUEDATE) {
					customers[pt].av = max(customers[pt].avp, input.datas[pt].READYTIME);
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
			r.rPc = max((DisType)0, r.rQ - input.Q);
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
				customers[pt].TW_X += max(customers[pt].avp - input.datas[pt].DUEDATE, (DisType)0);

				if (customers[pt].avp <= input.datas[pt].DUEDATE) {
					customers[pt].av = max(customers[pt].avp, input.datas[pt].READYTIME);
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
			r.rPc = max((DisType)0, r.rQ - input.Q);

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

				customers[pt].TWX_ += max(input.datas[pt].READYTIME - customers[pt].zvp, (DisType)0);

				customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
					min(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;

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
			r.rPc = max((DisType)0, r.rQ - input.Q);
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

				customers[pt].TWX_ += max(input.datas[pt].READYTIME - customers[pt].zvp, (DisType)0);

				customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
					min(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;
				
				customers[pt].QX_ = customers[ptnext].QX_ + input.datas[pt].DEMAND;
				ptnext = pt;
				pt = customers[pt].pre;
			}
			r.rPtw = customers[r.head].TWX_;
			r.rQ = customers[r.head].QX_;
			r.rPc = max((DisType)0, r.rQ - input.Q);
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

		DisType rUpdateRCost(Route& r) {

			int pt = r.head;
			int ptnext = customers[pt].next;
			r.routeCost = 0;
			while (pt != -1 && ptnext != -1) {
				r.routeCost += input.disOf[reCusNo(pt)][reCusNo(ptnext)];
				pt = ptnext;
				ptnext = customers[ptnext].next;
			}
			return r.routeCost;
		}

		vector<int> rPutCusInve(Route& r) {

			vector<int> ret;
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

		bool rCheck(Route& r) {

			vector<int> cus1;
			vector<int> cus2;

			int pt = r.head;
			while (pt != -1) {
				cus1.push_back(pt);
				pt = customers[pt].next;
			}
			
			pt = r.tail;
			while (pt != -1) {
				cus2.push_back(pt);
				pt = customers[pt].pre;
			}

			if (cus1.size() != cus2.size()) {
				debug(cus1.size() != cus2.size())
			}

			for (int i = 0; i < cus1.size(); ++i) {
			
				if (cus1[i] != cus2[cus1.size()-1-i]) {
					debug(cus1[i] != cus2[cus1.size() - 1 - i])
				}
			}

			if (r.rCustCnt != cus1.size()-2) {
				debug(r.rCustCnt)
					debug(cus1.size())
					rNextDisp(r);
					rNextDisp(r);

			}
			return true;
		}

		bool rtsCheck() {
			for (int i = 0; i < rts.cnt; ++i) {
				rCheck(rts[i]);
			}
			return true;
		}

		bool rUpdateRtsValsAndPen(Route& r) {

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

		bool updateRtsValsAndPen() {
			for (int i = 0; i < rts.cnt; ++i) {
				Route& r = rts[i];
				rUpdateRtsValsAndPen(r);
				rUpdateAvQfrom(r,r.head);
				rUpdateZvQfrom(r,r.tail);
				rUpdateRCost(r);
			}
			updatePen();
			return true;
		}

		int getCusCnt() {
			int ret = 0;
			for (int i = 0; i < rts.cnt; ++i) {
				ret += rGetCusCnt(rts[i]);
			}
			return ret;
		}

		bool RTSDisPlay() {
			for (int i = 0; i < rts.cnt; ++i) {
				rNextDisp(rts[i]);
			}
			return true;
		}

		DisType updatePen() {

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

			return penalty;
		}
		
		DisType updatePen(DeltPen delt) {

			Pc += delt.PcOnly;
			Ptw += delt.deltPtw;
			PtwNoWei += delt.PtwOnly;
			penalty = alpha * Ptw + beta * Pc;

			return penalty;
		}

		DisType updateRtsCost() {

			RoutesCost = 0;
			for (int i = 0; i < rts.size(); ++i) {
				RoutesCost += rUpdateRCost(rts[i]);
			}
			return RoutesCost;
		}
		
		DisType updateRtsCost(DeltPen delt) {

			RoutesCost += delt.deltCost;
			return RoutesCost;
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

			priority_queue<Position,Vec<Position>,decltype(cmp)> qu(cmp);
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
				DisType rPc = max((DisType)0, rt.rQ + input.datas[w].DEMAND - input.Q);
				rPc = rPc - oldrPc;

				while (v != -1 && vj != -1) {

					DisType oldrPtw = rts[i].rPtw;
					
					DisType rPtw = customers[v].TW_X;
					rPtw += customers[vj].TWX_;

					DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
					rPtw += max((DisType)0, awp - input.datas[w].DUEDATE);
					DisType aw =
						awp <= input.datas[w].DUEDATE ? max(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

					DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
					rPtw += max((DisType)0, avjp - input.datas[vj].DUEDATE);
					DisType avj =
						avjp <= input.datas[vj].DUEDATE ? max(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
					rPtw += max((DisType)0, avj - customers[vj].zv);

					rPtw = rPtw - oldrPtw;
					
					DisType cost = input.disOf[reCusNo(w)][reCusNo(v)] + input.disOf[reCusNo(w)][reCusNo(vj)];
					
					int vre = v > input.custCnt ? 0 : v;
					int vjre = vj > input.custCnt ? 0 : vj;

					LL year = yearTable[vre][w] + yearTable[w][vjre];
					
					Position pos(i,v, rPtw + rPc, cost,year);
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
					if (myRand.pick(p0cnt)==0) {
						posPool[0] = pos;
					}
				}
				
				if (pos.year < posPool[1].year) {
					p1cnt = 1;
					posPool[1] = pos;
				}
				else if (pos.year == posPool[1].year) {
					++p1cnt;
					if (myRand.pick(p1cnt)==0) {
						posPool[1] = pos;
					}
				}
				
				if (pos.cost < posPool[2].cost) {
					p2cnt = 1;
					posPool[2] = pos;
				}
				else if (pos.cost == posPool[2].cost) {
					++p2cnt;
					if (myRand.pick(p2cnt)==0) {
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
						if (myRand.pick(p3cnt) == 0) {
							posPool[3] = pos;
						}
					}
				}
			}
			
			
			if (posPool[0].pen > 0) {
				int index = myRand.pick(posPool.size());
				//debug(index);
				return posPool[index];
			}
			else {
				int cnt = 0;
				Position retP;
				for (int i = 0; i < posPool.size(); ++i) {
					if (posPool[i].pen == 0) {
						++cnt;
						if (myRand.pick(cnt) == 0) {
							retP = posPool[i];
						}
					}
				}
				
				return retP;
			}

			return Position();
			
		}

		Position findBestPosInSolForInit(int w) {

			Position bestPos;

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
					Ptw += max((DisType)0, awp - input.datas[w].DUEDATE);
					DisType aw =
						awp <= input.datas[w].DUEDATE ? max(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

					DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
					Ptw += max((DisType)0, avjp - input.datas[vj].DUEDATE);
					DisType avj =
						avjp <= input.datas[vj].DUEDATE ? max(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
					Ptw += max((DisType)0, avj - customers[vj].zv);

					Pc = max((DisType)0, rt.rQ + input.datas[w].DEMAND - input.Q);

					Ptw = Ptw - oldPtw;
					Pc = Pc - oldPc;

					DisType cost = input.disOf[reCusNo(w)][reCusNo(v)] + input.disOf[reCusNo(w)][reCusNo(vj)];
					if (bestPos.pen > Ptw + Pc) {
						bestPos.cost = cost;
						bestPos.pen = Ptw + Pc;
						bestPos.pos = v;
						bestPos.rIndex = i;
					}
					else if (bestPos.pen == Ptw + Pc) {
						if (cost < bestPos.cost) {
							bestPos.pen = Ptw + Pc;
							bestPos.pos = v;
							bestPos.rIndex = i;
							bestPos.cost = cost;
						}
					}
					v = vj;
					vj = customers[vj].next;
				}
			}

			return bestPos;
		}

		bool init() {

			auto cmp = [&](int x, int y) {
				return input.datas[x].DUEDATE > input.datas[y].DUEDATE;
			};

			vector<int>que1;

			for (int i = 1; i <= input.custCnt; ++i) {
				que1.push_back(i);
			}

			sort(que1.begin(), que1.end(), cmp);

			int rid = 0;

			for (int i = 0; i < customers.size(); ++i) {
				customers[i].id = i;
			}

			do {

				int tp = que1.back();
				que1.pop_back();

				bool isSucceed = false;
				for (int i = 0; i < rts.size(); ++i) {

					rInsAtPosPre(rts[i], rts[i].tail, tp);
					if (rUpdateAvfrom(rts[i], tp) == 0 && rts[i].rQ <= input.Q) {
						isSucceed = true;
						break;
					}
					else {
						rRemoveAtPos(rts[i], tp);
						rUpdateAvfrom(rts[i], rts[i].head);
					}
				}

				if (isSucceed) {
					continue;
				}

				Route r1 = rCreateRoute(rid++);
				rInsAtPos(r1, r1.head, tp);
				rUpdateAvfrom(r1, r1.head);
				rts.push_back(r1);
				//r1.nextDisp();

			} while (!que1.empty());

			Log(Log::Info) << "init: ";
			Log(Log::Info) << "rts.size(): " << rts.size() << endl;


			for (int i = 0; i < rts.size(); ++i) {

				rUpdateZvQfrom(rts[i], rts[i].tail);
				rUpdateAQfrom(rts[i], rts[i].head);
				rUpdateRCost(rts[i]);
			}

			updatePen();

			patternAdjustment();

			return true;
		}

		bool initBybestSolution() {

			MyString ms;
			string csCnt = ms.LL_str(input.custCnt);
			string path = env.sinBKSPath + "results" + csCnt + "/" + input.example + ".txt";
			ifstream fin(path);
			string str;

			int rid = 0;

			for (int i = 0; i < customers.size(); ++i) {
				customers[i].id = i;
			}

			while (getline(fin, str)) {

				if (str.find("Route") != string::npos) {
					string cus = ms.split(str, ":")[1];
					//debug(cus)
					Route r = rCreateRoute(rid++);
					vector<string> custs = ms.split(cus, " ");
					for (int i = 0; i < custs.size(); ++i) {
						int cusno = ms.str_LL(custs[i]);
						rInsAtPosPre(r, r.tail, cusno);
					}

					rUpdateAvQfrom(r, r.head);
					rUpdateZvQfrom(r, r.tail);

					//rUpdatePc(r);
					//rUpdateRouteCost(r);
					rts.push_back(r);
				}
			}

			//patternAdjustment();
			updatePen();
			updateRtsCost();
			//rts.disp();
			fin.clear();
			fin.close();

			Log(Log::Info) << "rts.size(): " << rts.size() << endl;

			return true;
		}

		bool initMaxRoute() {

			auto cmp = [&](int x, int y) {
				return input.datas[x].DUEDATE < input.datas[y].DUEDATE;
			};

			vector<int>que1;

			for (int i = 1; i <= input.custCnt; ++i) {
				que1.push_back(i);
			}

			ShuffleCards sc;

			sort(que1.begin(), que1.end(), cmp);

			int rid = 0;

			for (int i = 0; i < customers.size(); ++i) {
				customers[i].id = i;
			}

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
				
				rUpdateRCost(r);
			}
			updatePen();
			//patternAdjustment();
			return true;
		}

		bool InitBy2000betterSol(int cusNo = -1) {

			MyString ms;
			string csCnt = ms.LL_str(input.custCnt);

			string caseNoStr;
			string path = "";
			if (cusNo == -1) {
				caseNoStr = ms.LL_str(myRand.pick(2000));
			}
			else {
				caseNoStr = ms.LL_str(cusNo);
			}

			//_root_vrptw_data_200_R1_2_1.TXT_11.sol
			path = env.linBKSPath+"./betterInit/_root_vrptw_data_" + csCnt + "_" + input.example + ".TXT_" + caseNoStr + ".sol";

			ifstream f(path.c_str());
			Log(Log::Info) << "path: " << path << endl;
			Log(Log::Info) << "f.good(): " << f.good() << endl;

			if (!f.good()) {
				caseNoStr = ms.LL_str(myRand.pick(200));
				path = "./betterInit/_root_vrptw_data_" + csCnt + "_" + input.example + ".TXT_" + caseNoStr + ".sol";
				ifstream ff(path.c_str());

				Log(Log::Info) << "ff.good(): " << ff.good() << endl;
				if (!ff.good()) {
					return false;
				}
			}

			Log(Log::Warning) << "init bettersol suc use case " << caseNoStr << endl;

			ifstream fin(path);
			string str;
			int line = 1;
			int rid = 0;

			for (int i = 0; i < customers.size(); ++i) {
				customers[i].id = i;
			}

			while (getline(fin, str)) {
				++line;
				//debug(str)
				vector<string> arr = ms.split(str, " ");

				Route r = rCreateRoute(rid++);
				for (string cus : arr) {
					//cout << s << " ";
					rInsAtPosPre(r, r.tail, ms.str_LL(cus));
				}

				rUpdateAvQfrom(r, r.head);
				rUpdateZvQfrom(r, r.tail);

				rts.push_back(r);

			}

			//patternAdjustment();

			updatePen();
			//rts.disp();
			fin.clear();
			fin.close();
			//debug(rts.size());
			return true;

		}

		bool initDiffSituation() {

			if (cfg.breakRecord == 1) {
				//init();
				set<string> BetterSol = {
				"RC2_2_1",
				"RC2_4_1",
				"RC2_8_2",
				"RC2_10_1",
				"R1_2_1",
				"R1_4_1",
				"R1_6_1",
				"R1_8_1",
				"R1_10_1",
				"C2_8_7",
				"C1_2_8",
				"C1_4_7",
				"C2_10_9",
				};
				if (BetterSol.count(input.example) > 0) {
					if (!InitBy2000betterSol()) {
						initBybestSolution();
					}
				}
				else {
					initBybestSolution();
				}
			}
			else {
				//init();
				initMaxRoute();
			}

			if (cfg.breakRecord) {
				ourTarget = input.sintefRecRN - 1;
			}
			else {
				ourTarget = input.sintefRecRN;
			}

			int Qbound = input.sumQToQ;

			Log(Log::Level::Warning) << "penalty: " << penalty << endl;
			Log(Log::Level::Warning) << "Qbound: " << Qbound << endl;
			Log(Log::Level::Warning) << "ourTarget: " << ourTarget << endl;
			Log(Log::Level::Warning) << "rts.size(): " << rts.size() << endl;

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

		vector<int> EPve() {

			vector<int>ret = rPutCusInve(EPr);
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
				debug("estimate no this kind move")
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

			auto getDeltPtw = [&]() {

				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;

				// w->v
				DisType newwvPtw = 0;

				newwvPtw += customers[w].TW_X;
				newwvPtw += customers[v].TWX_;

				DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
				newwvPtw += max(avp - customers[v].zv, 0);

				// (v-) -> (w+)
				DisType newv_wjPtw = 0;
				newv_wjPtw += customers[v_].TW_X;
				newv_wjPtw += customers[wj].TWX_;
				DisType awjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(wj)];
				newv_wjPtw += max(awjp - customers[wj].zv, 0);

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

				bestM.deltPc = max((DisType)0, rwQ - input.Q) +
					max((DisType)0, rvQ - input.Q) -
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

			auto getDeltPtw = [&]() {

				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;

				// v->w
				DisType newwvPtw = 0;

				newwvPtw += customers[v].TW_X;
				newwvPtw += customers[w].TWX_;

				DisType awp = customers[v].av
					+ input.datas[v].SERVICETIME
					+ input.disOf[reCusNo(v)][reCusNo(w)];

				newwvPtw += max(awp - customers[w].zv, 0);

				// (w-) -> (v+)
				DisType neww_vjPtw = 0;

				neww_vjPtw += customers[w_].TW_X;
				neww_vjPtw += customers[vj].TWX_;
				DisType avjp = customers[w_].av
					+ input.datas[w_].SERVICETIME
					+ input.disOf[reCusNo(w_)][reCusNo(vj)];
				neww_vjPtw += max(avjp - customers[vj].zv, 0);

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

				bestM.deltPc = max((DisType)0, rvwQ - input.Q)
					+ max((DisType)0, rw_vjQ - input.Q)
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

			auto getDeltPtw = [&]() {

				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;

				DisType newv_vjPtw = 0;
				DisType newvwPtw = 0;

				if (rw.routeID == rv.routeID) {

					if (v == w || v == w_) {
						return bestM;
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

						newvwPtw += max((DisType)0, avjp - input.datas[vj].DUEDATE);
						newvwPtw += customers[v_].TW_X;

						DisType avj = avjp > input.datas[vj].DUEDATE ? input.datas[vj].DUEDATE :
							max(avjp, input.datas[vj].READYTIME);

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
								newvwPtw += max((DisType)0, aptp - input.datas[pt].DUEDATE);

								DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
									max(aptp, input.datas[pt].READYTIME);

								lastv = pt;
								lastav = apt;
								pt = customers[pt].next;
							}
						}

						DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(v)];
						newvwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

						DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						DisType awp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
						newvwPtw += max((DisType)0, awp - customers[w].zv);

						newvwPtw += customers[w].TWX_;

					}
					else if (front == w) {

						DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

						newvwPtw += customers[w_].TW_X;
						newvwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

						DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						lastav = av;
						lastv = v;

						DisType awp = lastav + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];

						newvwPtw += max((DisType)0, awp - input.datas[w].DUEDATE);

						DisType aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
							max(awp, input.datas[w].READYTIME);

						lastv = w;
						lastav = aw;

						int pt = customers[w].next;

						while (pt != -1) {

							DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
							newvwPtw += max((DisType)0, aptp - input.datas[pt].DUEDATE);

							DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
								max(aptp, input.datas[pt].READYTIME);

							lastv = pt;
							lastav = apt;

							pt = customers[pt].next;
							if (pt == v) {
								break;
							}
						}

						DisType avjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(vj)];
						newvwPtw += max((DisType)0, avjp - customers[vj].zv);
						newvwPtw += customers[vj].TWX_;
					}
					else {

						rNextDisp(rv);
						debug(front)
							debug(v)
							debug("error 333")
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
					newvwPtw += max((DisType)0, max(avp, input.datas[v].READYTIME) - min(input.datas[v].DUEDATE, zvp));

					newv_vjPtw += customers[v_].TW_X;
					newv_vjPtw += customers[vj].TWX_;

					DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];
					newv_vjPtw += max((DisType)0, avjp - customers[vj].zv);

					bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
					bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
					bestM.deltPtw *= alpha;
				}
			};
			
			auto getDeltPc = [&]() {

				DisType vPc = rv.rPc;
				DisType wPc = rw.rPc;

				if (rv.routeID == rw.routeID) {
					bestM.deltPc = 0;
					bestM.PcOnly = 0;
				}
				else {
					bestM.deltPc = max((DisType)0, rw.rQ + input.datas[v].DEMAND - input.Q)
						+ max((DisType)0, rv.rQ - input.datas[v].DEMAND - input.Q)
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

			auto getDeltPtw = [&]() {

				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;
				DisType newv_vjPtw = 0;
				DisType newvwPtw = 0;

				if (rw.routeID == rv.routeID) {

					if (v == w || v == wj) {
						return bestM;
					}


					int w_ = customers[w].pre;
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

						newvwPtw += max((DisType)0, avjp - input.datas[vj].DUEDATE);
						newvwPtw += customers[v_].TW_X;

						DisType avj = avjp > input.datas[vj].DUEDATE ? input.datas[vj].DUEDATE :
							max(avjp, input.datas[vj].READYTIME);

						lastav = avj;
						lastv = vj;

						if (vj == w) {
							;
						}
						else {

							int pt = customers[vj].next;
							while (pt != -1) {

								DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
								newvwPtw += max((DisType)0, aptp - input.datas[pt].DUEDATE);

								DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
									max(aptp, input.datas[pt].READYTIME);

								lastv = pt;
								lastav = apt;

								if (pt == w) {
									break;
								}

								pt = customers[pt].next;
							}
						}

						DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(v)];
						newvwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

						DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
						newvwPtw += max((DisType)0, awjp - customers[wj].zv);
						newvwPtw += customers[wj].TWX_;

					}
					else if (front == w) {

						DisType avp = customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];

						newvwPtw += customers[w].TW_X;
						newvwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

						DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
						newvwPtw += max((DisType)0, awjp - input.datas[wj].DUEDATE);

						DisType awj = awjp > input.datas[wj].DUEDATE ? input.datas[wj].DUEDATE :
							max(awjp, input.datas[wj].READYTIME);

						lastav = awj;
						lastv = wj;

						int pt = customers[lastv].next;
						while (pt != -1) {

							DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
							newvwPtw += max((DisType)0, aptp - input.datas[pt].DUEDATE);

							DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
								max(aptp, input.datas[pt].READYTIME);

							lastav = apt;
							lastv = pt;
							pt = customers[pt].next;

							if (pt == v) {
								break;
							}
						}

						DisType avjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(vj)];
						newvwPtw += max((DisType)0, avjp - customers[vj].zv);
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
					newvwPtw += max((DisType)0, max(avp, input.datas[v].READYTIME) - min(input.datas[v].DUEDATE, zvp));

					// insert v to (w,w-)
					newv_vjPtw += customers[v_].TW_X;
					newv_vjPtw += customers[vj].TWX_;

					DisType avjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vj)];
					newv_vjPtw += max((DisType)0, avjp - customers[vj].zv);

					bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
					bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
					bestM.deltPtw *= alpha;
				}
			};
			// insert v to w and (w+)
			
			auto getDeltPc = [&]() {

				DisType vPc = rv.rPc;
				DisType wPc = rw.rPc;

				if (rv.routeID == rw.routeID) {
					bestM.deltPc = 0;
					bestM.PcOnly = 0;
				}
				else {
					bestM.deltPc = max((DisType)0, rw.rQ + input.datas[v].DEMAND - input.Q)
						+ max((DisType)0, rv.rQ - input.datas[v].DEMAND - input.Q)
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

			auto getDeltPtw = [&]() {

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
					newwvPtw += max((DisType)0, max(awp, input.datas[w].READYTIME) - min(input.datas[w].DUEDATE, zwp));

					// insert w to (v,v-)
					neww_wjPtw += customers[w_].TW_X;
					neww_wjPtw += customers[wj].TWX_;
					DisType awjp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(wj)];
					neww_wjPtw += max((DisType)0, awjp - customers[wj].zv);

					bestM.PtwOnly = newwvPtw + neww_wjPtw - rv.rPtw - rw.rPtw;
					bestM.deltPtw = newwvPtw * rv.rWeight + neww_wjPtw * rw.rWeight - vPtw - wPtw;
					bestM.deltPtw *= alpha;

				}

			};

			auto getDeltPc = [&]() {

				DisType wPc = rw.rPc;
				DisType vPc = rv.rPc;

				if (rv.routeID == rw.routeID) {
					bestM.deltPc = 0;
					bestM.PcOnly = 0;
				}
				else {
					bestM.deltPc = max((DisType)0, rw.rQ - input.datas[w].DEMAND - input.Q)
						+ max((DisType)0, rv.rQ + input.datas[w].DEMAND - input.Q)
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
			auto getDeltPtw = [&]() {

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
					newwvPtw += max((DisType)0, max(awp, input.datas[w].READYTIME) - min(input.datas[w].DUEDATE, zwp));

					neww_wjPtw += customers[w_].TW_X;
					neww_wjPtw += customers[wj].TWX_;
					DisType awjp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(wj)];
					neww_wjPtw += max((DisType)0, awjp - customers[wj].zv);

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
					bestM.deltPc = max((DisType)0, rw.rQ - input.datas[w].DEMAND - input.Q)
						+ max((DisType)0, rv.rQ + input.datas[w].DEMAND - input.Q)
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
			auto getDeltPtw = [&]() {

				DisType newwPtw = 0;
				DisType newvPtw = 0;
				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;

				if (rv.routeID == rw.routeID) {

					if (v == w) {
						return bestM;
					}

					if (v_ == w) {

						// (v--)->(v-)->(v)->(v+)
						int v__ = customers[v_].pre;
						DisType av = 0;
						newvPtw = 0;

						DisType avp = customers[v__].av + input.datas[v__].SERVICETIME + input.disOf[reCusNo(v__)][reCusNo(v)];
						newvPtw += max((DisType)0, avp - input.datas[v].DUEDATE);
						newvPtw += customers[v__].TW_X;
						av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						DisType awp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
						newvPtw += max((DisType)0, awp - input.datas[w].DUEDATE);

						DisType aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
							max(awp, input.datas[w].READYTIME);

						DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
						newvPtw += max((DisType)0, avjp - customers[vj].zv);
						newvPtw += customers[vj].TWX_;

						newwPtw = newvPtw;
					}
					else if (w_ == v) {

						// (w--)->(w-)->(w)->(w+)
						int w__ = customers[w_].pre;
						DisType aw = 0;
						newvPtw = 0;

						DisType awp = customers[w__].av + input.datas[w__].SERVICETIME + input.disOf[reCusNo(w__)][reCusNo(w)];
						newvPtw += max((DisType)0, awp - input.datas[w].DUEDATE);
						newvPtw += customers[w__].TW_X;
						aw = awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE :
							max(awp, input.datas[w].READYTIME);

						DisType avp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(v)];
						newvPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

						DisType av = avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE :
							max(avp, input.datas[v].READYTIME);

						DisType awjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(wj)];
						newvPtw += max((DisType)0, awjp - customers[wj].zv);
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

						newvPtw += max((DisType)0, abkp - input.datas[back].DUEDATE);
						newvPtw += customers[fpre].TW_X;

						DisType abk = abkp > input.datas[back].DUEDATE ? input.datas[back].DUEDATE :
							max(abkp, input.datas[back].READYTIME);

						lastav = abk;
						lastv = back;

						int pt = customers[front].next;

						while (pt != -1) {

							if (pt == back) {
								break;
							}

							DisType avp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
							newvPtw += max((DisType)0, avp - input.datas[pt].DUEDATE);

							DisType av = avp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
								max(avp, input.datas[pt].READYTIME);

							lastv = pt;
							lastav = av;
							pt = customers[pt].next;
						}


						DisType afrvp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(front)];
						newvPtw += max((DisType)0, afrvp - input.datas[front].DUEDATE);

						DisType afrv = afrvp > input.datas[front].DUEDATE ? input.datas[front].DUEDATE :
							max(afrvp, input.datas[front].READYTIME);

						int bkn = customers[back].next;

						DisType abknp = afrv + input.datas[front].SERVICETIME + input.disOf[reCusNo(front)][reCusNo(bkn)];
						newvPtw += max((DisType)0, abknp - customers[bkn].zv);
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
						max((DisType)0, max(awp, input.datas[w].READYTIME) - min(input.datas[w].DUEDATE, zwp));

					newwPtw +=
						max((DisType)0, max(avp, input.datas[v].READYTIME) - min(input.datas[v].DUEDATE, zvp));
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

					bestM.deltPc = max((DisType)0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND - input.Q)
						+ max((DisType)0, vQ - input.datas[v].DEMAND + input.datas[w].DEMAND - input.Q)
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

			auto getDeltPtw = [&]() {

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
						max((DisType)0, max(awp, input.datas[w].READYTIME) - min(input.datas[w].DUEDATE, zwp));

					// (w-) -> (v) -> (v+) -> (wj)
					newwPtw += customers[w_].TW_X;
					newwPtw += customers[wj].TWX_;

					DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

					newwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);
					DisType av =
						avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : max(avp, input.datas[v].READYTIME);

					DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];
					DisType zvjp = customers[wj].zv - input.datas[vj].SERVICETIME - input.disOf[reCusNo(wj)][reCusNo(vj)];

					newwPtw +=
						max((DisType)0, max(avjp, input.datas[vj].READYTIME) - min(input.datas[vj].DUEDATE, zvjp));
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
						max((DisType)0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND - input.Q)
						+ max((DisType)0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND + input.datas[w].DEMAND - input.Q)
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

			/*Route& rv = rts.getRouteByRid(customers[v].routeID);
			Route& rw = rts.getRouteByRid(customers[w].routeID);*/

			int wj = customers[w].next;
			int vj = customers[v].next;

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

			auto getDeltPtw = [&]() {

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
					newvPtw += max((DisType)0, awp - input.datas[w].DUEDATE);

					DisType aw =
						awp > input.datas[w].DUEDATE ? input.datas[w].DUEDATE : max(input.datas[w].READYTIME, awp);

					DisType awjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(wj)];
					DisType zwjp = customers[v3j].zv - input.datas[wj].SERVICETIME - input.disOf[reCusNo(wj)][reCusNo(v3j)];

					newvPtw +=
						max((DisType)0, max(awjp, input.datas[wj].READYTIME) - min(input.datas[wj].DUEDATE, zwjp));

					// (w-) -> (v) -> (vj) -> (vjj)-> (wjj)
					newwPtw += customers[w_].TW_X;
					newwPtw += customers[wjj].TWX_;

					DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

					newwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);
					DisType av =
						avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : max(avp, input.datas[v].READYTIME);

					DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

					// (w-) -> (v) -> (vj) -> (vjj)-> (wjj)
					DisType zvjjp = customers[wjj].zv - input.datas[vjj].SERVICETIME - input.disOf[reCusNo(wjj)][vjj];

					newwPtw += max((DisType)0, input.datas[vjj].READYTIME - zvjjp);

					DisType zvjj = zvjjp < input.datas[vjj].READYTIME ? input.datas[vjj].READYTIME : min(input.datas[vjj].DUEDATE, zvjjp);
					DisType zvjp = zvjj - input.disOf[reCusNo(vjj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;

					newwPtw +=
						max((DisType)0, max(avjp, input.datas[vj].READYTIME) - min(input.datas[vj].DUEDATE, zvjp));
					
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
						max((DisType)0, wQ - input.datas[w].DEMAND - input.datas[wj].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND + input.datas[vjj].DEMAND - input.Q)
						+ max((DisType)0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.datas[vjj].DEMAND + input.datas[w].DEMAND + input.datas[wj].DEMAND - input.Q)
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

			auto getDeltPtw = [&]() {

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
						max((DisType)0, max(awp, input.datas[w].READYTIME) - min(input.datas[w].DUEDATE, zwp));

					// (w-) -> (v) -> (vj) -> (vjj)-> (wj)
					newwPtw += customers[w_].TW_X;
					newwPtw += customers[wj].TWX_;

					DisType avp = customers[w_].av + input.datas[w_].SERVICETIME + input.disOf[reCusNo(w_)][reCusNo(v)];

					newwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);
					DisType av =
						avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : max(avp, input.datas[v].READYTIME);

					DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

					// (w-) -> (v) -> (vj) -> (vjj)-> (wj)
					DisType zvjjp = customers[wj].zv - input.datas[vjj].SERVICETIME - input.disOf[reCusNo(wj)][(vjj)];

					newwPtw += max((DisType)0, input.datas[vjj].READYTIME - zvjjp);

					DisType zvjj = zvjjp < input.datas[vjj].READYTIME ? input.datas[vjj].READYTIME : min(input.datas[vjj].DUEDATE, zvjjp);
					DisType zvjp = zvjj - input.disOf[reCusNo(vjj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;

					newwPtw +=
						max((DisType)0, max(avjp, input.datas[vj].READYTIME) - min(input.datas[vj].DUEDATE, zvjp));
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
						max((DisType)0, wQ - input.datas[w].DEMAND + input.datas[v].DEMAND + input.datas[vj].DEMAND + input.datas[vjj].DEMAND - input.Q)
						+ max((DisType)0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.datas[vjj].DEMAND + input.datas[w].DEMAND - input.Q)
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

			auto getDeltPtw = [&]() {

				DisType vPtw = rv.rPtw * rv.rWeight;
				DisType wPtw = rw.rPtw * rw.rWeight;

				DisType newvPtw = 0;
				DisType newwPtw = 0;

				// insert (v v+) to w and (w+)
				if (rw.routeID == rv.routeID) {

					//return bestM;
					if (v == w || vj == w) {
						return bestM;
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
					newwPtw += max((DisType)0, avp - input.datas[v].DUEDATE);

					DisType av =
						avp > input.datas[v].DUEDATE ? input.datas[v].DUEDATE : max(input.datas[v].READYTIME, avp);

					DisType avjp = av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(vj)];

					newwPtw += customers[wj].TWX_;
					DisType zvjp = customers[wj].zv - input.disOf[reCusNo(wj)][reCusNo(vj)] - input.datas[vj].SERVICETIME;
					//}

					newwPtw +=
						max((DisType)0, max(avjp, input.datas[vj].READYTIME) - min(input.datas[vj].DUEDATE, zvjp));

					// link v- and vjj
					newvPtw += customers[v_].TW_X;
					newvPtw += customers[vjj].TWX_;
					DisType avjjp = customers[v_].av + input.datas[v_].SERVICETIME + input.disOf[reCusNo(v_)][reCusNo(vjj)];
					newvPtw += max((DisType)0, avjjp - customers[vjj].zv);

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
						max((DisType)0, wQ + input.datas[v].DEMAND + input.datas[vj].DEMAND - input.Q)
						+ max((DisType)0, vQ - input.datas[v].DEMAND - input.datas[vj].DEMAND - input.Q)
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
			if (back == -1) {
				debug(r.head)
				debug(r.tail)
				debug(v)
				debug(w)
				rNextDisp(r);
				rPreDisp(r);
				rNextDisp(r);

			}
			#endif // CHECKING

			int f_ = customers[front].pre;
			int bj = customers[back].next;
			

			auto getDeltPtw = [&]() {

				int fj = customers[front].next;
				if (fj == back) {
					bestM = exchangevw(v, w, 1);
					return bestM;
				}
				int fjj = customers[fj].next;
				if (fjj == back) {
					bestM = exchangevw(v, w, 1);
					return bestM;
				}

				DisType newPtw = 0;
				DisType lastav = 0;
				int lastv = 0;

				lastv = back;
				DisType lastavp = customers[f_].av + input.datas[f_].SERVICETIME + input.disOf[reCusNo(f_)][reCusNo(back)];
				newPtw += customers[f_].TW_X;
				newPtw += max((DisType)0, lastavp - input.datas[lastv].DUEDATE);
				lastav = lastavp > input.datas[lastv].DUEDATE ? input.datas[lastv].DUEDATE :
					max(lastavp, input.datas[lastv].READYTIME);

				int pt = customers[lastv].pre;
				while (pt != -1) {

					DisType aptp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(pt)];
					newPtw += max((DisType)0, aptp - input.datas[pt].DUEDATE);

					DisType apt = aptp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
						max(aptp, input.datas[pt].READYTIME);
					lastv = pt;
					lastav = apt;
					if (pt == front) {
						break;
					}

					pt = customers[pt].pre;
				}

				DisType abjp = lastav + input.datas[lastv].SERVICETIME + input.disOf[reCusNo(lastv)][reCusNo(bj)];
				newPtw += max((DisType)0, abjp - customers[bj].zv);
				newPtw += customers[bj].TWX_;

				bestM.PtwOnly = newPtw - r.rPtw;
				bestM.deltPtw = (newPtw - r.rPtw) * r.rWeight * alpha;
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

		DeltPen _Nopt(vector<int>& nodes) { //16 Nopt*

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

				newwvPtw += max(awp - (wt > 0 ? customers[wt].zv : input.datas[0].DUEDATE), 0);
				newPtwNoWei += newwvPtw;

				newPc += max((DisType)0,
					(vt > 0 ? customers[vt].Q_X : 0) + (wt > 0 ? customers[wt].QX_ : 0) - input.Q);
				return newwvPtw;
			};

			DisType oldPtwNoWei = 0;
			DisType oldPtw = 0;
			DisType oldPc = 0;
			for (int i = 0; i < nodes.size(); i += 2) {
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
				Log(Log::Level::Error) << "doNopt(M) error" << endl;
			}
			else {
				debug("no this kind of move!")
					return false;
			}
			return true;
		}

		bool twoOptStar(TwoNodeMove& M) {

			int v = M.v;
			int w = M.w;

			Route& rv = rts.getRouteByRid(customers[v].routeID);
			Route& rw = rts.getRouteByRid(customers[w].routeID);

			/*if (rv.routeID == rw.routeID) {
				return false;
			}*/

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

				swap(rv.tail, rw.tail);

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

				swap(rv.tail, rw.tail);

				rUpdateAvQfrom(rv, v);
				rUpdateZvQfrom(rv, w);

				rUpdateAvQfrom(rw, wpre);
				rUpdateZvQfrom(rw, vnext);

				return true;

			}
			else {
				debug("no this two opt * move!")
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
				debug("no this inrelocate move")
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
				debug("no this inrelocate move")
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

			vector<int> ve;
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
				swap(customers[pt].next, customers[pt].pre);
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

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 1) {
				//_2optOpenvvj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 2) {
				// outrelocatevToww_
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 3) {
				// outrelocatevTowwj
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 4) {

				// inrelocatevv_
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 5) {

				// inrelocatevvj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

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

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}
					else {

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w__][w_] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}

				}
				else {

					yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[w__][w_] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

				}

			}
			else if (t.kind == 7) {
				// exchangevwj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
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

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}
					else {

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[wj][wjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}

				}
				else {

					yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[wj][wjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

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

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}
					else if (w_ == v) {

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}
					else {

						yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
						yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					}

				}
				else {

					yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
					yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

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

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[vjj][v3j] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[wj][wjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);


			}
			else if (t.kind == 10) {

				//exchangevvjvjjw(v, w); 三换一 v v+ v++ | w

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
				int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[vjj][v3j] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 11) {
				//exchangevvjw(v, w); 二换一 v v +  | w
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[vj][vjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 12) {

				//exchangevwwj(v, w); 一换二 v  | w w+

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[wj][wjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 13) {

				//outrelocatevvjTowwj(v, w); 扔两个 v v+  | w w+
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[vj][vjj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 14) {
				// v- v | w_ w
				//outrelocatevv_Toww_  | w-  v- v w

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int v__ = customers[v_].pre > input.custCnt ? 0 : customers[v_].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;

				yearTable[v__][v_] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[v][vj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w_][w] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

			}
			else if (t.kind == 15) {

				//reverse [v,w]
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;


				yearTable[v_][v] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
				yearTable[w][wj] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

				int pt = v;
				int ptn = customers[pt].next;
				while (pt != w) {

					yearTable[pt][ptn] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);

					pt = ptn;
					ptn = customers[ptn].next;
				}

			}
			/*else if (t.kind == 16) {

				for (int i = 0; i < t.ve.size(); i += 2) {
					int v = t.ve[i];
					int w = t.ve[(i + 3) % t.ve.size()];
					sumYear += yearTable[v][w];
				}
				sumYear = sumYear / t.ve.size() * 2;

			}*/
			else {
				debug("sol tabu dont include this move")
			}


			return true;
		}

		vector<int> getPtwNodes(Route& r, int ptwKind = 0) {

			vector<int> ptwNodes;

			ptwNodes.reserve(r.rCustCnt);

			int v = 0;

#if CHECKING
			if (r.rPtw == 0) {
				debug("no Ptw in this route")
					debug("no Ptw in this route")
					return ptwNodes;
			}
#endif // CHECKING

			/*auto findRangeCanDePtw1 = [&]() {

				int startNode = customers[r.tail].pre;
				int endNode = customers[r.head].next;

				int pt =  customers[r.head].next;
				bool stop = false;
				while (pt <=input.custCnt) {

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
			};*/

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
					debug(v != endNode)
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

			if (ptwNodes.size() == 0) {
				debug("-----------ptwNodes empty-------------")
			}

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
				sumYear = (yearTable[w][v] + yearTable[v_][wj]) / 2;
			}
			else if (t.kind == 1) {
				//_2optOpenvvj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				sumYear = (yearTable[v][w] + yearTable[w_][vj]) / 2;
			}
			else if (t.kind == 2) {
				//outrelocatevToww_
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				sumYear = (yearTable[v_][vj] + yearTable[w_][v] + yearTable[v][w]) / 3;

			}
			else if (t.kind == 3) {
				//outrelocatevTowwj
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				sumYear = (yearTable[v_][vj] + yearTable[w][v] + yearTable[v][wj]) / 3;
			}
			else if (t.kind == 4) {
				//inrelocatevv_
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

				sumYear = (yearTable[w_][wj] + yearTable[v_][w] + yearTable[w][v]) / 3;
			}
			else if (t.kind == 5) {
				//inrelocatevvj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				sumYear = (yearTable[w_][wj] + yearTable[v][w] + yearTable[w][vj]) / 3;
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

						sumYear = (yearTable[v_][w_] + yearTable[w_][v]
							+ yearTable[v][w]) / 3;
					}
					else {
						// v- v vj | w-- w- w
						sumYear = (yearTable[w__][v] + yearTable[v][w]
							+ yearTable[v_][w_] + yearTable[w_][vj]) / 4;

					}

				}
				else {

					sumYear = (yearTable[w__][v] + yearTable[v][w]
						+ yearTable[v_][w_] + yearTable[w_][vj]) / 4;
				}
			}
			else if (t.kind == 7) {
				//exchangevwj
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
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

						sumYear = (yearTable[w][v] + yearTable[v][wj]
							+ yearTable[wj][vj]) / 3;
					}
					else {
						// v- v vj |  w wj wjj
						sumYear = (yearTable[w][v] + yearTable[v][wjj]
							+ yearTable[v_][wj] + yearTable[wj][vj]) / 4;
					}

				}
				else {

					sumYear = (yearTable[w][v] + yearTable[v][wjj]
						+ yearTable[v_][wj] + yearTable[wj][vj]) / 4;
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

						sumYear = (yearTable[w_][v] + yearTable[w][vj]
							+ yearTable[v][w]) / 3;
					}
					else if (w_ == v) {

						sumYear = (yearTable[v_][w] + yearTable[w][v]
							+ yearTable[v][wj]) / 3;
					}
					else {
						// v- v vj |  w- w wj
						sumYear = (yearTable[w_][v] + yearTable[v][wj]
							+ yearTable[v_][w] + yearTable[w][vj]) / 4;
					}

				}
				else {

					sumYear = (yearTable[w_][v] + yearTable[v][wj]
						+ yearTable[v_][w] + yearTable[w][vj]) / 4;
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

				sumYear = (yearTable[v][w_] + yearTable[vjj][wjj]
					+ yearTable[v_][w] + yearTable[wj][v3j]) / 4;

			}
			else if (t.kind == 10) {

				//exchangevvjvjjw(v, w); 三换一 v v + v++ | w

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
				int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

				sumYear = (yearTable[v][w_] + yearTable[vjj][wj]
					+ yearTable[v_][w] + yearTable[w][v3j]) / 4;

			}
			else if (t.kind == 11) {
				//exchangevvjw(v, w); 二换一 v v +  | w

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

				sumYear = (yearTable[v][w_] + yearTable[vj][wj]
					+ yearTable[v_][w] + yearTable[w][vjj]) / 4;
			}
			else if (t.kind == 12) {

				//exchangevwwj(v, w); 一换二 v  | w w+

				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

				sumYear = (yearTable[w_][v] + yearTable[v][wjj]
					+ yearTable[v_][w] + yearTable[wj][vj]) / 4;

			}
			else if (t.kind == 13) {

				//outrelocatevvjTowwj(v, w); 扔两个 v v+  | w w+

				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
				//int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

				sumYear = (yearTable[v][w] + yearTable[vj][wj] + yearTable[v][vj]) / 3;
			}
			else if (t.kind == 14) {

				//outrelocatevv_Toww_  | w-  v- v w
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int v__ = customers[v_].pre > input.custCnt ? 0 : customers[v_].pre;
				int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
				int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
				sumYear = (yearTable[w_][v_] + yearTable[v][w] + yearTable[v__][vj]) / 3;
			}
			else if (t.kind == 15) {

				//reverse [v,w]
				int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
				int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

				sumYear = (yearTable[v][wj] + yearTable[w][v_]) / 2;
			}
			else if (t.kind == 16) {

				for (int i = 0; i < t.ve.size(); i += 2) {
					int v = t.ve[i];
					int w = t.ve[(i + 3) % t.ve.size()];
					sumYear += yearTable[v][w];
				}
				sumYear = sumYear / t.ve.size() * 2;

			}
			else {
				debug("get year of none")
			}
			return sumYear;
		}

		TwoNodeMove getMovesRandomly(function<bool(TwoNodeMove & t,TwoNodeMove & bestM)>updateBestM) {

			TwoNodeMove bestM;

			auto _2optEffectively = [&](int v) {

				int vid = customers[v].routeID;

				int v_ = customers[v].pre;
				if (v_ > input.custCnt) {
					v_ = 0;
				}

				double broaden = cfg.broaden[cfg.broadIndex];

				int v_pos = input.addSTJIsxthcloseOf[v][v_];
				//int v_pos = input.jIsxthcloseOf[v][v_];
				if (v_pos == 0) {
					v_pos += 50;
				}
				else {
					v_pos *= broaden;
				}
				v_pos = min(v_pos, input.custCnt);

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
					vjpos += 50;
				}
				else {
					vjpos *= broaden;
				}
				vjpos = min(vjpos, input.custCnt);

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

				double broaden = cfg.broaden[cfg.broadIndex];

				int devided = 7;

				if (vpos1 == 0) {
					vpos1 += 50;
				}
				else {
					vpos1 *= broaden;
				}

				vpos1 = min(vpos1, input.custCnt);

				if (vpos1 > 0) {

					int N = vpos1;
					int m = max(1, N / devided);
					myRandX.getMN(N, m);
					vector<int>& ve = myRandX.mpLLArr[N];
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
					vpos2 += 50;
				}
				else {
					vpos2 *= broaden;
				}

				vpos2 = min(vpos2, input.custCnt);

				if (vpos2 > 0) {

					int N = vpos2;
					int m = max(1, N / devided);

					m = max(1, m);
					myRandX.getMN(N, m);
					vector<int>& ve = myRandX.mpLLArr[N];

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
				vector<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[cfg.broadIndex][v];

				int N = relatedToV.size();
				int m = max(1, N / devided);

				myRandX.getMN(N, m);
				vector<int>& ve = myRandX.mpLLArr[N];
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

				double broaden = cfg.broaden[cfg.broadIndex];

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

				vjpos = min(vjpos, input.custCnt);
				int vrId = customers[v].routeID;

				int N = vjpos;
				int m = max(1, sqrt(N));

				myRandX.getMN(N, m);
				vector<int>& ve = myRandX.mpLLArr[N];
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

					wIsw_pos = min(wIsw_pos, input.custCnt);

					int mpos = myRand.pick(wIsw_pos);
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

					vector<int>ve = { v,vj,w_,w,m_,m };
					TwoNodeMove m16(ve, 16, _Nopt(ve));
					updateBestM(m16, bestM);
				}

			};

#endif // _3optEffectively

			bestM.reSet();

			int rId = -1;
			if (PtwConfRts.cnt > 0) {
				int index = -1;
				index = myRand.pick(PtwConfRts.cnt);
				rId = PtwConfRts.ve[index];
			}
			else if (PcConfRts.cnt > 0) {
				int index = -1;
				index = myRand.pick(PcConfRts.cnt);
				rId = PcConfRts.ve[index];
			}
			else {
				debug("error on conf route")
			}

			/*int confRCnt = PtwConfRts.cnt + PcConfRts.cnt;
			int index = -1;
			if (confRCnt > 0) {
				index = myRand.pick(confRCnt);
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
			if (r.rPc == 0 && r.rPtw == 0) {
				cout << "confrts error" << endl;
				debug(r.routeID)
					//debug(r.penalty)
			}
#endif // CHECKING

			if (r.rPtw > 0) {

				vector<int> ptwNodes = getPtwNodes(r);
				
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

					//sectorArea(v);
					//_3optEffectively(v);

					int w = v;
					int maxL = max(5, r.rCustCnt / 5);
					//int maxL = 5;
					//debug(r.rCustCnt)

					if (myRand.pick(3) == 0 ) {
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

					vector<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[cfg.broadIndex][v];
					int N = relatedToV.size();
					int m = N / 7;
					m = max(1, m);

					myRandX.getMN(N, m);
					vector<int>& ve = myRandX.mpLLArr[N];
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

		bool resetConfRtsByOneMove(vector<int> ids) {

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

		bool rCheckRoutePreNext(Route& r) {

			vector<int> pre;
			vector<int> next;
			for (int i = r.head; i != -1; i = customers[i].next) {
				next.push_back(i);
			}
			for (int i = r.tail; i != -1; i = customers[i].pre) {
				pre.push_back(i);
			}

			if (next.size() != pre.size()) {
				debug(next.size() != pre.size())
					return false;
			}

			for (int i = 0; i < next.size(); ++i) {
				if (next[i] != pre[next.size() - 1 - i]) {
					debug("not same ele in two arr")
						return false;
				}
			}

			return true;
		}

		bool doEject(vector<eOneRNode>& XSet) {

#if CHECKING

			int cnt = 0;
			for (int i : PtwConfRts.pos) {
				if (i >= 0) {
					++cnt;
				}
			}
			if (cnt != PtwConfRts.cnt) {
				debug(cnt != PtwConfRts.cnt)
					debug(cnt != PtwConfRts.cnt)
			}

			for (int i = 0; i < PtwConfRts.cnt; ++i) {
				Route& r = rts.getRouteByRid(PtwConfRts.ve[i]);

				if (r.rPtw == 0) {
					debug(r.routeID)
						debug(r.rPtw)
						debug(r.rPc)

				}
			}

			for (int i = 0; i < PcConfRts.cnt; ++i) {
				Route& r = rts.getRouteByRid(PcConfRts.ve[i]);

				if (r.rPc == 0) {
					debug(r.routeID)
						debug(r.rPtw)
						debug(r.rPc)
						debug(222)
				}

			}

			for (eOneRNode& en : XSet) {

				if (en.ejeVe.size() == 0) {
					debug("en.ejeVe.size() == 0")
						debug(en.rId)
						Route& r = rts.getRouteByRid(en.rId);
					rNextDisp(r);
					debug("en.ejeVe.size() == 0")
						continue;
				}
			}

			for (int i = 0; i < rts.cnt; ++i) {
				if (rts[i].rPtw > 0) {
					if (PtwConfRts.pos[rts[i].routeID] < 0) {
						debug(PtwConfRts.pos[rts[i].routeID])
							debug(444)
							debug(444)
					}
				}
				if (rts[i].rPc > 0 && rts[i].rPtw == 0) {
					if (PcConfRts.pos[rts[i].routeID] < 0) {
						debug(PcConfRts.pos[rts[i].routeID])
							debug(PcConfRts.pos[rts[i].routeID])
					}
				}
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
				if (r.rPc > 0 || r.rPtw > 0) {

					for (auto n : ejeNodesAfterSqueeze) {
						for (int c : n.ejeVe) {
							debug(c)
						}
					}
					debug("111error after eject")
						debug("111error after eject penalty exist")
						debug("222error after eject")
				}

#endif // CHECKING
			}

			resetConfRts();
			updatePen();

#if CHECKING
			if (penalty > 0 || PtwConfRts.cnt > 0 || PcConfRts.cnt > 0) {

				debug(penalty)
					debug(PtwConfRts.cnt)
					debug(PcConfRts.cnt)

					debug("error after eject penalty exist")

			}
#endif // CHECKING

			return true;
		}

		bool EPNodesCanEasilyPut() {

			for (int EPIndex = 0; EPIndex < EPsize();) {

				DisType oldpenalty = PtwNoWei + Pc;
				Vec<int> arr = EPve();
				int top = arr[EPIndex];

				Position bestP = findBestPosInSol(top);


				if (bestP.pen == 0) {

					Route& r = rts[bestP.rIndex];

					P[top] += cfg.Pwei0;
					//EPYearTable[top] = EPIter + cfg.EPTabuStep + myRand.pick(cfg.EPTabuRand);
					EPremoveByVal(top);

					rInsAtPos(r, bestP.pos, top);
					rUpdateAvQfrom(r, top);
					rUpdateZvQfrom(r, top);

					updatePen();
					resetConfRts();

#if CHECKING

					if (oldpenalty + bestP.pen != PtwNoWei + Pc) {
						debug("EPNodesCanEasilyPut findBestPosInSol pen update error ")
							debug(bestP.rIndex)
							debug(bestP.pos)
							debug(top)
							debug(oldpenalty)
							debug(bestP.pen)
							debug(PtwNoWei + Pc)
							rNextDisp(rts[bestP.rIndex]);
					}

#endif // CHECKING

				}
				else {
					++EPIndex;
				}

			}
			return true;
		}

		bool managerCusMem() {

			for (int i = input.custCnt + 3; i < customers.size(); ++i) {
				if (customers[i].routeID == -1) {

					for (int j = customers.size()-1; j > i; --j) {

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
						}
					}
				}
			}
			
			while (customers.back().routeID==-1)
			{
				customers.pop_back();
			} 
			while (input.datas.size()> customers.size())
			{
				input.datas.pop_back();
			}

			return true;
		}

		bool removeOneRouteRandomly(int index = -1) {

			//disUsangeOfHashArr();
			// delete one route randomly
			/*for (int i = 0; i <= input.custCnt; ++i) {
				P[i] = 1;
			}*/

			vector<CircleSector> angs(rts.cnt);
			vector<int> range(rts.cnt);
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
				index = myRand.pick(rts.size());
			}

			Route& rt = rts[index];

			vector<int> rtVe = rPutCusInve(rt);

			rReset(rt);
			rts.removeIndex(index);

			for (int pt : rtVe) {
				EPpush_back(pt);
			}

			updatePen();
			resetConfRts();
			managerCusMem();

			return true;
		}
		
		bool removeEveryRoute(Vec<bestSol>& arr) {

			bestSol sClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
			for (int index = 0; index < rts.size(); ++index) {
				Route& rt = rts[index];
				vector<int> rtVe = rPutCusInve(rt);
				rReset(rt);
				rts.removeIndex(index);

				for (int pt : rtVe) {
					EPpush_back(pt);
				}

				updatePen();
				resetConfRts();
				managerCusMem();

				arr.push_back(bestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty));
				setCurSolBy(sClone);
			}
			
			return true;
		}

		DisType verify() {

			vector<int> visitCnt(input.custCnt + 1, 0);

			int cusCnt = 0;
			DisType routesCost = 0;

			Ptw = 0;
			Pc = 0;

			for (int i = 0; i < rts.size(); ++i) {

				Route& r = rts[i];

				//cusCnt += r.rCustCnt;
				Ptw += rUpdateAvfrom(r, r.head);
				Pc += rUpdatePc(r);

				vector<int> cusve = rPutCusInve(r);
				for (int pt : cusve) {
					++cusCnt;
					++visitCnt[pt];
					if (visitCnt[pt] > 1) {
						return -1;
					}
				}
				routesCost += rUpdateRCost(r);
				//debug(routesCost)
			}

			for (int i = 0; i < rts.cnt; ++i) {
				rts[i].rWeight = 1;
			}

			updatePen();

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

		bool addWeightToRoute(TwoNodeMove& bestM) {

			if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly >= 0) {

				for (int i = 0; i < PtwConfRts.cnt; ++i) {
					Route& r = rts.getRouteByRid(PtwConfRts.ve[i]);
					r.rWeight += cfg.weightUpStep;
					Ptw += r.rPtw;
				}
				penalty = alpha * Ptw + beta * Pc;

			}
			return true;
		};

		vector<eOneRNode> ejeNodesAfterSqueeze;

		bool squeeze(Configuration squCon) {

			vector<bestSol> bestPool;

			bestSol sClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);

			bestPool.push_back(sClone);

			auto updateBestPool = [&](DisType Pc, DisType PtwNoWei) {

				bool isUpdate = false;
				bool dominate = false;

				vector<bestSol> ::iterator it = bestPool.begin();
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

					bestSol s(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);

					bestPool.push_back(s);
				}

				return isUpdate;
			};

			auto getBestSolIndex = [&]() {

				DisType min1 = DisInf;
				int index = 0;
				for (int i = 0; i < bestPool.size(); ++i) {
					//debug(i)
					DisType temp = bestPool[i].Pc + bestPool[i].PtwNoWei;
					if (temp < min1) {
						min1 = temp;
						index = i;
					}
				}
				return index;
			};

			auto getMinPsumSolIndex = [&]() {

				int min1 = IntInf;
				int index = 0;

				for (int i = 0; i < bestPool.size(); ++i) {
					customers = bestPool[i].customers;
					rts = bestPool[i].rts;
					resetConfRts();
					updatePen();

					vector<eOneRNode> ret = ejectFromPatialSol();

					int bestCnt = 1;
					int sum = 0;
					for (eOneRNode& en : ret) {
						sum += en.Psum;
						/*for (int c : en.ejeVe) {
							sum = max(sum,P[c]);
						}*/
					}

					if (sum < min1) {
						bestCnt = 1;
						min1 = sum;
						index = i;
						ejeNodesAfterSqueeze = ret;
					}
					else if (sum == min1) {
						++bestCnt;
						if (myRand.pick(bestCnt) == 0) {
							index = i;
							ejeNodesAfterSqueeze = ret;
						}
					}
				}

				return index;
			};

			alpha = 1;
			beta = 1;

			for (int i = 0; i < rts.cnt; ++i) {
				rts[i].rWeight = 1;
			}

			resetConfRts();
			updatePen();

			int deTimeOneTurn = 0;
			int contiTurnNoDe = 0;

			squIter += cfg.yearTabuLen + cfg.yearTabuRand;

			squCon.squContiIter = squCon.squMinContiIter;
			pBestThisTurn = DisInf;

			int contiNotDe = 0;

			//debug(squIter)

#if DISDOUBLE

			auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

				if (t.deltPen.deltPc == DisInf || t.deltPen.deltPtw == DisInf) {
					return false;
				}

				if (lfeq4(t.deltPen.deltPc + t.deltPen.deltPtw
					, golbalBestM.deltPen.deltPc + golbalBestM.deltPen.deltPtw)) {
					if (getYearOfMove(t) < getYearOfMove(golbalBestM)) {
						golbalBestM = t;
					}
				}
				else if (t.deltPen.deltPc + t.deltPen.deltPtw
					< golbalBestM.deltPen.deltPc + golbalBestM.deltPen.deltPtw) {
					golbalBestM = t;
				}

				//}
				return true;
			};

#else

			auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

				/*LL tYear = getYearOfMove(t);
				bool isTabu = (squIter <= tYear);
				/*bool isTabu = false;
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

#endif // DISDOUBLE
			
			while (penalty > 0 && !lyhTimer.isTimeOut()) {

				TwoNodeMove bestM = getMovesRandomly(updateBestM);

				if (bestM.deltPen.PcOnly == DisInf || bestM.deltPen.PtwOnly == DisInf) {
					debug("squeeze fail find move")
						debug(squIter)
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

				vector<vector<int>> oldRoutes;
				vector<int> oldrv;
				vector<int> oldrw;

				DisType oldpenalty = penalty;
				DisType oldPtw = Ptw;
				DisType oldPc = Pc;
				DisType oldPtwNoWei = PtwNoWei;
				DisType oldPtwOnly = PtwNoWei;
				DisType oldPcOnly = Pc;
				DisType oldRcost = updateRtsCost();

				Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
				Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
				oldrv = rPutCusInve(rv);
				oldrw = rPutCusInve(rw);

				for (int i = 0; i < rts.cnt; ++i) {
					Route& r = rts[i];
					if (rPutCusInve(r).size() != r.rCustCnt) {
						debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(r.routeID)
							debug(r.rCustCnt)
							debug(rPutCusInve(r).size())
							rNextDisp(r);
							rNextDisp(r);

					}

					int pt = r.head;
					while (pt != -1) {
						if (pt >= customers.size()) {
							debug(pt)
								debug(customers.size())
								debug(customers.size())
						}
						pt = customers[pt].next;
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
				updatePen();
				//updatePen(bestM.deltPen);

#if CHECKING

				updateRtsCost();
				updatePen();
				bool iderror = false;
				bool penaltyWeiError = 
					!DISlfeq(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty);
				bool penaltyError =
					!DISlfeq(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly, PtwNoWei + Pc);
				
				bool RoutesCostError = false;
					//!DISlfeq(oldRcost + bestM.deltPen.deltCost, RoutesCost);
				for (int i = 0; i < rts.cnt; ++i) {
					Route& r = rts[i];
					if (rPutCusInve(r).size() != r.rCustCnt) {
						debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(r.routeID)
							debug(r.rCustCnt)
							debug(rPutCusInve(r).size())
							rNextDisp(r);
						rNextDisp(r);

					}

					int pt = r.head;
					while (pt != -1) {
						if (pt >= customers.size()) {
							debug(pt)
								debug(customers.size())
								debug(customers.size())
						}
						pt = customers[pt].next;
					}
				}

				if (iderror || penaltyWeiError|| penaltyError|| RoutesCostError){

					debug("squeeze penalty update error!")

					debug(bestM.v)
					debug(bestM.w)
					debug(bestM.kind)

					debug(rv.routeID == rw.routeID)

					debug(iderror)
					debug(penaltyWeiError)
					debug(penaltyError)
					debug(RoutesCostError)


					debug(oldpenalty)

					debug(bestM.deltPen.deltPc)
					debug(bestM.deltPen.deltPtw)
					debug(bestM.deltPen.deltCost)
					debug(oldpenalty + bestM.deltPen.deltPc+ bestM.deltPen.deltPtw)
					debug(penalty)
					debug(bestM.deltPen.deltPc + bestM.deltPen.deltPtw)
					debug(penalty - oldpenalty)

					debug(oldRcost)
					debug(bestM.deltPen.deltCost)
					debug(RoutesCost)
					debug(RoutesCost-oldRcost)

					debug(oldPtw)
					debug(bestM.deltPen.deltPtw)
					debug(Ptw)
					debug(Ptw - oldPtw)
					
					
					debug(oldPtwNoWei)
					debug(bestM.deltPen.PtwOnly)
					debug(PtwNoWei)

					debug(oldPc)
					debug(bestM.deltPen.deltPc)
					debug(bestM.deltPen.PcOnly)
					debug(Pc)

					cout << "oldrv: ";

					for (auto i : oldrv) {
						cout << i << " ,";
					}
					cout << endl;
					cout << "oldrw: ";
					for (auto i : oldrw) {
						cout << i << " ,";
					}
					cout << endl;

					rNextDisp(rv);
					rNextDisp(rw);

					debug(squIter)

					debug(DEFabs(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty))
					debug("false false")
					debug("false false")

				}
#endif // CHECKING

				//resetConfRts();
				resetConfRtsByOneMove({ rvId,rwId });
				addWeightToRoute(bestM);

				if (penalty < pBestThisTurn) {
					contiNotDe = 0;
					cfg.broadIndex = 0;
					pBestThisTurn = penalty;
				}
				else {
					++contiNotDe;
				}

				bool isDown = updateBestPool(Pc, PtwNoWei);

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
					//	minW = min(minW, rts[i].rWeight);
					//}
					//minW = minW/3 + 1;
					//for (int i = 0; i < rts.size(); ++i) {
					//	//rts[i].rWeight = rts[i].rWeight - minW +1;
					//	rts[i].rWeight = rts[i].rWeight/ minW +1;
					//}
					//updatePen();
				//}

				if (contiNotDe == squCon.squContiIter) {

					if (penalty < 1.1 * pBestThisTurn && squCon.squContiIter < squCon.squMaxContiIter) {
						squCon.squContiIter += squCon.squIterStepUp;
						squCon.squContiIter = min(squCon.squMaxContiIter, squCon.squContiIter);
						cfg.broadIndex += 1;
						cfg.broadIndex = min(cfg.broadIndex, cfg.broaden.size() - 1);
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
				updatePen();

				return true;
			}
			else {

				bestPool.push_back(sClone);
				int index = getMinPsumSolIndex();
				//debug(index)
				customers = bestPool[index].customers;
				rts = bestPool[index].rts;
				PtwNoWei = bestPool[index].PtwNoWei;
				Ptw = bestPool[index].Ptw;
				Pc = bestPool[index].Pc;
				penalty = bestPool[index].penalty;
				EPr = bestPool[index].EPr;

				/*for (int i = 0; i < rts.cnt; ++i) {
					rts[i].rWeight = 1;
				}*/

				resetConfRts();

#if CHECKING
				DisType oldp = penalty;
				/*debug(penalty)
				debug("resetConfRts: ")*/

				updatePen();
				if (oldp != penalty) {
					debug(oldp)
					debug(penalty)
					debug(penalty)
				}
#endif // CHECKING		
				/*debug(PtwNoWei)
				debug(Pc)
				debug(penalty)*/

				/*PtwNoWei = bestPool[index].PtwNoWei;
				Pc = bestPool[index].Pc;
				penalty = bestPool[index].penalty;*/

				return false;
			}
			return true;
		}

		bool ejectLocalSearch() {

			minEPcus = IntInf;
			squIter += cfg.yearTabuLen + cfg.yearTabuRand;
			LL maxOfPval = -1;

			#ifdef SAVE_LOCAL_BEST_SOL
			bestSol solClone(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
			bestSol gBestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
			auto cloneP = P;

			LL maxContiNode = 1024;

			LL contiNoReduce = 1;
			int minRPSolCnt = 1;

			auto updateGBestSolPool = [&]() {

				auto& curBestSol = gBestSol;

				if (EPr.rCustCnt < curBestSol.EPr.rCustCnt) {
					minRPSolCnt = 1;
					gBestSol = bestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
					return true;
				}
				else if (EPr.rCustCnt == curBestSol.EPr.rCustCnt) {
					++minRPSolCnt;
					if (myRand.pick(minRPSolCnt) == 0) {
						gBestSol = bestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
					}
					return false;
				}
				return false;
			};

			int curUse = 0;// 0 global 1 pool

			#endif // SAVE_LOCAL_BEST_SOL

			while (!lyhTimer.isTimeOut()) {

				++EPIter;
				EPNodesCanEasilyPut();

				/*if (EPr.rCustCnt < minEPcus) {
					deOut(minEPcus)deOut(EPr.rCustCnt)debug(contiNoReduce)
				}*/

				minEPcus = min(minEPcus, EPr.rCustCnt);
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
						solClone = bestSol(customers, rts, EPr, Pc, Ptw, PtwNoWei, penalty);
					}
					
					if (myRand.pick(5) < 4 ) {
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
					squIter += cfg.yearTabuLen + cfg.yearTabuRand;

					continue;
				}
				#endif // SAVE_LOCAL_BEST_SOL

				vector<int> EPrVe = rPutCusInve(EPr);
				int top = EPrVe[myRand.pick(EPrVe.size())];
				Position bestP = findBestPosInSol(top);
				Route& r = rts[bestP.rIndex];
				EPremoveByVal(top);

				P[top] += cfg.Pwei0;
				maxOfPval = max(P[top], maxOfPval);

				if (maxOfPval >= 10000) {
					for (auto& i : P) {
						i = i / 2 + 1;
					}
					maxOfPval = -1;
					debug("pinghua")
				}

				#if CHECKING
				DisType oldpenalty = PtwNoWei + Pc;
				#endif

				rInsAtPos(r, bestP.pos, top);
				rUpdateAvQfrom(r, top);
				rUpdateZvQfrom(r, top);
				updatePen();
				resetConfRts();

				#if CHECKING
				if (!DISlfeq(oldpenalty + bestP.pen, PtwNoWei + Pc)) {
					debug("deleteRoute findBestPosInSol pen update error ")
						debug(bestP.rIndex)
						debug(bestP.pos)
						debug(top)
						debug(oldpenalty)
						debug(bestP.pen)
						debug(PtwNoWei + Pc)
						rNextDisp(rts[bestP.rIndex]);
				}
				#endif // CHECKING

				if (bestP.pen == 0) {
					continue;
				}
				else {

					Configuration con = cfg;
					con.squContiIter = 100;
					con.squMinContiIter = 100;
					con.squMaxContiIter = 199;
					con.squIterStepUp = 10;

					bool squ = squeeze(con);
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
								yearTable[cpre][c] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);
								yearTable[c][cnext] = squIter + cfg.yearTabuLen + myRand.pick(cfg.yearTabuRand);*/
								P[c] += cfg.Pwei1;
								maxOfPval = max(P[c], maxOfPval);
								
								//P[c] += max(log(P[c]), cfg.Pwei1);
								//debug(max(log(P[c]), cfg.Pwei1))
							}
						}

						doEject(XSet);
						int Irand = input.custCnt / EPr.rCustCnt/6;
						Irand = max(Irand,10);
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

		bool patternMakeBigBigger() {

			int I1000 = myRand.pick(cfg.Irand);
			int Irand = I1000;

			LL iter = 0;

			vector<int> arr;
			arr.reserve(rts.cnt);
			// iota(arr.begin(),arr.end(),0);

			int maxRId = 0;
			for (int i = 0; i < rts.size(); ++i) {
				maxRId = max(maxRId,rts[i].routeID);
				arr.push_back(rts[i].routeID);
			}

			sort(arr.begin(), arr.end(), [&](int a,int b) {
				return rts.getRouteByRid(a).rCustCnt < rts.getRouteByRid(b).rCustCnt;
			});

			ConfSet bigRts(maxRId+1);
			ConfSet litRts(maxRId+1);

			for (int i = 0; i < arr.size(); ++i) {
				if (i < arr.size() / 2) {
					bigRts.ins(arr[i]);
				}
				else {
					litRts.ins(arr[i]);
				}
			}

			Timer t1(1);

			vector<int> kindSet = { /*0,1,6,7,*/ 2,3,8 };
			int N = 60;
			int m = 10;


			auto makeBigBiger = [&]() {

				while (!t1.isTimeOut()) {

					int bId = myRand.pick(bigRts.cnt);
					bId = bigRts.ve[bId];

					int litId = myRand.pick(litRts.cnt);
					litId = litRts.ve[litId];

					auto bigR = rPutCusInve(rts.getRouteByRid(bId));
					auto litR = rPutCusInve(rts.getRouteByRid(litId));

					for (int v : bigR) {
						for (int w: litR) {
							for (int kind : kindSet) {
								DeltPen d = estimatevw(kind, v, w);
								if (d.deltPc + d.deltPtw == 0) {
									TwoNodeMove m(v, w, kind, d);
									return m;
								}
							}
						}
					}
				}
				return TwoNodeMove(0, 0, 0, DeltPen());
			};


			ShuffleCards sc;

			++squIter;

			do {

				TwoNodeMove bestM = makeBigBiger();
				//if (myRand.pick(3) != 0) {
				/*while (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0 && !t1.isTimeOut()) {
					bestM = makeBigBiger();
				}*/

				if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {
					break;
				}

				#if CHECKING

				Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
				Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
				if (customers[bestM.v].routeID == -1 || customers[bestM.w].routeID == -1) {

					debug(bestM.v)
						debug(bestM.w)
						debug("error")
				}

				vector<int> oldrv;
				vector<int> oldrw;
				int pt = rv.head;

				while (pt != -1) {
					oldrv.push_back(pt);
					pt = customers[pt].next;
				}

				pt = rw.head;
				while (pt != -1) {
					oldrw.push_back(pt);
					pt = customers[pt].next;
				}
				DisType oldpenalty = penalty;
				DisType oldPtw = Ptw;
				DisType oldPc = Pc;

				#endif // CHECKING

				updateYearTable(bestM);
				doMoves(bestM);
				++squIter;

				#if CHECKING

				if (oldpenalty != penalty) {

					debug(PtwConfRts.cnt)
						debug(PcConfRts.cnt)
						debug("patternAdjustment penalty update error!")
						debug((rv.routeID == rw.routeID))

						debug(oldPtw)
						debug(bestM.deltPen.deltPtw)
						debug(Ptw)

						debug(oldPc)
						debug(bestM.deltPen.deltPc)
						debug(Pc)

						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)
						for (auto i : oldrv) {
							cout << i << " ";
						}
					cout << endl;
					for (auto i : oldrw) {
						cout << i << " ";
					}
					cout << endl;
					rNextDisp(rv);
					rNextDisp(rw);
					debug(iter)
						debug("oldpenalty != penalty")
						debug("error oldpenalty != penalty")
				}

				if (rw.rCustCnt == 0 || rv.rCustCnt == 0) {

					debug("one route empty!")
						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)
						debug(oldpenalty)
						//debug(bestM.deltPen)
						rNextDisp(rv);
					rNextDisp(rw);
				}

				#endif // CHECKING

			} while (++iter < I1000 && !t1.isTimeOut());
			//debug(iter)

			updatePen();

			return true;
		}

		bool patternAdjustment(int Irand = -1) {

			int I1000 = myRand.pick(cfg.Irand);
			if (Irand > 0) {
				I1000 = Irand;
			}

			LL iter = 0;

			Timer t1(1);

			vector<int> kindSet = { 0,1,6,7,/*8,9,10,2,3,4,5*/ };
			int N = 60;
			int m = 10;

			auto getDelt0MoveRandomly = [&]() {

				TwoNodeMove ret;

				while (!t1.isTimeOut()) {

					int v = myRand.pick(input.custCnt) + 1;

					if (customers[v].routeID == -1) {
						continue;
					}

					m = max(1, m);
					myRandX.getMN(N, m);
					vector<int>& ve = myRandX.mpLLArr[N];
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
							#if CHECKING
							if (d.deltPc + d.deltPtw < 0) {
								debug(v)
									debug(w)
									debug(kind)
									debug(penalty)
									debug(Ptw)
									debug(PtwNoWei)
									debug(Pc)
									debug(d.deltPc + d.deltPtw)
									debug(d.deltPc + d.deltPtw)
							}
							#endif // CHECKING

							#ifdef ATTRIBUTETABU

							if (d.deltPc + d.deltPtw == 0) {
								TwoNodeMove m(v, w, kind, d);
								ret = m;
								if (squIter > getYearOfMove(m)) {
									return m;
								}
							}
							#else

							#endif // ATTRBUTETABU

						}
					}
				}
				//debug(11111)
				return ret;
			};

			ShuffleCards sc;

			++squIter;

			do {

				TwoNodeMove bestM = getDelt0MoveRandomly();;
				//if (myRand.pick(3) != 0) {

				/*while (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0 && !t1.isTimeOut()) {
					bestM = getDelt0MoveRandomly();
				}*/
				
				if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {
					break;
				}

				#if CHECKING

				Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
				Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
				if (customers[bestM.v].routeID == -1 || customers[bestM.w].routeID == -1) {

					debug(bestM.v)
						debug(bestM.w)
						debug("error")
				}

				vector<int> oldrv;
				vector<int> oldrw;
				int pt = rv.head;

				while (pt != -1) {
					oldrv.push_back(pt);
					pt = customers[pt].next;
				}

				pt = rw.head;
				while (pt != -1) {
					oldrw.push_back(pt);
					pt = customers[pt].next;
				}
				DisType oldpenalty = penalty;
				DisType oldPtw = Ptw;
				DisType oldPc = Pc;

				#endif // CHECKING

				updateYearTable(bestM);
				doMoves(bestM);
				++squIter;

				#if CHECKING

				if (oldpenalty != penalty) {

					debug(PtwConfRts.cnt)
						debug(PcConfRts.cnt)
						debug("patternAdjustment penalty update error!")
						debug((rv.routeID == rw.routeID))

						debug(oldPtw)
						debug(bestM.deltPen.deltPtw)
						debug(Ptw)

						debug(oldPc)
						debug(bestM.deltPen.deltPc)
						debug(Pc)

						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)
						for (auto i : oldrv) {
							cout << i << " ";
						}
					cout << endl;
					for (auto i : oldrw) {
						cout << i << " ";
					}
					cout << endl;
					rNextDisp(rv);
					rNextDisp(rw);
					debug(iter)
						debug("oldpenalty != penalty")
						debug("error oldpenalty != penalty")
				}

				if (rw.rCustCnt == 0 || rv.rCustCnt == 0) {

					debug("one route empty!")
						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)
						debug(oldpenalty)
						//debug(bestM.deltPen)
						rNextDisp(rv);
					rNextDisp(rw);
				}

				#endif // CHECKING

			} while (++iter < I1000 && !t1.isTimeOut());
			//debug(iter)
			updatePen();
			return true;
		}

		bool ejectPatternAdjustment() {

			Vec<eOneRNode> ens;
			Vec<int> cus = rPutCusInve(EPr);
			auto cmp = [&](const int& a, const int& b) {
				return P[a] > P[b];
			};
			sort(cus.begin(), cus.end(), cmp);

			
			map<int, Vec<int>>mp;

			unordered_set<int> set1;
			for (int v : cus) {

				for (int wpos = 0; wpos <= 10; ++wpos) {
					//int wpos = myRand.pick(10);
					int w = input.allCloseOf[v][wpos];
					if (customers[w].routeID != -1 && P[w] >= P[v]) {

						if (set1.count(w) == 0) {
							mp[customers[w].routeID].push_back(w);
							set1.insert(w);
						}
						else {
							debug("a")
						}

						/*debug(wpos)
						debug(w)
						debug(v)*/
						//break;
					}
				}
			}

			for (auto it : mp) {
				eOneRNode en(it.first);
				Route& r = rts.getRouteByRid(it.first);

				for (int c : it.second) {
					if (en.ejeVe.size() + 1 < r.rCustCnt) {
						en.ejeVe.push_back(c);
					}
				}
				ens.push_back(en);
			}
			doEject(ens);
			patternAdjustment();

			return true;
		}

		vector<eOneRNode> ejectFromPatialSol() {

			vector<eOneRNode>ret;

			vector<int>confRSet;

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
				int tKmax = cfg.minKmax;
				tKmax = cfg.maxKmax;

				while (tKmax <= cfg.maxKmax) {
					auto en = ejectOneRouteOnlyP(r, 2, tKmax);

					if (retNode.ejeVe.size() == 0) {
						retNode = en;
					}
					else {
						//double rateMoreEj = ((double)en.ejeVe.size() / retNode.ejeVe.size());
						if (en.Psum < retNode.Psum) {
							//bool satisfy1 = en.getMaxEle() < retNode.getMaxEle();
							//bool satisfy1 = false;
							//bool satisfy1 = en.Psum * retNode.ejeVe.size() < retNode.Psum * en.ejeVe.size();
							bool satisfy1 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();
							//bool satisfy1 = en.Psum * 1.5 < retNode.Psum;
							if (satisfy1) {
								deOut(en.Psum)debug(en.ejeVe.size())
								deOut(retNode.Psum)debug(retNode.ejeVe.size())
								//debug("satisfy12")
								retNode = en;
							}
						}
					}
					++tKmax;
				}

				//debug(retNode.ejeVe.size())
				//eOneRNode retNode = ejectOneRouteOnlyP(r, 2, cfg.maxKmax);
				auto en = ejectOneRouteMinPsumGreedy(r);

				if (retNode.ejeVe.size() == 0) {
					retNode = en;
				}
				else {

					if (en.Psum < retNode.Psum) {
						//bool satisfy2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();
						bool satisfy2 = en.ejeVe.size() > cfg.maxKmax;
						//bool satisfy2 = true;
						if (satisfy2) {

							deOut(en.Psum)debug(en.ejeVe.size())
							deOut(retNode.Psum)debug(retNode.ejeVe.size())
							debug(satisfy2)
							retNode = en;
						}
					}

				}

#if CHECKING

				if (retNode.ejeVe.size() == 0) {
					debug(r.rPtw)
						debug(r.rPc)
						rNextDisp(r);
					outVe(P)
						system("pause");
				}

				if (retNode.ejeVe.size() == 0) {
					rNextDisp(r);
					debug("retNode.ejeVe.size() == 0")
						debug("retNode.ejeVe.size() == 0")
				}
#endif // CHECKING

				ret.push_back(retNode);
			}

			return ret;
		}

		eOneRNode ejectOneRouteOnlyHasPcMinP(Route& r,int Kmax) {

			eOneRNode noTabuN(r.routeID);
			noTabuN.Psum = 0;

			vector<int> R = rPutCusInve(r);

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

			priority_queue<int, Vec<int>, decltype(cmpMinP)> qu(cmpMinP);
			
			for (const int& c : R) {
				qu.push(c);
			}

			DisType rPc = r.rPc;
			DisType rQ = r.rQ;

			while (rPc > 0) {

				int ctop = qu.top();
				//debug(P[ctop])
				qu.pop();
				rQ -= input.datas[ctop].DEMAND;
				rPc = max(0, rQ - input.Q);
				noTabuN.ejeVe.push_back(ctop);
				noTabuN.Psum += P[ctop];
			}

			return noTabuN;

		}

		eOneRNode ejectOneRouteOnlyP(Route& r, int kind, int Kmax) {

			eOneRNode noTabuN(r.routeID);
			
			int sameCnt = 1;
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
						if (etemp.ejeVe.size() * etemp.Psum < noTabuN.Psum * noTabuN.ejeVe.size()*1.4) {
						//if (etemp.ejeVe.size() < noTabuN.ejeVe.size()) {
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
				//	/*if (myRand.pick(sameCnt) == 0) {
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
						: max(customers[n].avp, input.datas[n].READYTIME);
					customers[n].TW_X = max((DisType)0, customers[n].avp - input.datas[n].DUEDATE);
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
				DisType ptw = max((DisType)0, avp - customers[v].zv) + customers[v].TWX_;
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
				if (pre > input.custCnt&& next > input.custCnt) {
					debug("no this sit")
						debug("no this sit")
				}
#endif // CHECKING

				DisType ptw = 0;

				while (next != -1) {

					//debug(pt)
					DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(next)];
					ptw = max((DisType)0, avnp - customers[next].zv) + customers[next].TWX_ + customers[pre].TW_X;

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

			vector<int> R = rPutCusInve(r);

			vector<int> ptwArr;

			if (r.rPtw > 0) {

				ptwArr = R;
				Kmax = min(Kmax, ptwArr.size() - 1);
			}
			else if (r.rPc > 0) {

				//return ejectOneRouteOnlyHasPcMinP(r, Kmax);
				ptwArr = R;
				Kmax = min(Kmax, ptwArr.size() - 1);
				
			}
			else {
				return {};
			}

			int k = 0;
			int N = ptwArr.size() - 1;
			vector<int> ve(Kmax + 1, -1);

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

			if (
				!DISlfeq(r.rQ, rQStart)
				|| !DISlfeq(PcStart, r.rPc)
				|| !DISlfeq(PtwStart, r.rPtw)
				|| !DISlfeq(PtwStart, customers[r.head].TWX_)
				|| !DISlfeq(PtwStart, customers[r.tail].TW_X)
				|| !DISlfeq(customers[r.head].QX_, rQStart)
				|| !DISlfeq(customers[r.tail].Q_X, rQStart)
				) {

				debug(!DISlfeq(r.rQ, rQStart))
					debug(!DISlfeq(PcStart, r.rPc))
					debug(!DISlfeq(PtwStart, r.rPtw))
					debug(!DISlfeq(PtwStart, customers[r.head].TWX_))
					debug(!DISlfeq(PtwStart, customers[r.tail].TW_X))
					debug(!DISlfeq(customers[r.head].QX_, rQStart))
					debug(!DISlfeq(customers[r.tail].Q_X, rQStart))

					de15lf(r.rQ)
					de15lf(rQStart)
					de15lf(PcStart)
					de15lf(r.rPc)
					de15lf(PtwStart)
					de15lf(r.rPtw)
					de15lf(customers[r.head].TWX_)

					de15lf(depot0TW_XStart)
					de15lf(depotN1TW_XStart);

				de15lf(customers[r.head].QX_)
					de15lf(customers[r.tail].Q_X)
					de15lf(customers[r.tail].TW_X)

					outVe(R)
					rNextDisp(r);
				rPreDisp(r);

				debug(rCheckRoutePreNext(r))
					debug("eject not restore state")
					debug("eject not restore state")
			}

			vector<int> v1 = rPutCusInve(r);
			vector<int> v2;

			for (int pt = customers[r.tail].pre; pt <= input.custCnt; pt = customers[pt].pre) {
				v2.push_back(pt);
			}

			if (v1.size() != v2.size()) {
				debug("v1.size() != v2.size()")
					debug("v1.size() != v2.size()")
			}
#endif // CHECKING

			return noTabuN;
		}

		eOneRNode ejectOneRouteMinPsumGreedy
		(Route& r, eOneRNode cutBranchNode = eOneRNode()) {

			eOneRNode ret(r.routeID);
			ret.Psum = 0;
			vector<int> R = rPutCusInve(r);

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

			priority_queue<int, Vec<int>, decltype(cmp)> qu(cmp);

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
				DisType ptw = max((DisType)0, avnp - customers[nex].zv) + customers[nex].TWX_ + customers[pre].TW_X;

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
		
		bool setCurSolBy(bestSol& sClone) {

			customers = sClone.customers;
			rts = sClone.rts;
			PtwNoWei = sClone.PtwNoWei;
			Ptw = sClone.Ptw;
			Pc = sClone.Pc;
			penalty = sClone.penalty;
			EPr = sClone.EPr;
			resetConfRts();
			return true;
		}
		
		bool minimizeRN() {

			gamma = 0;
			initDiffSituation();
			bool isSucceed = false;
			lyhTimer.reStart();

			while (!lyhTimer.isTimeOut()) {

				removeOneRouteRandomly();

				fill(P.begin(), P.end(), 1);
				bool isDelete = ejectLocalSearch();
				Log(Log::Level::Warning) << "rts.size(): " << rts.size() << endl;
				if (rts.size() <= ourTarget) {
					if ((EPsize() == 0 && penalty == 0)) {
						isSucceed = true;
					}
					break;
				}
			}

			DisType state = verify();

			if (state > 0) {

				Log(Log::Level::Warning) << "success: "
					<< " minEPcus: " << (minEPcus)
					<< " EPsize: " << (EPsize())
					<< " rts.size(): " << (rts.size())
					<< endl;
			}
			else {

				Log(Log::Level::Warning) << "fail:"
					<< " minEPcus: " << (minEPcus)
					<< " EPsize: " << (EPsize())
					<< " rts.size(): " << (rts.size())
					<< endl;
			}

			/*Log(Log::Level::Warning) << ("P");
			for (int p : P) {
				Log(Log::Level::Warning) << p << ", ";
			}
			Log(Log::Level::Warning) << endl;*/

			output.runTime = lyhTimer.getRunTime();
			output.EP = rPutCusInve(EPr);
			output.minEP = minEPcus;
			output.PtwNoWei = PtwNoWei;
			output.Pc = Pc;

			for (int i = 0; i < rts.size(); ++i) {
				output.rts.push_back(rPutCusInve(rts[i]));
			}
			//lyhTimer.disp();
			output.state = state;
			return true;
		}

		TwoNodeMove evalMinRLMovesFromConfRts(function<bool(TwoNodeMove & t, TwoNodeMove & bestM)>updateBestM) {

			TwoNodeMove bestM;

			vector<int> confRts;

			confRts.insert(confRts.begin(),PtwConfRts.ve.begin(), PtwConfRts.ve.begin()+ PtwConfRts.cnt);
			confRts.insert(confRts.begin(), PcConfRts.ve.begin(), PcConfRts.ve.begin()+ PcConfRts.cnt);
			//debug(confRts.size())
			for (int rId : confRts) {
				Route& r = rts.getRouteByRid(rId);
				vector<int> cusV = rPutCusInve(r);
				for (int v : cusV) {

					if (customers[v].routeID == -1) {
						continue;
					}
					for (int wpos = 0; wpos < 50; ++wpos) {

						int w = input.allCloseOf[v][wpos];
						if (customers[w].routeID == -1) {
							continue;
						}
						for (int i = 0; i < 15; ++i) {

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

			LL contiNotDe = 0;

			for (int i = 0; i < rts.cnt; ++i) {
				rts[i].rWeight = 1;
			}

			resetConfRts();
			updatePen();

			int deTimeOneTurn = 0;
			int contiTurnNoDe = 0;

			squIter += cfg.yearTabuLen + cfg.yearTabuRand;

			pBestThisTurn = DisInf;

#if DISDOUBLE

			auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

				if (t.deltPen.deltPc == DisInf || t.deltPen.deltPtw == DisInf) {
					return false;
				}

				if (lfeq4(t.deltPen.deltPc + t.deltPen.deltPtw
					, golbalBestM.deltPen.deltPc + golbalBestM.deltPen.deltPtw)) {
					if (getYearOfMove(t) < getYearOfMove(golbalBestM)) {
						golbalBestM = t;
					}
				}
				else if (t.deltPen.deltPc + t.deltPen.deltPtw
					< golbalBestM.deltPen.deltPc + golbalBestM.deltPen.deltPtw) {
					golbalBestM = t;
				}

				//}
				return true;
			};

#else

			auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

				if (t.deltPen.deltPc == DisInf || t.deltPen.deltPtw == DisInf) {
					return false;
				}

				if (t.deltPen.PcOnly + t.deltPen.PtwOnly < 0) {
					if (t.deltPen.PcOnly + t.deltPen.PtwOnly + t.deltPen.deltCost
						< bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly + bestM.deltPen.deltCost) {
						bestM = t;
					}
				}
				
				return true;
			};

#endif // DISDOUBLE

			int iter = 0;
			
			while (penalty > 0 && !lyhTimer.isTimeOut()) {
				
				TwoNodeMove bestM = getMovesRandomly(updateBestM);
				if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
					bestM = evalMinRLMovesFromConfRts(updateBestM);
				}
				++iter;
				if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
					/*debug("penaltyAndRLRepair fail find move")
						debug(++iter)
						debug(++iter)*/
						++contiNotDe;

					return false;
					continue;
				}
				/*else if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly <= 0) {
					;
				}
				else if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {
					;
				}*/

#if CHECKING

				vector<vector<int>> oldRoutes;
				vector<int> oldrv;
				vector<int> oldrw;

				DisType oldpenalty = penalty;
				DisType oldPtw = Ptw;
				DisType oldPc = Pc;
				DisType oldPtwNoWei = PtwNoWei;
				DisType oldPtwOnly = PtwNoWei;
				DisType oldPcOnly = Pc;
				DisType oldRcost = updateRtsCost();

				Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
				Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
				oldrv = rPutCusInve(rv);
				oldrw = rPutCusInve(rw);

				for (int i = 0; i < rts.cnt; ++i) {
					Route& r = rts[i];
					if (rPutCusInve(r).size() != r.rCustCnt) {
						debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(r.routeID)
							debug(r.rCustCnt)
							debug(rPutCusInve(r).size())
							rNextDisp(r);
						rNextDisp(r);

					}

					int pt = r.head;
					while (pt != -1) {
						if (pt >= customers.size()) {
							debug(pt)
								debug(customers.size())
								debug(customers.size())
						}
						pt = customers[pt].next;
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
				updatePen();
				//updatePen(bestM.deltPen);

#if CHECKING

				updateRtsCost();
				updatePen();
				bool iderror = false;
				bool penaltyWeiError =
					!DISlfeq(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty);
				bool penaltyError =
					!DISlfeq(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly, PtwNoWei + Pc);

				bool RoutesCostError = false;
				//!DISlfeq(oldRcost + bestM.deltPen.deltCost, RoutesCost);
				for (int i = 0; i < rts.cnt; ++i) {
					Route& r = rts[i];
					if (rPutCusInve(r).size() != r.rCustCnt) {
						debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(r.routeID)
							debug(r.rCustCnt)
							debug(rPutCusInve(r).size())
							rNextDisp(r);
						rNextDisp(r);

					}

					int pt = r.head;
					while (pt != -1) {
						if (pt >= customers.size()) {
							debug(pt)
								debug(customers.size())
								debug(customers.size())
						}
						pt = customers[pt].next;
					}
				}

				if (iderror || penaltyWeiError || penaltyError || RoutesCostError) {

					debug("naRepair penalty update error!")

						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)

						debug(rv.routeID == rw.routeID)

						debug(iderror)
						debug(penaltyWeiError)
						debug(penaltyError)
						debug(RoutesCostError)


						debug(oldpenalty)

						debug(bestM.deltPen.deltPc)
						debug(bestM.deltPen.deltPtw)
						debug(bestM.deltPen.deltCost)
						debug(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw)
						debug(penalty)
						debug(bestM.deltPen.deltPc + bestM.deltPen.deltPtw)
						debug(penalty - oldpenalty)

						debug(oldRcost)
						debug(bestM.deltPen.deltCost)
						debug(RoutesCost)
						debug(RoutesCost - oldRcost)

						debug(oldPtw)
						debug(bestM.deltPen.deltPtw)
						debug(Ptw)
						debug(Ptw - oldPtw)


						debug(oldPtwNoWei)
						debug(bestM.deltPen.PtwOnly)
						debug(PtwNoWei)

						debug(oldPc)
						debug(bestM.deltPen.deltPc)
						debug(bestM.deltPen.PcOnly)
						debug(Pc)

						cout << "oldrv: ";

					for (auto i : oldrv) {
						cout << i << " ,";
					}
					cout << endl;
					cout << "oldrw: ";
					for (auto i : oldrw) {
						cout << i << " ,";
					}
					cout << endl;

					rNextDisp(rv);
					rNextDisp(rw);

					debug(squIter)

						debug(DEFabs(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty))
						debug("false false")
						debug("false false")

				}
#endif // CHECKING

				//resetConfRts();
				resetConfRtsByOneMove({ rvId,rwId });
				//addWeightToRoute(bestM);

				if (penalty < pBestThisTurn) {
					pBestThisTurn = penalty;
					contiNotDe = 0;
				}
				else {
					++contiNotDe;
				}

				if (contiTurnNoDe > 100) {
					break;
				}
			}


			if (penalty == 0) {
				resetConfRts();
				updatePen();
				return true;
			}
			else {
				return false;
			}
			return true;
		}

		bool repair() {

			gamma = 1;
			return naRepair();
		}

		bool mRLLocalSearch() {

			TwoNodeMove MRLbestM;

			resetConfRts();
			updatePen();
			updateRtsCost();

			auto MRLUpdateM = [&](TwoNodeMove& m) {

				if (m.deltPen.deltPc == 0 && m.deltPen.deltPtw == 0) {

					if (m.deltPen.deltCost < 0 && m.deltPen.deltCost < MRLbestM.deltPen.deltCost) {
						MRLbestM = m;
					}
					/*else if(m.deltPen.deltCost == MRLbestM.deltPen.deltCost){
						if (getYearOfMove(m) < getYearOfMove(MRLbestM)) {
							MRLbestM = m;
						}
					}*/
					
				}
			};

			auto getMRLAllMoves = [&]() {

				MRLbestM.reSet();
				
				for (int v = 1; v <= input.custCnt; ++v) {

					if (customers[v].routeID == -1) {
						continue;
					}
					for (int wpos = 0; wpos < 100; ++wpos) {

						int w = input.allCloseOf[v][wpos];
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
			
			auto getMRLPartMoves = [&]() {

				MRLbestM.reSet();

				for (int v = 1; v <= input.custCnt; ++v) {

					if (customers[v].routeID == -1) {
						continue;
					}

					Vec<int> reV = input.allCloseOf[v];

					for (int wpos = 0; wpos < 20; ++wpos) {
						//int w = myRand.pick(input.custCnt) + 1;
						int w = reV[wpos];
						if (customers[w].routeID == -1) {
							continue;
						}
						for (int i = 0; i < 15; ++i) {

							if (MRLbestM.deltPen.deltCost < 0) {
								return MRLbestM;
							}

							TwoNodeMove m;
							m = TwoNodeMove(v, w, i, estimatevw(i, v, w));
							MRLUpdateM(m);
						}
					}
				}

				return MRLbestM;
			};

			LL contiNotDe = 0;

			bestSol bestS(customers, rts, 
				EPr, Pc, Ptw, PtwNoWei, penalty,RoutesCost);
			
			
			while (!lyhTimer.isTimeOut()) {

				TwoNodeMove bestM = getMRLPartMoves();
				if (bestM.deltPen.PtwOnly > 0
					|| bestM.deltPen.PcOnly > 0
					|| bestM.deltPen.deltCost > 0) {

					break;
				}

#if CHECKING

				vector<vector<int>> oldRoutes;
				vector<int> oldrv;
				vector<int> oldrw;

				DisType oldpenalty = penalty;
				DisType oldPtw = Ptw;
				DisType oldPc = Pc;
				DisType oldPtwNoWei = PtwNoWei;
				DisType oldPtwOnly = PtwNoWei;
				DisType oldPcOnly = Pc;
				DisType oldRcost = updateRtsCost();

				Route& rv = rts.getRouteByRid(customers[bestM.v].routeID);
				Route& rw = rts.getRouteByRid(customers[bestM.w].routeID);
				oldrv = rPutCusInve(rv);
				oldrw = rPutCusInve(rw);

#endif // CHECKING

				updateYearTable(bestM);
				int rvId = customers[bestM.v].routeID;
				int rwId = customers[bestM.w].routeID;

				/*debug(bestM.v)
				debug(bestM.w)
				debug(bestM.kind)*/

				doMoves(bestM);

				/*solTabuTurnSolToBitArr();
				solTabuUpBySolToBitArr();*/

				updatePen();
				updateRtsCost();

#if CHECKING

				bool iderror = false;
				bool penaltyWeiError =
					!DISlfeq(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty);
				bool penaltyError =
					!DISlfeq(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly, PtwNoWei + Pc);

				bool RoutesCostError =
					!DISlfeq(oldRcost + bestM.deltPen.deltCost, RoutesCost);

				bool penAb0 = verify() < 0;

				for (int i = 0; i < rts.cnt; ++i) {
					Route& r = rts[i];
					if (rPutCusInve(r).size() != r.rCustCnt) {
						debug(rPutCusInve(r).size() != r.rCustCnt)
							debug(rPutCusInve(r).size() != r.rCustCnt)
					}
					if (customers[r.head].QX_ != customers[r.tail].Q_X) {
						debug(customers[r.head].QX_ != customers[r.tail].Q_X)
							debug(customers[r.head].QX_ != customers[r.tail].Q_X)
					}

					vector<int> ve = rPutCusInve(r);
					for (int node : ve) {
						if (customers[node].routeID != r.routeID) {
							deOut(node)deOut(r.routeID)debug("rid error")
								deOut(node)deOut(r.routeID)debug("rid error")
								iderror = true;
						}
					}
				}

				if (penAb0 || iderror || penaltyWeiError || penaltyError || RoutesCostError) {

					debug("mRLLocalSearch score update error!")
						
						debug(bestM.v)
						debug(bestM.w)
						debug(bestM.kind)

						debug(rv.routeID == rw.routeID)

						debug(penAb0)
						debug(verify())
						debug(iderror)
						debug(penaltyWeiError)
						debug(penaltyError)
						debug(RoutesCostError)


						debug(oldpenalty)

						debug(bestM.deltPen.deltPc)
						debug(bestM.deltPen.deltPtw)
						debug(bestM.deltPen.deltCost)
						debug(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw)
						debug(penalty)
						debug(bestM.deltPen.deltPc + bestM.deltPen.deltPtw)
						debug(penalty - oldpenalty)

						debug(oldRcost)
						debug(bestM.deltPen.deltCost)
						debug(RoutesCost)
						debug(RoutesCost - oldRcost)

						debug(oldPtw)
						debug(bestM.deltPen.deltPtw)
						debug(Ptw)
						debug(Ptw - oldPtw)


						debug(oldPtwNoWei)
						debug(bestM.deltPen.PtwOnly)
						debug(PtwNoWei)

						debug(oldPc)
						debug(bestM.deltPen.deltPc)
						debug(bestM.deltPen.PcOnly)
						debug(Pc)

						cout << "oldrv: ";

					for (auto i : oldrv) {
						cout << i << " ,";
					}
					cout << endl;
					cout << "oldrw: ";
					for (auto i : oldrw) {
						cout << i << " ,";
					}
					cout << endl;

					rNextDisp(rv);
					rNextDisp(rw);

					debug(squIter)

						debug(DEFabs(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw, penalty))
						debug("false false")
						debug("false false")

				}
#endif // CHECKING

			}

			debug(RoutesCost)

			return true;
		}

		bool minimizeRL() {

			gamma = 1;

			mRLLocalSearch();

			gamma = 0;

			return true;
		}

		bool saveToOutPut() {

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
			lyhTimer.disp();
			output.state = state;

			return true;
		}

		bool saveOutAsSintefFile() {
			
			DateTime d(time(0));
			d.year;
			d.month;
			d.day;
			
			string s = "";
			
			MyString ms;

			s += "Instance name : " + input.example + "\n";
			s += "Authors : <the name of the authors>\n";
			s += "Date : <dd - mm - yy>\n";
			s += "Reference : <reference to publication of method>\n";
			s += "Solution\n";
			for (int i = 0; i < rts.cnt; i++) {
				auto cus = rPutCusInve(rts[i]);
				string r = "Route " + ms.LL_str(i+1) +" :";

				for (auto c : cus) {
					r += " " + ms.LL_str(c);
				}
				r += "\n";
				s += r;
			}

			updateRtsCost();

			std::ofstream rgbData;
			string wrPath = env.outputPath
				+ "sinType" + input.example + "L" + ms.LL_str(RoutesCost) + ".txt";

			rgbData.open(wrPath, std::ios::app | std::ios::out);

			if (!rgbData) {
				Log(Log::Level::Warning) << "output file open errno" << endl;
				return false;
			}

			rgbData << s;

			debug(s);
			rgbData.close();

			return true;
		}
		
		bool testRoute() {

			/*
			* 
			* 
nextdisp: 697 ,13 ,10 ,166 ,202 ,189 ,181 ,340 ,251 ,345 ,472 ,698 ,
P: 1, 7, 7, 4, 1, 4, 5, 13, 1, 7, 7, 1, 4, 10, 4, 10, 7, 4, 7, 10, 10, 7, 2, 4, 1, 1, 1, 1, 7, 7, 4, 10, 1, 10, 7, 1, 10, 1, 7, 1, 1, 1, 10, 1, 10, 1, 1, 4, 1, 1, 7, 7, 13, 7, 1, 10, 1, 7, 4, 4, 7, 10, 4, 10, 1, 4, 1, 7, 4, 7, 1, 7, 7, 1, 1, 7, 7, 7, 1, 4, 1, 1, 1, 1, 10, 1, 10, 7, 1, 4, 9, 1, 1, 1, 2, 1, 7, 4, 10, 4, 1, 7, 1, 4, 7, 1, 1, 10, 1, 7, 4, 10, 4, 1, 1, 4, 10, 7, 1, 16, 4, 1, 4, 4, 1, 1, 1, 7, 7, 1, 7, 7, 1, 4, 1, 4, 1, 4, 1, 10, 4, 1, 4, 10, 1, 1, 4, 7, 10, 7, 7, 4, 10, 1, 7, 7, 1, 7, 7, 1, 4, 1, 1, 4, 1, 1, 7, 7, 1, 1, 4, 1, 4, 10, 4, 7, 4, 1, 4, 1, 1, 10, 4, 7, 7, 1, 1, 4, 4, 7, 7, 10, 7, 7, 1, 4, 4, 4, 1, 4, 1, 7, 7, 7, 4, 10, 10, 4, 4, 4, 10, 13, 4, 7, 1, 1, 1, 7, 1, 4, 1, 4, 1, 4, 7, 1, 4, 1, 4, 1, 7, 1, 7, 13, 2, 1, 1, 7, 4, 7, 1, 7, 2, 4, 7, 1, 4, 4, 7, 4, 4, 4, 4, 7, 4, 1, 7, 4, 7, 13, 4, 13, 7, 1, 4, 1, 4, 10, 4, 1, 1, 4, 1, 1, 1, 4, 16, 1, 7, 1, 1, 1, 7, 4, 1, 1, 1, 4, 4, 1, 4, 10, 4, 1, 1, 1, 1, 16, 4, 1, 4, 7, 1, 7, 1, 1, 10, 1, 10, 4, 7, 1, 4, 4, 1, 10, 1, 1, 1, 1, 4, 7, 10, 3, 4, 1, 1, 4, 4, 4, 4, 4, 1, 1, 4, 1, 9, 4, 7, 4, 4, 1, 1, 4, 10, 4, 1, 7, 7, 4, 4, 1, 7, 4, 10, 10, 4, 10, 4, 1, 13, 4, 7, 10, 4, 1, 1, 7, 1, 1, 10, 7, 1, 1, 10, 4, 10, 4, 4, 10, 10, 10, 4, 10, 13, 5, 5, 1, 1, 4, 1, 7, 4, 7, 1, 1, 1, 1, 2, 7, 1, 1, 1, 1, 1, 16, 4, 1, 10, 1, 1, 1, 10, 4, 10, 1, 10, 7, 7, 1, 7, 10, 1, 1, 1, 4, 4, 4, 1, 10, 1, 10, 1, 1, 4, 1, 1, 4, 1, 1, 13, 4, 7, 1, 4, 4, 4, 1, 1, 1, 1, 4, 4, 4, 1, 1, 10, 4, 10, 10, 4, 1, 1, 9, 7, 11, 1, 1, 7, 7, 1, 7, 1, 1, 7, 7, 1, 4, 4, 10, 7, 4, 4, 1, 1, 1, 1, 10, 1, 7, 10, 4, 10, 4, 7, 1, 1, 9, 1, 4, 13, 4, 1, 1, 1, 4, 4, 1, 4, 7, 7, 1, 4, 1, 1, 4, 4, 4, 13, 4, 1, 4, 7, 7, 7, 1, 4, 4, 1, 4, 4, 13, 7, 10, 4, 4, 4, 7, 10, 4, 7, 1, 10, 1, 1, 7, 10, 10, 1, 1, 4, 10, 4, 1, 10, 1, 7, 7, 1, 10, 10, 1, 4, 4, 10, 4, 4, 2, 7, 4, 1, 4, 13, 4, 1, 7, 4, 7, 1, 7, 1, 1, 4, 1, 1, 1, 10, 10, 4, 4, 4, 4, 12, 7, 13, 10, 10, 4, 4, 1, 4,
nextdisp: 697 ,13 ,10 ,166 ,202 ,189 ,181 ,340 ,251 ,345 ,472 ,698 ,
			*/
			
			vector<int>oldrv = { 13, 76, 308, 338, 393, 55, 38, 34, 56, 289 };
			//vector<int>oldrw = { 120,121 };
			//vector<int>oldrw = { 118,109 };
			P = { 1, 127, 58, 133, 31, 103, 136, 145, 7, 136, 115, 11, 67, 116, 40, 37, 136, 67, 58, 124, 127, 133, 70, 77, 10, 19, 46, 25, 124, 142, 103, 121, 10, 64, 40, 58, 91, 28, 82, 52, 23, 19, 142, 13, 142, 112, 7, 97, 52, 61, 136, 142, 133, 91, 13, 67, 19, 142, 103, 43, 148, 109, 16, 109, 10, 106, 13, 28, 100, 145, 10, 91, 61, 55, 37, 133, 134, 28, 16, 43, 7, 10, 40, 13, 94, 10, 124, 55, 100, 64, 124, 52, 40, 22, 58, 61, 70, 28, 118, 139, 13, 70, 10, 124, 115, 13, 61, 118, 7, 121, 16, 145, 46, 11, 10, 43, 124, 100, 16, 136, 73, 10, 46, 4, 13, 34, 22, 94, 94, 13, 73, 85, 16, 31, 7, 40, 91, 73, 46, 106, 139, 13, 124, 82, 70, 4, 19, 139, 124, 97, 145, 40, 130, 10, 139, 109, 13, 136, 139, 10, 64, 4, 49, 19, 67, 52, 112, 115, 10, 64, 106, 7, 112, 130, 133, 97, 85, 16, 46, 7, 10, 100, 7, 109, 136, 49, 13, 10, 19, 136, 100, 133, 103, 127, 7, 25, 121, 37, 10, 52, 4, 43, 109, 109, 136, 142, 127, 37, 55, 55, 130, 94, 112, 100, 55, 10, 7, 91, 16, 127, 43, 61, 58, 13, 136, 7, 103, 55, 82, 13, 130, 10, 121, 133, 13, 16, 49, 97, 25, 85, 10, 82, 13, 130, 82, 52, 121, 97, 91, 19, 142, 16, 22, 106, 136, 19, 136, 85, 139, 109, 49, 103, 121, 70, 121, 10, 10, 124, 139, 7, 16, 19, 40, 55, 13, 49, 130, 4, 133, 10, 4, 7, 135, 49, 13, 100, 61, 115, 106, 16, 73, 130, 136, 40, 19, 7, 7, 247, 43, 46, 103, 67, 25, 136, 10, 19, 136, 19, 110, 115, 136, 10, 46, 13, 10, 132, 16, 13, 10, 19, 88, 106, 124, 10, 112, 7, 16, 121, 28, 19, 91, 85, 67, 49, 16, 4, 124, 70, 113, 7, 46, 43, 43, 67, 142, 16, 97, 97, 91, 133, 97, 79, 139, 139, 83, 127, 10, 136, 139, 40, 124, 139, 73, 115, 22, 13, 16, 145, 7, 1, 103, 139, 13, 46, 109, 22, 118, 25, 79, 127, 133, 103, 19, 97, 127, 70, 70, 7, 46, 16, 22, 127, 91, 88, 28, 40, 10, 7, 10, 136, 79, 4, 127, 4, 10, 118, 52, 7, 118, 10, 7, 16, 97, 97, 133, 37, 94, 97, 67, 13, 145, 100, 10, 10, 10, 88, 130, 133, 16, 82, 7, 118, 10, 4, 139, 25, 10, 25, 67, 52, 142, 82, 97, 10, 139, 22, 91, 10, 85, 70, 13, 10, 40, 58, 46, 4, 121, 118, 127, 133, 22, 136, 58, 109, 40, 148, 31, 4, 82, 145, 103, 43, 7, 22, 91, 136, 10, 67, 46, 142, 94, 46, 139, 43, 16, 46, 7, 133, 10, 106, 106, 10, 124, 127, 43, 34, 7, 130, 13, 13, 121, 124, 10, 10, 13, 25, 31, 28, 91, 118, 139, 13, 112, 73, 13, 7, 22, 77, 118, 112, 10, 100, 91, 133, 121, 11, 139, 94, 10, 22, 22, 145, 142, 109, 82, 34, 139, 103, 124, 58, 130, 67, 130, 22, 10, 133, 133, 100, 10, 13, 52, 127, 10, 7, 136, 22, 46, 70, 16, 88, 124, 13, 79, 49, 115, 136, 76, 13, 91, 139, 13, 97, 130, 136, 55, 124, 25, 142, 4, 94, 103, 13, 91, 13, 7, 10, 121, 133, 130, 136, 100, 58, 124, 109, 130, 121, 124, 34, 139, 40, 142, }; Route rvv = rCreateRoute(0);
			rts.push_back(rvv);
			Route rww = rCreateRoute(1);
			rts.push_back(rww);

			Route& rv = rts.getRouteByRid(0);
			Route& rw = rts.getRouteByRid(1);

			for (auto c : oldrv) {
				rInsAtPosPre(rv, rv.tail, c);
			}
			/*for (auto c : oldrw) {
				rInsAtPosPre(rw, rw.tail, c);
			}*/

			rNextDisp(rv);
			rNextDisp(rw);

			auto v1 = rPutCusInve(rv);
			auto v2 = rPutCusInve(rw);

			debug(v1.size())
			debug(v2.size())

			rUpdateAvfrom(rv,rv.head);
			rUpdateZvfrom(rv,rv.tail);
			
			rUpdateAvfrom(rw,rw.head);
			rUpdateZvfrom(rw,rw.tail);

			debug(rv.rPtw)
			debug(rw.rPtw)

			auto node1 = ejectOneRouteMinPsumGreedy(rv);
			auto node2 = ejectOneRouteOnlyP(rv,2,2);
			debug(node1.ejeVe.size())
			outVe(node1.ejeVe)
			debug(node1.Psum)

			debug(node2.ejeVe.size())
				outVe(node2.ejeVe)
			debug(node2.Psum)

			debug(1)

			/*DeltPen d = _2optOpenvvj(431, 211);
			DisType old = updateRtsCost();
			TwoNodeMove m(431,211,1,d);

			twoOptStar(m);
			updateRtsCost();
			debug(old)

			debug(d.deltCost)
			debug(RoutesCost)
				debug(RoutesCost-old)
			debug(gamma)
			debug(d.deltCost)

			
			rNextDisp(rv);

			rNextDisp(rv);
			rNextDisp(rv);*/

			return true;
		}

		class pos {
		public:
			int id = 0;
			unordered_set<int> s;
			//operator =
		};

		int getBoundByIncompatible() {

			int n = input.custCnt;
			vector<pos> arr(n);
			
			for (int i = 0; i < n; ++i) {
				arr[i].id = i+1;
			}

			for (int i = 1; i <= n; ++i) {
				for (int j = i + 1; j <= n; ++j) {

					auto getptw = [&] (int v,int w){
						DisType av = input.datas[0].READYTIME;
						av += input.disOf[0][v];
						DisType aw = av + input.datas[v].SERVICETIME + input.disOf[v][w];
						DisType an = aw + input.datas[w].SERVICETIME + input.disOf[0][w];
						DisType rPtw = max(0,aw-input.datas[w].DUEDATE);
						rPtw += max(0,an-input.datas[0].DUEDATE);
						return rPtw;
					};
					
					bool s1 = (getptw(i,j) == 0);
					bool s2 = (getptw(j,i) == 0);

					if (s1 || s2) {
						arr[i-1].s.insert(j);
						arr[j-1].s.insert(i);
					}
				}
			}

			int rid = 0;

			for (int i = 0; i < customers.size(); ++i) {
				customers[i].id = i;
			}

			auto cmp = [&](const pos& a, const pos& b) {
				return P[a.id] > P[b.id];
				return input.datas[a.id].DUEDATE < input.datas[b.id].DUEDATE;
				return a.id<b.id;
				return a.s.size() < b.s.size();
			};

			while (true){

				sort(arr.begin(), arr.end(), cmp);

				bool isSucceed = true;
				int cus = -1;

				for (auto it=arr.begin(); it!= arr.end();) {
					cus = it->id;
					Position tPos = findBestPosInSolForInit(cus);
					//debug(tPos.pen)
					if (tPos.pen > 0 ) {
						isSucceed = false;
						it = arr.erase(it);
						break;
					}
					else {
						++it;
					}
				}

				if (isSucceed) {
					debug(arr.size())
					break;
				}

				Route r1 = rCreateRoute(rid++);
				rInsAtPosPre(r1, r1.tail, cus);
				rUpdateAvfrom(r1, r1.head);
				rUpdateZvfrom(r1, r1.tail);
				for (auto& node : arr) {
					auto it = node.s.find(cus);
					if ( it!= node.s.end()) {
						node.s.erase(it);
					}
				}
				rts.push_back(r1);
			} 

			for (int i = 0; i < rts.size(); ++i) {
				Route& r = rts[i];
				rUpdateZvQfrom(r, r.tail);
				rUpdateAQfrom(r, r.head);
				rUpdateRCost(r);
			}
			updatePen();

			return rts.size();
		}

		~Solver() {};

	public:
		Random myRand;
		Timer lyhTimer;
		RandomX myRandX;
	};

};
#endif // !vrptwNew_SOLVER_H
