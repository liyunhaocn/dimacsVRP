
#include "EAX.h"
#include "Problem.h"
#include "Solver.h"


namespace hust{

bool Solver::initEPr() {

	Route& r = EPr;
	EPr = rCreateRoute(-1);

	r.head = input.custCnt + 1;
	r.tail = input.custCnt + 2;

	customers[r.head].next = r.tail;
	customers[r.tail].pre = r.head;

	customers[r.head].routeID = -1;
	customers[r.tail].routeID = -1;

	return true;
}

int Solver::reCusNo(int x) {
	return x <= input.custCnt ? x : 0;
}

Solver::Solver() :
	input(*globalInput),
	lyhTimer(globalCfg->runTimer),
	PtwConfRts(globalInput->custCnt/8),
	PcConfRts(globalInput->custCnt/8),
	rts(globalInput->custCnt/8)
{

	customers = Vec<Customer>(input.custCnt + 1);
	alpha = 1;
	beta = 1;
	gamma = 1;
	//squIter = 1;
	penalty = 0;
	Ptw = 0;
	PtwNoWei = 0;
	Pc = 0;
	//minEPSize = inf;
	RoutesCost = 0;
	initEPr();
}

Solver::Solver(const Solver& s) :
	input(s.input),
	lyhTimer(s.lyhTimer),
	PtwConfRts(s.PtwConfRts),
	PcConfRts(s.PcConfRts),
	rts(s.rts)
{

	//this->output = s.output;
	this->customers = s.customers;
	this->penalty = s.penalty;
	this->Ptw = s.Ptw;
	this->PtwNoWei = s.PtwNoWei;
	this->Pc = s.Pc;
	this->alpha = s.alpha;
	this->beta = s.beta;
	this->gamma = s.gamma;
	this->EPr = s.EPr;
	this->RoutesCost = s.RoutesCost;
}

Solver& Solver::operator = (const Solver& s) {

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

	return *this;
}

// route function
Route Solver::rCreateRoute(int id) {

	Route r(id);
	int index = (rts.cnt + 1) * 2 + input.custCnt + 1;

	while (index + 1 >= customers.size()) {
		customers.push_back(customers[0]);
	}

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

DisType Solver::rUpdatePc(Route& r) {

	int pt = r.head;
	r.rQ = 0;
	while (pt != -1) {
		r.rQ += input.datas[pt].DEMAND;
		pt = customers[pt].next;
	}
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);
	return r.rPc;
}

bool Solver::rReset(Route& r) {

	r.rCustCnt = 0; //û�м���ֿ�
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

bool Solver::rRemoveAllCusInR(Route& r) {

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

bool Solver::rInsAtPos(Route& r, int pos, int node) {

	customers[node].routeID = r.routeID;
	r.rQ += input.datas[node].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);

	int anext = customers[pos].next;
	customers[node].next = anext;
	customers[node].pre = pos;
	customers[pos].next = node;
	customers[anext].pre = node;

	++r.rCustCnt;

	return true;
}

bool Solver::rInsAtPosPre(Route& r, int pos, int node) {

	customers[node].routeID = r.routeID;
	r.rQ += input.datas[node].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);

	int apre = customers[pos].pre;
	customers[node].pre = apre;
	customers[node].next = pos;
	customers[pos].pre = node;
	customers[apre].next = node;
	++r.rCustCnt;

	return true;
}

int Solver::getFrontofTwoCus(int v, int w) {
	//return customers[v].av < customers[w].av ? v : w;

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

bool Solver::rRemoveAtPos(Route& r, int a) {

	if (r.rCustCnt <= 0) {
		return false;
	}

	r.rQ -= input.datas[a].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);

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

void Solver::rPreDisp(Route& r) {

	int pt = r.tail;
	std::cout << "predisp: ";
	while (pt != -1) {
		std::cout << pt << " ";
		std::cout << ",";
		pt = customers[pt].pre;
	}
	std::cout << std::endl;
}

void Solver::rNextDisp(Route& r) {

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

DisType Solver::rUpdateAvfrom(Route& r, int vv) {

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
		customers[pt].TW_X += std::max<DisType>(customers[pt].avp - input.datas[pt].DUEDATE, 0);

		if (customers[pt].avp <= input.datas[pt].DUEDATE) {
			customers[pt].av = std::max<DisType>(customers[pt].avp, input.datas[pt].READYTIME);
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

DisType Solver::rUpdateAQfrom(Route& r, int v) {

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
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);
	return r.rPc;
}

bool Solver::rUpdateAvQfrom(Route& r, int vv) {

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
		customers[pt].TW_X += std::max<DisType>(customers[pt].avp - input.datas[pt].DUEDATE, 0);

		if (customers[pt].avp <= input.datas[pt].DUEDATE) {
			customers[pt].av = std::max<DisType>(customers[pt].avp, input.datas[pt].READYTIME);
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
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);

	return true;
}

DisType Solver::rUpdateZvfrom(Route& r, int vv) {

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

		customers[pt].TWX_ += std::max<DisType>(input.datas[pt].READYTIME - customers[pt].zvp, 0);

		customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
			std::min<DisType>(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;

		ptnext = pt;
		pt = customers[pt].pre;
	}
	r.rPtw = customers[r.head].TWX_;
	return r.rPtw;
}

DisType Solver::rUpdateZQfrom(Route& r, int v) {

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
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);
	return r.rPc;
}

void Solver::rUpdateZvQfrom(Route& r, int vv) {

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

		customers[pt].TWX_ += std::max<DisType>(input.datas[pt].READYTIME - customers[pt].zvp, 0);

		customers[pt].zv = customers[pt].zvp >= input.datas[pt].READYTIME ?
			std::min<DisType>(customers[pt].zvp, input.datas[pt].DUEDATE) : input.datas[pt].READYTIME;

		customers[pt].QX_ = customers[ptnext].QX_ + input.datas[pt].DEMAND;
		ptnext = pt;
		pt = customers[pt].pre;
	}
	r.rPtw = customers[r.head].TWX_;
	r.rQ = customers[r.head].QX_;
	r.rPc = std::max<DisType>(0, r.rQ - input.Q);
}

int Solver::rGetCusCnt(Route& r) {

	int pt = customers[r.head].next;
	int ret = 0;
	while (pt <= input.custCnt) {
		++ret;
		pt = customers[pt].next;
	}
	return ret;
}

void Solver::rReCalRCost(Route& r) {
	int pt = r.head;
	int ptnext = customers[pt].next;
	r.routeCost = 0;
	while (pt != -1 && ptnext != -1) {
		r.routeCost += input.disOf[reCusNo(pt)][reCusNo(ptnext)];
		pt = ptnext;
		ptnext = customers[ptnext].next;
	}
}

Vec<int> Solver::rPutCusInve(Route& r) {

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

bool Solver::rtsCheck() {

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];

		Vec<int> cus1;
		Vec<int> cus2;

		int pt = r.head;
		while (pt != -1) {

			if (customers[pt].routeID != r.routeID) {
				ERROR("customers[pt].routeID != r.routeID", pt, r.routeID);
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
			ERROR("cus1.size() != cus2.size():",cus1.size() != cus2.size());
		}

		for (int i = 0; i < cus1.size(); ++i) {

			if (cus1[i] != cus2[cus1.size() - 1 - i]) {
				ERROR("cus1[i] != cus2[cus1.size() - 1 - i]",cus1[i] != cus2[cus1.size() - 1 - i]);
			}
		}

		if (r.rCustCnt != cus1.size() - 2) {
			ERROR("r.rCustCnt:",r.rCustCnt);
			ERROR("cus1.size():",cus1.size());
			rNextDisp(r);
			rNextDisp(r);
		}

	}
	return true;
}

bool Solver::rReCalCusNumAndSetCusrIdWithHeadrId(Route& r) {

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

bool Solver::reCalRtsCostAndPen() {

	Pc = 0;
	Ptw = 0;
	PtwNoWei = 0;
	RoutesCost = 0;

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		rReCalCusNumAndSetCusrIdWithHeadrId(r);
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rReCalRCost(r);

		PtwNoWei += r.rPtw;
		Ptw += r.rWeight * r.rPtw;
		Pc += r.rPc;
		RoutesCost += r.routeCost;
	}
	penalty = alpha * Ptw + beta * Pc;

	return true;
}

CircleSector Solver::rGetCircleSector(Route& r) {

	CircleSector ret;
	auto ve = rPutCusInve(r);
	ret.initialize(input.datas[ve.front()].polarAngle);
	for (int j = 1; j < ve.size(); ++j) {
		ret.extend(input.datas[ve[j]].polarAngle);
		
		#if CHECKING
		if (!ret.isEnclosed(input.datas[ve[j]].polarAngle)) {
			ERROR(ret.start);
			ERROR(ret.end);
			ERROR(input.datas[ve[j]].polarAngle);
			for (int v : ve) {
				ERROR(input.datas[v].polarAngle);
			}
		}
		#endif // CHECKING
	}
	return ret;
}

void Solver::sumRtsPen() {

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

void Solver::sumRtsCost() {
	RoutesCost = 0;
	for (int i = 0; i < rts.cnt; ++i) {
		RoutesCost += rts[i].routeCost;
	}
}

DisType Solver::updatePen(const DeltPen& delt) {

	Pc += delt.PcOnly;
	Ptw += delt.deltPtw;
	PtwNoWei += delt.PtwOnly;
	penalty = alpha * Ptw + beta * Pc;
	return penalty;
}

void Solver::reCalRtsCostSumCost() {

	RoutesCost = 0;
	for (int i = 0; i < rts.size(); ++i) {
		rReCalRCost(rts[i]);
		RoutesCost += rts[i].routeCost;
	}
}

bool Solver::initBySecOrder() {

	Vec<int>que1(input.custCnt);
	std::iota(que1.begin(), que1.end(), 1);

	auto cmp0 = [&](int x, int y) {
		return input.datas[x].polarAngle < input.datas[y].polarAngle;
	};
	std::sort(que1.begin(), que1.end(), cmp0);

	int rid = 0;

	int indexBeg = myRand->pick(input.custCnt);

	//int indexBeg = 100;
	// TODO[0]:init����+inupt.custCnt-1 �൱�ڷ�����ת��
	int deltstep = input.custCnt - 1;
	if (myRand->pick(2) == 0) {
		deltstep = 1;
	}

	for (int i = 0; i < input.custCnt; ++i) {
		indexBeg += deltstep;
		int tp = que1[indexBeg % input.custCnt];
		
		Position bestP = findBestPosInSolForInit(tp);
		//Position bestP = findBestPosForRuin(tp);

		if (bestP.rIndex != -1 && bestP.pen == 0) {
			rInsAtPos(rts[bestP.rIndex], bestP.pos, tp);
			rUpdateAvfrom(rts[bestP.rIndex], tp);
			rUpdateZvfrom(rts[bestP.rIndex], tp);
		}
		else {
			Route r1 = rCreateRoute(rid++);
			rInsAtPosPre(r1, r1.tail, tp);
			rUpdateAvfrom(r1, tp);
			rUpdateZvfrom(r1, tp);
			rts.push_back(r1);
		}
	}

	for (int i = 0; i < rts.size(); ++i) {
		Route& r = rts[i];
		rUpdateZvQfrom(r, r.tail);
		rUpdateAQfrom(r, r.head);
		rReCalRCost(r);
	}
	sumRtsPen();

	return true;
}

bool Solver::initSortOrder(int kind) {

	Vec<int>que1(input.custCnt);
	std::iota(que1.begin(), que1.end(), 1);

	auto cmp1 = [&](int x, int y) {
		return input.disOf[0][x] > input.disOf[0][y];
	};

	auto cmp2 = [&](int x, int y) {
		return input.datas[x].READYTIME > input.datas[y].READYTIME;
	};

	auto cmp3 = [&](int x, int y) {
		return input.datas[x].DUEDATE < input.datas[y].DUEDATE;
	};

	if (kind == 1) {
		std::sort(que1.begin(), que1.end(), cmp1);
	}
	else if (kind == 2) {
		std::sort(que1.begin(), que1.end(), cmp2);
	}
	else if (kind == 3) {
		std::sort(que1.begin(), que1.end(), cmp3);
	}
	else {
		ERROR("no this kind of initMaxRoute");
	}

	int rid = 0;
	for (int tp : que1) {

		//Position bestP = findBestPosForRuin(tp);
		Position bestP = findBestPosInSolForInit(tp);

		if (bestP.rIndex != -1 && bestP.pen == 0) {
			rInsAtPos(rts[bestP.rIndex], bestP.pos, tp);
			rUpdateAvQfrom(rts[bestP.rIndex], tp);
			rUpdateZvQfrom(rts[bestP.rIndex], tp);
		}
		else {
			Route r1 = rCreateRoute(rid++);
			rInsAtPosPre(r1, r1.tail, tp);
			rUpdateAvQfrom(r1, r1.head);
			rUpdateZvQfrom(r1, r1.tail);
			rts.push_back(r1);
		}
		//saveOutAsSintefFile("k_" + std::to_string(kind));
	}

	for (int i = 0; i < rts.size(); ++i) {
		Route& r = rts[i];
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rReCalRCost(r);
	}
	sumRtsPen();
	//patternAdjustment();
	return true;
}

bool Solver::initMaxRoute() {

	Vec<int>que1(input.custCnt);
	std::iota(que1.begin(), que1.end(), 1);

	//myRand->shuffleVec(que1);

	auto cmp = [&](int x, int y) {
		return input.datas[x].polarAngle < input.datas[y].polarAngle;
	};
	std::sort(que1.begin(), que1.end(), cmp);

	int rid = 0;

	do {

		int tp = -1;
		Position bestP;
		bool isSucceed = false;
		int eaIndex = -1;

		for (int i = 0; i < que1.size(); ++i) {
			int cus = que1[i];
			//Position tPos = findBestPosForRuin(cus);
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

bool Solver::initByArr2(Vec < Vec<int>> arr2) {

	int rid = 0;
	for (auto& arr : arr2) {

		Route r = rCreateRoute(rid++);

		for (int cus : arr) {
			rInsAtPosPre(r, r.tail, (cus));
		}

		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rts.push_back(r);
	}
	reCalRtsCostAndPen();
	//sumRtsPen();
	return true;
}

bool Solver::initBySolFile(std::string bksPath) {

	std::ifstream myfile(bksPath);
	if (!myfile.is_open()) {
		ERROR("globalCfg->inputPath:", globalCfg->inputPath);
		ERROR("fail to open initBySolFile,bksPath:", bksPath);
		return false;
	}

	std::string t;
	int rn = 0;

	Vec<Vec<int>> rArr2;

	while (std::getline(myfile, t)) {
		if (t.find("Route #") != std::string::npos) {
			int mao = t.find(":");
			t = t.substr(mao + 2);

			std::stringstream ss;
			ss << t;

			std::string st;

			Vec<int> rArr;
			while (std::getline(ss, st, ' ')) {
				//debug(st);
				int c = std::stoi(st);
				rArr.push_back(c);
			}
			rArr2.push_back(rArr);
			//INFO("t:",t);
			++rn;
		}
		else if (t.find("Cost ") != std::string::npos) {
			t = t.substr(5);
		}
	}
	initByArr2(rArr2);
	myfile.close();
	return true;
}

bool Solver::initSolution(int kind) {//5��

	if (kind == 0) {
		initBySecOrder();
	}
	else if(kind <= 3){
		initSortOrder(kind);
	}
	else if (kind == 4) {
		initMaxRoute();
	}
	
	else {
		ERROR("no this kind of init");
	}
	#if 0
	else if (kind == 5) {
		initByDimacsBKS();
	}
	else if (kind == 6) {
		initByLKHBKS();
	}
	#endif // 0
	reCalRtsCostAndPen();
	INFO("init penalty:",penalty);
	INFO("rts.size():",rts.size());
	INFO("rtcost:",RoutesCost);

	return true;
}

bool Solver::EPrReset() {

	Route& r = EPr;
	r.rCustCnt = 0; //û�м���ֿ�
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

bool Solver::EPrInsTail(int t) {

	Route& r = EPr;
	rInsAtPosPre(r, r.tail, t);

	return true;
}

bool Solver::EPrInsHead(int t) {

	Route& r = EPr;
	rInsAtPos(r, r.head, t);

	return true;
}

bool Solver::EPrInsAtPos(int pos, int node) {

	Route& r = EPr;
	rInsAtPos(r, pos, node);
	return true;
}

bool Solver::EPpush_back(int v) {

	rInsAtPosPre(EPr, EPr.tail, v);
	return true;
}

int Solver::EPsize() {
	return EPr.rCustCnt;
}

Vec<int> Solver::EPve() {

	Vec<int>ret = rPutCusInve(EPr);
	return ret;
}

bool Solver::EPrRemoveAtPos(int a) {
	rRemoveAtPos(EPr, a);
	return true;
}

bool Solver::EPremoveByVal(int val) {
	rRemoveAtPos(EPr, val);
	return true;
}

int Solver::EPrGetCusByIndex(int index) {

	int pt = EPr.head;
	pt = customers[pt].next;

	while (pt > 0 && pt <= input.custCnt && index > 0) {
		pt = customers[pt].next;
		index--;
	}
	return pt;
}

Solver::DeltPen Solver::estimatevw(int kind, int v, int w, int oneR) {
	// TODO[move]:�鿴�������ı��
	switch (kind) {
	case 0:return _2optOpenvv_(v, w);
	case 1:return _2optOpenvvj(v, w);
	case 2:return outrelocatevToww_(v, w, oneR);
	case 3:return outrelocatevTowwj(v, w, oneR);
	case 4:return inrelocatevv_(v, w, oneR);
	case 5:return inrelocatevvj(v, w, oneR);
	case 6:return exchangevw_(v, w, oneR);
	case 7:return exchangevwj(v, w, oneR);
	case 8:return exchangevw(v, w, oneR);
	case 9:return exchangevvjvjjwwj(v, w, oneR);
	case 10:return exchangevvjvjjw(v, w, oneR);
	case 11:return exchangevvjw(v, w, oneR);
	case 12:return exchangevwwj(v, w, oneR);
	case 13:return outrelocatevvjTowwj(v, w, oneR);
	case 14:return outrelocatevv_Toww_(v, w, oneR);
	case 15: return reversevw(v, w);
	default:
		ERROR("estimate no this kind move");
		break;
	}
	DeltPen bestM;
	return bestM;
}

Solver::DeltPen Solver::_2optOpenvv_(int v, int w) { //0
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

		bestM.deltPc = std::max<DisType>(0, rwQ - input.Q) +
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

	if (bestM.deltPtw == DisInf) {
		INFO(11111);
	}
	return bestM;
}

Solver::DeltPen Solver::_2optOpenvvj(int v, int w) { //1

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

Solver::DeltPen Solver::outrelocatevToww_(int v, int w, int oneR) { //2

	Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);

	DeltPen bestM;

	int v_ = customers[v].pre;
	int vj = customers[v].next;
	int w_ = customers[w].pre;

	if (v_ > input.custCnt && vj > input.custCnt) {
		return bestM;
	}

	if (rw.routeID == rv.routeID) {

		if (oneR == 0) {
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

			int front = getFrontofTwoCus(v, w);
			//int back = (front == v ? w : v);

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
			#if CHECKING
			else {

				//rNextDisp(rv);
				ERROR(front);
				ERROR(v);
				ERROR(w);
				ERROR(globalCfg->seed);
				ERROR(rv.head);
				ERROR(rv.tail);
			}
			#endif // CHECKING

			newv_vjPtw = newvwPtw;

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
		return;
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

Solver::DeltPen Solver::outrelocatevTowwj(int v, int w, int oneR) { //3

	Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);

	DeltPen bestM;

	int v_ = customers[v].pre;
	int vj = customers[v].next;
	int wj = customers[w].next;

	if (v_ > input.custCnt && vj > input.custCnt) {
		return bestM;
	}

	if (rw.routeID == rv.routeID) {

		if (oneR == 0) {
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
				return;
			}


			//int w_ = customers[w].pre;
			////////////////////////
			newv_vjPtw = newvwPtw = 0;

			int front = getFrontofTwoCus(v, w);
			//int back = (front == v ? w : v);

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
		return;
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

//������(twbegin��twend) twbegin��twend�ĸ���ֵ���ǿɿ��ģ��������м�ĵ���Ա仯 twbegin��twend�����ǲֿ� 
DisType Solver::getaRangeOffPtw(int twbegin, int twend) {

	DisType newwvPtw = customers[twbegin].TW_X;
	newwvPtw += customers[twend].TWX_;

	DisType lastav = customers[twbegin].av;
	int ptpre = twbegin;
	int pt = customers[ptpre].next;

	for (; pt != -1;) {
		DisType avp = lastav + input.datas[ptpre].SERVICETIME + input.disOf[reCusNo(ptpre)][reCusNo(pt)];
		newwvPtw += std::max<DisType>(0, avp - input.datas[pt].DUEDATE);
		lastav = avp > input.datas[pt].DUEDATE ? input.datas[pt].DUEDATE :
			std::max<DisType>(avp, input.datas[pt].READYTIME);
		if (pt == twend) {
			break;
		}
		ptpre = pt;
		pt = customers[pt].next;
	}

	#if CHECKING
	lyhCheckTrue(pt == twend);
	if (pt != twend) {
		ERROR("pt != twend:", pt != twend);
	}
	#endif // CHECKING


	newwvPtw += std::max<DisType>(0, lastav - customers[twend].zv);
	return newwvPtw;
}

Solver::DeltPen Solver::inrelocatevv_(int v, int w, int oneR) { //4

	Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);

	DeltPen bestM;

	int w_ = customers[w].pre;
	int wj = customers[w].next;
	int v_ = customers[v].pre;

	if (w_ > input.custCnt && wj > input.custCnt) {
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

			int whoIsFront = getFrontofTwoCus(v, w);

			int twbegin = -1;
			int twend = -1;
			//(v-),v 
			if (whoIsFront == v) {
				twbegin = v_;
				twend = wj;
			}
			else {
				twbegin = w_;
				twend = customers[v].next;
				// w ..... (v-) v
			}

			//auto cus = rPutCusInve(rw);

			rRemoveAtPos(rw, w);
			rInsAtPos(rw, v_, w);

			//INFO("twbegin:", twbegin);
			//INFO("twend:", twend);
			
			newwvPtw = getaRangeOffPtw(twbegin, twend);
			//newwvPtw = rUpdateAvfrom(rw, rw.head);

			rRemoveAtPos(rw, w);
			rInsAtPos(rw, w_, w);

			//rUpdateAvfrom(rw, rw.head);

			bestM.PtwOnly = newwvPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
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

Solver::DeltPen Solver::inrelocatevvj(int v, int w, int oneR) { //5

	// insert w to (v,v+)
	Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);

	DeltPen bestM;

	int w_ = customers[w].pre;
	int wj = customers[w].next;
	int vj = customers[v].next;

	if (w_ > input.custCnt && wj > input.custCnt) {
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

		//insert w to v vj
		if (rw.routeID == rv.routeID) {

			int whoIsFront = getFrontofTwoCus(v, w);

			int twbegin = -1;
			int twend = -1;
			//(v-),v 
			if (whoIsFront == v) {

				twbegin = v;
				twend = wj;
			}
			else {
				twbegin = w_;
				twend = vj;
				// w ..... (v-) v
			}

			rRemoveAtPos(rw, w);
			rInsAtPos(rw, v, w);

			newwvPtw = getaRangeOffPtw(twbegin, twend);

			rRemoveAtPos(rw, w);
			rInsAtPos(rw, w_, w);

			bestM.PtwOnly = newwvPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;

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

Solver::DeltPen Solver::exchangevw(int v, int w, int oneR) { // 8

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

	auto getDeltPtw = [&]()->void {

		DisType newwPtw = 0;
		DisType newvPtw = 0;
		DisType vPtw = rv.rPtw * rv.rWeight;
		DisType wPtw = rw.rPtw * rw.rWeight;

		if (rv.routeID == rw.routeID) {

			if (v == w) {
				return;
			}

			if (v_ == w) {

				// (v--)->(v-)->(v)->(v+)
				int v__ = customers[v_].pre;

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
				int w__ = customers[w_].pre;

				#if CHECKING
				lyhCheckTrue(w__ >= 0)
					#endif // CHECKING

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

				int fr = getFrontofTwoCus(v, w);

				// exchangevw
				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, v);

				rInsAtPos(rw, v_, w);
				rInsAtPos(rw, w_, v);

				if (fr == v) {
					newwPtw = newvPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newwPtw = newvPtw = getaRangeOffPtw(w_, vj);
				}

				rRemoveAtPos(rw, w);
				rRemoveAtPos(rw, v);

				rInsAtPos(rw, w_, w);
				rInsAtPos(rw, v_, v);

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
		return;
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

Solver::DeltPen Solver::exchangevw_(int v, int w, int oneR) { // 6

	// exchange v and (w_)
	/*Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);*/

	DeltPen bestM;

	int w_ = customers[w].pre;

	if (w_ > input.custCnt || v == w_) {
		return bestM;
	}
	return exchangevw(v, w_, oneR);

}

Solver::DeltPen Solver::exchangevwj(int v, int w, int oneR) { // 7

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

Solver::DeltPen Solver::exchangevvjw(int v, int w, int oneR) { // 11 2��1

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
				// exchange vvj and (w)

				newvPtw = newwPtw = getaRangeOffPtw(w_, vjj);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, w);

				rInsAtPos(rv, w_, w);
				rInsAtPos(rv, w, v);
				rInsAtPos(rv, v, vj);

			}
			else if (w_ == vj) {

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rw, w);

				// v -> (v+) -> w 

				rInsAtPos(rv, v_, w);
				rInsAtPos(rv, w, v);
				rInsAtPos(rv, v, vj);

				// exchange vvj and (w)
				newvPtw = newwPtw = getaRangeOffPtw(v_, wj);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, w);

				rInsAtPos(rv, v_, v);
				rInsAtPos(rv, v, vj);
				rInsAtPos(rv, vj, w);
			}
			else {

				// exchange vvj and (w)
				int fr = getFrontofTwoCus(v, w);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rw, w);

				rInsAtPos(rv, v_, w);
				rInsAtPos(rv, w_, v);
				rInsAtPos(rv, v, vj);

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, vjj);
				}

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, w);

				rInsAtPos(rv, v_, v);
				rInsAtPos(rv, v, vj);
				rInsAtPos(rv, w_, w);
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

Solver::DeltPen Solver::exchangevwwj(int v, int w, int oneR) { // 12 1��2

	// exchange v and (ww+)
	DeltPen bestM;

	int wj = customers[w].next;

	if (wj > input.custCnt) {
		return bestM;
	}
	return exchangevvjw(w, v, oneR);

}

Solver::DeltPen Solver::exchangevvjvjjwwj(int v, int w, int oneR) { // 9 3��2

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
			//exchange vvjvjj and (ww + )
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

				newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);

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

			}
			else if (w_ == vjj) {
				//exchange vvjvjj and (ww + )
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

				newvPtw = newwPtw = getaRangeOffPtw(v_, wjj);

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

			}
			else {

				//exchange vvjvjj and (ww + )
				int fr = getFrontofTwoCus(v, w);

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

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wjj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);
				}

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

Solver::DeltPen Solver::exchangevvjvjjw(int v, int w, int oneR) { // 10 ����һ

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

		if (oneR == 0) {
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

				newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);

				rInsAtPos(rw, w_, w);

				rInsAtPos(rw, w, v);
				rInsAtPos(rw, v, vj);
				rInsAtPos(rw, vj, vjj);

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

				newvPtw = newwPtw = getaRangeOffPtw(v_, wj);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rv, w);


				rInsAtPos(rv, v_, v);
				rInsAtPos(rv, v, vj);
				rInsAtPos(rv, vj, vjj);
				rInsAtPos(rv, vjj, w);

			}
			else {

				int fr = getFrontofTwoCus(v, w);

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rw, w);

				rInsAtPos(rv, v_, w);
				rInsAtPos(rv, w_, v);
				rInsAtPos(rv, v, vj);
				rInsAtPos(rv, vj, vjj);

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);
				}

				rRemoveAtPos(rv, v);
				rRemoveAtPos(rv, vj);
				rRemoveAtPos(rv, vjj);
				rRemoveAtPos(rv, w);

				rInsAtPos(rv, v_, v);
				rInsAtPos(rv, v, vj);
				rInsAtPos(rv, vj, vjj);
				rInsAtPos(rv, w_, w);
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
			delt -= input.disOf[reCusNo(w)][reCusNo(vjj)];
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

Solver::DeltPen Solver::outrelocatevvjTowwj(int v, int w, int oneR) {  //13 ������

	DeltPen bestM;

	Route& rv = rts.getRouteByRid(customers[v].routeID);
	Route& rw = rts.getRouteByRid(customers[w].routeID);

	int v_ = customers[v].pre;
	int vj = customers[v].next;

	if (vj > input.custCnt || vj == w) {
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
				return;
			}

			int fr = getFrontofTwoCus(v, w);

			rRemoveAtPos(rv, v);
			rRemoveAtPos(rv, vj);

			rInsAtPos(rv, w, v);
			rInsAtPos(rv, v, vj);

			if (fr == v) {
				newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
			}
			else {
				newvPtw = newwPtw = getaRangeOffPtw(w, vjj);
			}

			rRemoveAtPos(rv, v);
			rRemoveAtPos(rv, vj);

			rInsAtPos(rv, v_, v);
			rInsAtPos(rv, v, vj);

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
		return;
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

Solver::DeltPen Solver::outrelocatevv_Toww_(int v, int w, int oneR) {  //14 ���������

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
	return outrelocatevvjTowwj(v_, w_, oneR);

}

Solver::DeltPen Solver::reversevw(int v, int w) {//15 ��ת

	DeltPen bestM;

	int vId = customers[v].routeID;
	int wId = customers[w].routeID;

	if (vId > input.custCnt || wId > input.custCnt || vId != wId || v == w) {
		return bestM;
	}

	Route& r = rts.getRouteByRid(customers[v].routeID);

	int front = getFrontofTwoCus(v, w);
	int back = (front == v ? w : v);

	int f_ = customers[front].pre;
	int bj = customers[back].next;

	auto getDeltPtw = [&]()->void {

		int fj = customers[front].next;
		if (fj == back) {
			bestM = exchangevw(v, w, 1);
			return;
		}
		int fjj = customers[fj].next;
		if (fjj == back) {
			bestM = exchangevw(v, w, 1);
			return;
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
		return;
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

Solver::DeltPen Solver::_Nopt(Vec<int>& nodes) { //16 Nopt*

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

bool Solver::doMoves(TwoNodeMove& M) {

	switch (M.kind) {

	case 0:
	case 1:
		twoOptStar(M); break;
	case 2:
	case 3:
	case 13:
	case 14:
		outRelocate(M); break;
	case 4:
	case 5:
		inRelocate(M); break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		exchange(M); break;
	case 15:
		doReverse(M); break;
	default:
		ERROR("doNopt(M) error");
		return false;
		break;
	}
	return false;
}

bool Solver::twoOptStar(TwoNodeMove& M) {

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
		ERROR("no this two opt * move!");
		return false;
	}
	return false;
}

bool Solver::outRelocate(TwoNodeMove& M) {

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
		ERROR("no this inrelocate move");
		return false;
	}
	return true;

	return true;
}

bool Solver::inRelocate(TwoNodeMove& M) {

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

bool Solver::exchange(TwoNodeMove& M) {

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

		//exchangevvjvjjwwj 3��2
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

		//exchangevvjvjjw 3��1
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
		//exchangevvjw 2��1
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
		ERROR("no this inrelocate move");
		return false;
	}
	return true;
}

bool Solver::doReverse(TwoNodeMove& M) {

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

bool Solver::updateYearTable(TwoNodeMove& t) {

	int v = t.v;
	int w = t.w;

	if (t.kind == 0) {
		//_2optOpenvv_
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 1) {
		//_2optOpenvvj
		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 2) {
		// outrelocatevToww_
		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 3) {
		// outrelocatevTowwj
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 4) {

		// inrelocatevv_
		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 5) {

		// inrelocatevvj
		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

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

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}
			else {

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w__][w_] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}

		}
		else {

			(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[w__][w_] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

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

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}
			else {

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[wj][wjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}

		}
		else {

			(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[wj][wjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

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

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}
			else if (w_ == v) {

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}
			else {

				(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
				(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

			}

		}
		else {

			(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
			(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

		}

	}
	else if (t.kind == 9) {

		//exchangevvjvjjwwj(v, w); ������ v v+ v++ | w w+

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[vjj][v3j] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[wj][wjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);


	}
	else if (t.kind == 10) {

		//exchangevvjvjjw(v, w); ����һ v v+ v++ | w

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input.custCnt ? 0 : customers[vjj].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[vjj][v3j] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 11) {
		//exchangevvjw(v, w); ����һ v v +  | w
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[vj][vjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 12) {

		//exchangevwwj(v, w); һ���� v  | w w+

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[wj][wjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 13) {

		//outrelocatevvjTowwj(v, w); ������ v v+  | w w+
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;

		//int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		//int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[vj][vjj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 14) {
		// v- v | w_ w
		//outrelocatevv_Toww_  | w-  v- v w

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int v__ = customers[v_].pre > input.custCnt ? 0 : customers[v_].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;

		(*yearTable)[v__][v_] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[v][vj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w_][w] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

	}
	else if (t.kind == 15) {

		//reverse [v,w]
		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;


		(*yearTable)[v_][v] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
		(*yearTable)[w][wj] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

		int fr = getFrontofTwoCus(v, w);
		if (fr == w) {
			std::swap(v, w);
		}

		int pt = v;
		int ptn = customers[pt].next;
		while (pt != w) {
			//debug(pt);
			(*yearTable)[pt][ptn] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);

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
		ERROR("sol tabu dont include this move");
	}


	return true;
}

Vec<int> Solver::getPtwNodes(Route& r, int ptwKind) {

	Vec<int> ptwNodes;
	ptwNodes.reserve(r.rCustCnt);

	#if CHECKING
	lyhCheckTrue(r.rPtw > 0);
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

		int v = startNode;
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
		#if CHECKING
		if (v != endNode) {
			ERROR(v != endNode);
		}
		#endif // CHECKING

	};

	auto getPtwNodesByLastPtw = [&]() {

		int startNode = customers[r.tail].pre;
		int endNode = customers[r.head].next;

		int pt = customers[r.head].next;
		while (pt != -1) {
			if (customers[pt].avp > input.datas[pt].DUEDATE) {
				endNode = pt;
			}
			pt = customers[pt].next;
		}
		if (endNode > input.custCnt) {
			endNode = customers[r.tail].pre;
		}

		//pt = customers[r.tail].pre;
		//while (pt != -1) {
		//	if (customers[pt].zvp < input.datas[pt].READYTIME) {
		//		startNode = pt;
		//	}
		//	pt = customers[pt].pre;
		//}
		//if (startNode > input.custCnt ) {
		//	startNode = customers[r.head].next;
		//}
		startNode = customers[r.head].next;

		pt = startNode;
		while (pt <= input.custCnt) {
			ptwNodes.push_back(pt);
			if (pt == endNode) {
				break;
			}
			pt = customers[pt].next;
		}
		//TODO[7]:�ǵ�ע�͵������
		//if (pt != endNode) {
		//	debug(pt);
		//}
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
		// TODO[1]: һ�����Լ��� �Ӻ���ǰ��last ��ǰ�����first
		getPtwNodesByLastPtw();
	}

	#if CHECKING
	lyhCheckTrue(ptwNodes.size() > 0);
	#endif // CHECKING

	return ptwNodes;
}

LL Solver::getYearOfMove(TwoNodeMove& t) {

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

		//exchangevvjvjjwwj(v, w); ������ v v+ v++ | w w+

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

		//exchangevvjvjjw(v, w); ����һ v v + v++ | w

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
		//exchangevvjw(v, w); ����һ v v +  | w

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;
		int vjj = customers[vj].next > input.custCnt ? 0 : customers[vj].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;

		sumYear = ((*yearTable)[v][w_] + (*yearTable)[vj][wj]
			+ (*yearTable)[v_][w] + (*yearTable)[w][vjj]) / 4;
	}
	else if (t.kind == 12) {

		//exchangevwwj(v, w); һ���� v  | w w+

		int v_ = customers[v].pre > input.custCnt ? 0 : customers[v].pre;
		int vj = customers[v].next > input.custCnt ? 0 : customers[v].next;

		int w_ = customers[w].pre > input.custCnt ? 0 : customers[w].pre;
		int wj = customers[w].next > input.custCnt ? 0 : customers[w].next;
		int wjj = customers[wj].next > input.custCnt ? 0 : customers[wj].next;

		sumYear = ((*yearTable)[w_][v] + (*yearTable)[v][wjj]
			+ (*yearTable)[v_][w] + (*yearTable)[wj][vj]) / 4;

	}
	else if (t.kind == 13) {

		//outrelocatevvjTowwj(v, w); ������ v v+  | w w+

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
		ERROR("get year of none");
	}
	return sumYear;
}

Solver::TwoNodeMove Solver::getMovesRandomly
(std::function<bool(Solver::TwoNodeMove& t, Solver::TwoNodeMove& bestM)>updateBestM) {

	TwoNodeMove bestM;

	auto _2optEffectively = [&](int v) {

		int vid = customers[v].routeID;

		int v_ = customers[v].pre;
		if (v_ > input.custCnt) {
			v_ = 0;
		}

		double broaden = globalCfg->broaden;

		int v_pos = input.addSTJIsxthcloseOf[v][v_];
		//int v_pos = input.jIsxthcloseOf[v][v_];
		if (v_pos == 0) {
			v_pos += globalCfg->broadenWhenPos_0;
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

			//if (customers[w].av + input.datas[w].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)] >= customers[v].avp) {
			//	continue;
			//}

			TwoNodeMove m0(v, w, 0, _2optOpenvv_(v, w));
			updateBestM(m0, bestM);

		}

		int vj = customers[v].next;
		if (vj > input.custCnt) {
			vj = 0;
		}
		int vjpos = input.addSTJIsxthcloseOf[v][vj];
		//int vjpos = input.jIsxthcloseOf[v][vj];
		if (vjpos == 0) {
			vjpos += globalCfg->broadenWhenPos_0;
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

			//if (customers[w].zv - input.disOf[reCusNo(v)][reCusNo(w)] - input.datas[v].SERVICETIME <= customers[v].zvp) {
			//	continue;
			//}

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

		double broaden = globalCfg->broaden;

		//int devided = 7;
		int devided = 4;

		if (vpos1 == 0) {
			vpos1 += globalCfg->broadenWhenPos_0;
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
				int vrId = customers[v].routeID;
				int wrId = customers[w].routeID;
				if (customers[w].routeID == -1 || wrId == vrId) {
					continue;
				}
				TwoNodeMove m8(v, w, 8, exchangevw(v, w, 0));
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
			vpos2 += globalCfg->broadenWhenPos_0;
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
				int vrId = customers[v].routeID;
				int wrId = customers[w].routeID;
				if (customers[w].routeID == -1 || wrId == vrId) {
					continue;
				}

				TwoNodeMove m8(v, w, 8, exchangevw(v, w, 0));
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

		int devided = 5;
		Vec<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[v];

		int N = relatedToV.size();
		int m = std::max<DisType>(1, N / devided);

		Vec<int>& ve = myRandX->getMN(N, m);
		for (int i = 0; i < m; ++i) {
			int wpos = ve[i];
			int w = relatedToV[wpos];

			int wrId = customers[w].routeID;
			if (wrId == -1) {
				continue;
			}

			if (wrId != customers[v].routeID) {

				TwoNodeMove m2(v, w, 2, outrelocatevToww_(v, w, 0));
				updateBestM(m2, bestM);
				/*TwoNodeMove m14(v, w, 14, outrelocatevv_Toww_(v, w));
				updateBestM(m14);*/
				TwoNodeMove m3(v, w, 3, outrelocatevTowwj(v, w, 0));
				updateBestM(m3, bestM);
				/*TwoNodeMove m13(v, w, 13, outrelocatevvjTowwj(v, w));
				updateBestM(m13,bestM);*/
			}
		}
	};

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
		ERROR("error on conf route");
	}

	Route& r = rts.getRouteByRid(rId);

	#if CHECKING
	lyhCheckTrue(r.rPc > 0 || r.rPtw > 0);
	#endif // CHECKING

	if (r.rPtw > 0) {

		int pt = r.tail;
		while (pt != -1) {
			if (customers[pt].avp > input.datas[pt].DUEDATE) {
				break;
			}
			pt = customers[pt].pre;
		}
		
		#if CHECKING
		if (pt == r.head) {
			ERROR("NO Ptw In R");
		}
		#endif // CHECKING

		int endNode = -1;
		if (pt == r.tail) {
			endNode = pt;
		}
		else {
			endNode = customers[pt].next;
		}
		
		//auto arr = rPutCusInve(r);
		//for (int v : arr ) {
		for (int v = customers[r.head].next; v != endNode; v = customers[v].next) {

			int v_ = customers[v].pre;
			int vj = customers[v].next;
			int maxL = std::max<int>(5, r.rCustCnt / 5);

			if (customers[v_].TW_X + customers[vj].TWX_ == r.rPtw) {
				
				int wbegin = v;
				int preStep = myRand->pick(1, maxL + 1);

				for (int i = 0; i < preStep; ++i) {
					wbegin = customers[wbegin].pre;
					if (wbegin > input.custCnt) {
						break;
					}
				}
				wbegin = customers[wbegin].next;

				for (int i = 0; i < maxL; ++i) {
					wbegin = customers[wbegin].next;
					if (wbegin > input.custCnt) {
						break;
					}
					if (wbegin != v) {

						TwoNodeMove m4(v, wbegin, 4, inrelocatevv_(v, wbegin, 1));
						updateBestM(m4, bestM);

						TwoNodeMove m8(v, wbegin, 8, exchangevw(v, wbegin, 1));
						updateBestM(m8, bestM);

						TwoNodeMove m13(v, wbegin, 13, outrelocatevvjTowwj(v, wbegin, 1));
						updateBestM(m13, bestM);

						TwoNodeMove m15(v, wbegin, 15, reversevw(v, wbegin));
						updateBestM(m15, bestM);
					}
				}
			}
			
			_2optEffectively(v);
			exchangevwEffectively(v);
			outrelocateEffectively(v);

		}
	}
	else {

		auto rArr = rPutCusInve(r);
		for (int v : rArr) {

			Vec<int>& relatedToV = input.iInNeicloseOfUnionNeiCloseOfI[v];
			int N = relatedToV.size();
			int m = N / 7;
			//int m = N;
			m = std::max<int>(1, m);

			Vec<int>& ve = myRandX->getMN(N, m);
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				int w = relatedToV[wpos];
				int vrId = customers[v].routeID;
				int wrId = customers[w].routeID;

				if (customers[w].routeID == -1 || wrId == vrId) {
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

				//TwoNodeMove m4(v, w, 4, inrelocatevv_(v, w));
				//updateBestM(m4,bestM);
				//TwoNodeMove m5(v, w, 5, inrelocatevvj(v, w));
				//updateBestM(m5,bestM);

				TwoNodeMove m6(v, w, 6, exchangevw_(v, w, 0));
				updateBestM(m6, bestM);
				TwoNodeMove m7(v, w, 7, exchangevwj(v, w, 0));
				updateBestM(m7, bestM);

				TwoNodeMove m8(v, w, 8, exchangevw(v, w, 0));
				updateBestM(m8, bestM);

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

	return bestM;

}

bool Solver::resetConfRts() {

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

bool Solver::resetConfRtsByOneMove(Vec<int> ids) {

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

bool Solver::doEject(Vec<eOneRNode>& XSet) {

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
		lyhCheckTrue(en.ejeVe.size() > 0);
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
	lyhCheckTrue(penalty == 0);
	lyhCheckTrue(PtwConfRts.cnt == 0);
	lyhCheckTrue(PcConfRts.cnt == 0);
	#endif // CHECKING

	return true;
}

bool Solver::managerCusMem(Vec<int>& releaseNodes) {

	//printve(releaseNodes);
	int useEnd = input.custCnt + 2 + (rts.cnt + 1) * 2 + 1;

	for (int i : releaseNodes) {

		for (int j = useEnd - 1; j > i; --j) {

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

bool Solver::removeOneRouteByRid(int rId) {

	if (rId == -1) {
		int index = myRand->pick(rts.size());
		rId = rts[index].routeID;
	}

	Route& rt = rts.getRouteByRid(rId);

	Vec<int> rtVe = rPutCusInve(rt);
	Vec<int> releasedNodes = { rt.head,rt.tail };
	rReset(rt);
	rts.removeIndex(rts.posOf[rId]);
	
	for (int pt : rtVe) {
		EPpush_back(pt);
	}

	sumRtsPen();
	resetConfRts();
	managerCusMem(releasedNodes);

	return true;
}

DisType Solver::verify() {

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

Solver::DeltPen Solver::getDeltIfRemoveOneNode(Route& r, int pt) {

	int pre = customers[pt].pre;
	int next = customers[pt].next;

	DeltPen d;

	DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(next)];
	d.PtwOnly = std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[pre].TW_X;
	d.PtwOnly = d.PtwOnly - r.rPtw;
	d.PcOnly = std::max<DisType>(0, r.rQ - input.datas[pt].DEMAND - input.Q);
	d.PcOnly = d.PcOnly - r.rPc;
	return d;

};

bool Solver::addWeightToRoute(TwoNodeMove& bestM) {

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
			//		(*yearTable)[c][cnext] = squIter + globalCfg->yearTabuLen;
			//		(*yearTable)[cpre][c] = squIter + globalCfg->yearTabuLen;
			//	}
			//}

			r.rWeight += globalCfg->weightUpStep;
			Ptw += r.rPtw;
		}
		penalty = alpha * Ptw + beta * Pc;


	}
	return true;
};

bool Solver::squeeze() {

	List<Solver> bestPool;
	bestPool.push_back(*this);

	Solver sclone = *this;

	auto updateBestPool = [&](DisType Pc, DisType PtwNoWei) {

		bool isUpdate = false;
		bool dominate = false;

		auto it = bestPool.begin();
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

			bestPool.push_back(*this);
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

	auto getMinPsumSolIter = [&]() -> List<Solver>::iterator {

		int min1 = IntInf;
		List<Solver>::iterator retIt;

		for (auto it = bestPool.begin(); it != bestPool.end(); ++it) {
			customers = it->customers;
			rts = it->rts;
			resetConfRts();
			sumRtsPen();

			Vec<eOneRNode> reteNode = ejectFromPatialSol();

			int bestCnt = 1;
			int sum = 0;
			//int ejeNum = 0;

			for (eOneRNode& en : reteNode) {
				sum += en.Psum;
				//ejeNum += en.ejeVe.size();
				/*for (int c : en.ejeVe) {
					sum = std::max<DisType>(sum,P[c]);
				}*/
			}

			if (sum < min1) {
				bestCnt = 1;
				min1 = sum;
				retIt = it;
				ejeNodesAfterSqueeze = reteNode;
			}
			else if (sum == min1) {
				++bestCnt;
				if (myRand->pick(bestCnt) == 0) {
					retIt = it;
					ejeNodesAfterSqueeze = reteNode;
				}
			}
		}

		return retIt;
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

	squIter += globalCfg->yearTabuLen + globalCfg->yearTabuRand;

	globalCfg->squContiIter = globalCfg->squMinContiIter;
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
			if (contiNotDe == globalCfg->squContiIter) {
				break;
			}
			INFO("squeeze fail find move");
			INFO("squIter", squIter);
			++contiNotDe;
			continue;
		}

		#if CHECKING
		if (bestM.deltPen.PcOnly == DisInf || bestM.deltPen.PtwOnly == DisInf) {
			ERROR("squeeze fail find move");
			ERROR("squIter",squIter);
			++contiNotDe;
			continue;
		}
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

		lyhCheckTrue(penaltyaferup == penalty)
			bool penaltyWeiError =
			!(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty);
		bool penaltyError =
			!(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly == PtwNoWei + Pc);

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

		if (penaltyWeiError || penaltyError) {

			ERROR("squeeze penalty update error!");

			ERROR("bestM.v:",bestM.v);
			ERROR("bestM.w:",bestM.w);
			ERROR("bestM.kind:",bestM.kind);
			ERROR("penaltyWeiError:",penaltyWeiError);
			ERROR("penaltyError:", penaltyError);

			ERROR("penaltyaferup:",penaltyaferup);
			ERROR("penalty:",penalty);
			ERROR("Ptw:",Ptw);
			ERROR("Pc:",Pc);

			ERROR("bestM.deltPen.deltPtw:",bestM.deltPen.deltPtw);
			ERROR("oldPtw:", oldPtw);
			ERROR("oldPc:", oldPc);

			ERROR("rv.routeID == rw.routeID:",rv.routeID == rw.routeID);

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

		if (contiNotDe == globalCfg->squContiIter) {

			if (penalty < 1.1 * pBestThisTurn && globalCfg->squContiIter < globalCfg->squMaxContiIter) {
				globalCfg->squContiIter += globalCfg->squIterStepUp;
				globalCfg->squContiIter = std::min<int>(globalCfg->squMaxContiIter, globalCfg->squContiIter);
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

		bestPool.push_back(sclone);
		//debug(bestPool.size());
		auto minIter = getMinPsumSolIter();
		//debug(index)
		*this = *minIter;

		#if CHECKING
		DisType oldp = penalty;
		sumRtsPen();
		lyhCheckTrue(oldp == penalty);
		#endif // CHECKING		

		return false;
	}
	return true;
}

Solver::Position Solver::findBestPosInSol(int w) {

	Vec<Position> posPool = { Position(),Position(),Position(),Position(), };

	// �ͷ�����������ǰ��
	auto cmp = [&](const Position a, const Position& b) {
		if (a.pen == b.pen) {
			return  a.year < b.year;
		}
		else {
			return a.pen < b.pen;
		}
	};

	std::priority_queue<Position, Vec<Position>, decltype(cmp)> qu(cmp);
	qu.push(Position());

	auto updatePool = [&](Position& pos) {

		if (pos.pen <= qu.top().pen) {
			qu.push(pos);
			if (qu.size() < globalCfg->findBestPosInSolPqSize) {
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
				- input.disOf[reCusNo(vj)][reCusNo(v)];

			int vre = v > input.custCnt ? 0 : v;
			int vjre = vj > input.custCnt ? 0 : vj;

			int year = (*yearTable)[vre][w] + (*yearTable)[w][vjre];

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

	int p0cnt = 1;
	int p1cnt = 1;
	int p2cnt = 1;
	int p3cnt = 1;
	//auto q = qu;

	while (!qu.empty()) {

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
			if (myRand->pick(p0cnt) == 0) {
				posPool[0] = pos;
			}
		}

		if (pos.year < posPool[1].year) {
			p1cnt = 1;
			posPool[1] = pos;
		}
		else if (pos.year == posPool[1].year) {
			++p1cnt;
			if (myRand->pick(p1cnt) == 0) {
				posPool[1] = pos;
			}
		}

		if (pos.cost < posPool[2].cost) {
			p2cnt = 1;
			posPool[2] = pos;
		}
		else if (pos.cost == posPool[2].cost) {
			++p2cnt;
			if (myRand->pick(p2cnt) == 0) {
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

Solver::Position Solver::findBestPosInSolForInit(int w) {

	//int quMax = globalCfg->initFindPosPqSize;

	//Vec<CircleSector> secs(rts.cnt);
	//for (int i = 0; i < rts.cnt; ++i) {
	//	secs[i] = rGetCircleSector(rts[i]);
	//}

	Position bestPos;

	for (int i = 0; i < rts.size(); ++i) {
		//debug(i)
		Route& rt = rts[i];

		int v = rt.head;
		int vj = customers[v].next;

		DisType rtPc = std::max<DisType>(0, rt.rQ + input.datas[w].DEMAND - input.Q);
		rtPc = rtPc - rt.rPc;

		if (rtPc > bestPos.pen) {
			continue;
		}

		//int secDis = CircleSector::disofpointandsec
		//(input.datas[w].polarAngle, secs[i]);
		//if (secDis > bestPos.secDis) {
		//	continue;
		//}

		while (v != -1 && vj != -1) {

			DisType rtPtw = 0;
			rtPtw += customers[v].TW_X;
			rtPtw += customers[vj].TWX_;

			DisType awp = customers[v].av + input.datas[v].SERVICETIME + input.disOf[reCusNo(v)][reCusNo(w)];
			rtPtw += std::max<DisType>(0, awp - input.datas[w].DUEDATE);
			DisType aw =
				awp <= input.datas[w].DUEDATE ? std::max<DisType>(input.datas[w].READYTIME, awp) : input.datas[w].DUEDATE;

			DisType avjp = aw + input.datas[w].SERVICETIME + input.disOf[reCusNo(w)][reCusNo(vj)];
			rtPtw += std::max<DisType>(0, avjp - input.datas[vj].DUEDATE);
			DisType avj =
				avjp <= input.datas[vj].DUEDATE ? std::max<DisType>(input.datas[vj].READYTIME, avjp) : input.datas[vj].DUEDATE;
			rtPtw += std::max<DisType>(0, avj - customers[vj].zv);

			rtPtw = rtPtw - rt.rPtw;

			DisType cost =
				input.disOf[reCusNo(w)][reCusNo(v)]
				+ input.disOf[reCusNo(w)][reCusNo(vj)];
			    - input.disOf[reCusNo(vj)][reCusNo(v)];

			Position pt;
			pt.cost = cost;
			pt.pen = rtPtw + rtPc;
			pt.pos = v;
			pt.rIndex = i;
			//pt.secDis = secDis;

			if (pt.pen < bestPos.pen) {
				bestPos = pt;
			}
			else {

				if (myRand->pick(100) < globalCfg->initWinkacRate) {
					if (pt.cost < bestPos.cost) {
						bestPos = pt;
					}
				}
				
				//if (pt.secDis < bestPos.secDis) {
				//	bestPos = pt;
				//}
				//else {
				//	if (pt.cost < bestPos.cost) {
				//		bestPos = pt;
				//	}
				//}
			}

			v = vj;
			vj = customers[vj].next;
		}
	}

	return bestPos;
}

Solver::Position Solver::findBestPosForRuin(int w) {

	Position ret;

	// �ͷ�����������ǰ��
	auto updatePool = [&](Position& pos) {

		if (pos.pen < ret.pen) {
			ret = pos;
		}
		else if (pos.cost < ret.cost) {
			// TODO[8]:գ���ʿ��Ե� 5%���ʣ�
			if (myRand->pick(100) < globalCfg->ruinWinkacRate) {
				ret = pos;
			}
		}
	};

	auto& rtsIndexOrder = myRandX->getMN(rts.cnt, rts.cnt);
	sort(rtsIndexOrder.begin(), rtsIndexOrder.end(), [&](int x,int y) {
		return rts[x].rQ < rts[y].rQ;
	});
	//printve(rtsIndexOrder);
	for (int i : rtsIndexOrder) {
	
		Route& r = rts[i];

		DisType oldrPc = rts[i].rPc;
		DisType rPc = std::max<DisType>(0, r.rQ + input.datas[w].DEMAND - input.Q);
		rPc = rPc - oldrPc;

		if (rPc > ret.pen) {
			continue;
		}
		Vec<int> a = rPutCusInve(r);
		a.push_back(r.tail);

		for(int vj:a) {

			int v = customers[vj].pre;

			//if (myRand->pick(2) == 0) {
			//	continue;
			//}

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

			//TODO[-1]:����ľ�����㷽ʽ�ı���
			DisType cost = input.disOf[reCusNo(w)][reCusNo(v)]
				+ input.disOf[reCusNo(w)][reCusNo(vj)]
				//;
				- input.disOf[reCusNo(v)][reCusNo(vj)];
			//int year = (*yearTable)[reCusNo(w)][reCusNo(v)] + (*yearTable)[reCusNo(w)][reCusNo(vj)];
			//year >>= 1;

			Position posTemp;
			posTemp.rIndex = i;
			posTemp.cost = cost;
			posTemp.pen = rPtw + rPc;
			posTemp.pos = v;
			//posTemp.year = year;
			//posTemp.secDis = abs(input.datas[w].polarAngle - input.datas[v].polarAngle);

			updatePool(posTemp);

		}
	}
	return ret;
}

void Solver::ruinClearEP(int kind) {

	// �������ڵ��·�����������֮��ֻ������Щ·����costֵ
	std::unordered_set<int> insRts;
	Vec<int> EPArr = rPutCusInve(EPr);

	auto cmp1 = [&](int a, int b) {
		return input.datas[a].DEMAND > input.datas[b].DEMAND;
	};
	auto cmp2 = [&](int a, int b) {
		return input.datas[a].DUEDATE - input.datas[a].READYTIME
			< input.datas[b].DUEDATE - input.datas[b].READYTIME;
	};

	auto cmp3 = [&](int a, int b) {
		return input.disOf[a][0] < input.disOf[b][0];
	};
	
	auto cmp4 = [&](int a, int b) {
		return input.datas[a].READYTIME > input.datas[b].READYTIME;
	};
	
	auto cmp5 = [&](int a, int b) {
		return input.datas[a].DUEDATE < input.datas[b].DUEDATE;
	};


	switch (kind) {
	case 0:
		myRand->shuffleVec(EPArr);
		break;
	case 1:
		std::sort(EPArr.begin(), EPArr.end(), cmp1);
		break;
	case 2:
		std::sort(EPArr.begin(), EPArr.end(), cmp2);
		break;
	case 3:
		std::sort(EPArr.begin(), EPArr.end(), cmp3);
		break;
	case 4:
		std::sort(EPArr.begin(), EPArr.end(), cmp4);
		break;
	case 5:
		std::sort(EPArr.begin(), EPArr.end(), cmp5);
		break;
	default:
		break;
	}

	//std::sort(EPArr.begin(), EPArr.end(), cmp3);

	for (int i = 0; i < EPArr.size(); ++i) {
		//for (int i = EPArr.size() - 1;i>=0;--i) {
		int pt = EPArr[i];
		EPrRemoveAtPos(pt);
		//++P[pt];
		auto bestFitPos = findBestPosForRuin(pt);
		Route& r = rts[bestFitPos.rIndex];
		insRts.insert(r.routeID);
		rInsAtPos(r, bestFitPos.pos, pt);
		rUpdateAvQfrom(r, pt);
		rUpdateZvQfrom(r, pt);

	}

	sumRtsPen();
	for (auto rId : insRts) {
		Route& r = rts.getRouteByRid(rId);
		rReCalRCost(r);
	}
	sumRtsCost();
	//sumRtsPen();
}

int Solver::ruinGetSplitDepth(int maxDept) {

	if (maxDept < 1) {
		return -1;
	}

	static Vec<int> a = { 10000 };
	static Vec<int> s = { 10000 };
	if (maxDept >= a.size()) {
		int oldS = a.size();
		a.resize(maxDept + 2);
		s.resize(maxDept + 2);
		for (int i = oldS; i <= maxDept; ++i) {
			a[i] = a[i - 1] - a[i - 1] / 64;
			s[i] = s[i - 1] + a[i];
		}
	}
	int rd = myRand->pick(1, 1000001);
	auto index = lower_bound(s.begin(), s.begin() + maxDept - 1, rd) - s.begin();
	return index + 1;
}

Vec<int> Solver::ruinGetRuinCusBySting(int ruinKmax, int ruinLmax) {

	std::unordered_set<int> uset;

	int ruinK = myRand->pick(1, ruinKmax + 1);
	

	int v = myRand->pick(1, input.custCnt + 1);
	UnorderedSet<int> rIdSet;
	UnorderedSet<int> begCusSet;
	rIdSet.insert(customers[v].routeID);
	begCusSet.insert(v);

	int wpos = 0;
	while (rIdSet.size() < ruinK) {
		int w = input.addSTclose[v][wpos];
		int wrId = customers[w].routeID;
		if (wrId != -1 && rIdSet.count(wrId) == 0) {
			rIdSet.insert(wrId);
			begCusSet.insert(w);
		}
		++wpos;
	}

	#if 0
	auto splitAndmiddle = [&](int beg) {

		Route& r = rts.getRouteByRid(customers[beg].routeID);

		int ruinL = myRand->pick(1, ruinLmax + 1);

		int ruinCusNumInRoute = std::min<int>(r.rCustCnt, ruinL);
		
		int mputBack = 0;
		if (myRand->pick(100) < globalCfg->ruinSplitRate) {
			int maxMCusPutBack = r.rCustCnt - ruinCusNumInRoute;
			if (maxMCusPutBack > 0) {
				mputBack = ruinGetSplitDepth(maxMCusPutBack);
			}
		}

		if (mputBack != 0) {
			ruinCusNumInRoute += mputBack;
		}
		int cusNumAfterbeg = 0;
		int pbeg = beg;
		for (; pbeg <= input.custCnt; pbeg = customers[pbeg].next) {
			++cusNumAfterbeg;
		}
		cusNumAfterbeg -= 1;
		// ��beg��λ�������ǰ��ruinCusNumInRoute-1 ����
		//// ��beg��λ��������ǰ��ruinCusNumInRoute - (cusNumAfterbeg + 1) ����
		int minStepFward = ruinCusNumInRoute - (cusNumAfterbeg + 1);
		
		int cusNumbegorebeg = r.rCustCnt - cusNumAfterbeg - 1;
		minStepFward =std::max<int>(0, minStepFward);

		int preStep = myRand->pick(minStepFward, cusNumbegorebeg + 1);
		
		int pt = beg;
		for (int i = 0; i < preStep; ++i) {
			//int wposMax = myRand->pick(1, avg * 2 + 1);
			if (pt > input.custCnt) {
				break;
			}
			//arr.push_back(pt);
			pt = customers[pt].pre;
		}

		if (pt > input.custCnt) {
			pt = customers[pt].next;
		}

		// ��һ�γ�����(ruinCusNumInRoute - mputBack) / 2
		int firstPartMax = (ruinCusNumInRoute - mputBack) / 2;
		int firstPart = 0;
		for (int i = 0; i < firstPartMax; ++i) {
			//int wposMax = myRand->pick(1, avg * 2 + 1);
			if (pt > input.custCnt) {
				break;
			}
			++firstPart;
			uset.insert(pt);
			pt = customers[pt].next;
		}

		//����mputBack��
		for (int i = 0; i < mputBack; ++i) {
			pt = customers[pt].next;
		}

		int secPart = ruinCusNumInRoute - mputBack - firstPart;

		for (int i = 0; i < secPart; ++i) {
			if (pt > input.custCnt) {
				break;
			}
			uset.insert(pt);
			pt = customers[pt].next;
		}

	};
	#endif

	auto splitAndmiddle = [&](int beg) {
		Route& r = rts.getRouteByRid(customers[beg].routeID);
		auto a = rPutCusInve(r);
		int n = r.rCustCnt;
		int index = std::find(a.begin(), a.end(),beg) - a.begin();

		//ruin m+t �� ��t���Ż���

		int ruinL = myRand->pick(1, ruinLmax + 1);

		int m = std::min<int>(r.rCustCnt, ruinL);

		int t = 0;
		if (myRand->pick(100) < globalCfg->ruinSplitRate) {
			//int maxMCusPutBack = n - m;
			if (n-m > 0) {
				t = ruinGetSplitDepth(n-m);
			}
			//if (n - m > 0) {
			//	t = myRand->pick(1, n - m + 1);
			//}
			
		}
		int s = m + t;

		int strbeglowbound = std::max<int>(0, index - s + 1);
		int strbegupbound = std::min<int>(index,n-s);
		int strbeg = myRand->pick(strbeglowbound, strbegupbound+1);

		int frontStr = myRand->pick(1,m+1);
		int endStr = m - frontStr;

		//Vec<int> farr;
		for (int i = 0; i < frontStr;++i) {
			uset.insert(a[strbeg+i]);
			//farr.push_back(a[strbeg + i]);
		}

		//Vec<int> eArr;
		for (int i = 0; i < endStr; ++i) {
			uset.insert(a[strbeg + t + i]);
			//eArr.push_back(a[strbeg + frontStr + t + i]);
		}

		//INFO("m:",m);
		//INFO("t:",t);
		//printve(a);
		//printve(farr);
		//printve(eArr);
	};

	for (int beg : begCusSet) {
		splitAndmiddle(beg);
	}

	Vec<int> runCus;
	runCus.reserve(uset.size());
	for (int c : uset) {
		runCus.push_back(c);
	}

	return runCus;
}

Vec<int> Solver::ruinGetRuinCusByRound(int ruinCusNum) {

	ruinCusNum = myRand->pick(1, ruinCusNum+1);
	//ruinCusNum = std::min<int>(ruinCusNum,input.custCnt-1);
	int v = myRand->pick(input.custCnt) + 1;

	Vec<int> runCus;
	runCus.reserve(ruinCusNum);
	runCus.push_back(v);

	for (int i = 0; i < ruinCusNum; ++i) {
		int wpos = i;
		int w = input.addSTclose[v][wpos];
		if (customers[w].routeID != -1) {
			runCus.push_back(w);
		}
	}
	return runCus;
}
#if 0
Vec<int> Solver::ruinGetRuinCusBySec(int ruinCusNum) {

	ruinCusNum = myRand->pick(1, ruinCusNum * 2 + 1);

	Vec<CircleSector> secs(rts.cnt);
	for (int i = 0; i < rts.cnt; ++i) {
		secs[i] = rGetCircleSector(rts[i]);
	}

	Vec<int> rOrder(rts.cnt, 0);
	std::iota(rOrder.begin(), rOrder.end(), 0);
	myRand->shuffleVec(rOrder);

	int rti = 0;
	int rtj = 0;

	for (int i = 0; i < rts.cnt; ++i) {
		rti = rOrder[i];
		bool isbreak = false;
		for (int j = i + 1; j < rts.cnt; ++j) {
			rtj = rOrder[j];
			bool is = CircleSector::overlap(secs[rti], secs[rtj]);
			if (is) {
				isbreak = true;
				break;
			}
		}
		if (isbreak) {
			break;
		}
	}

	auto vei = rPutCusInve(rts[rti]);

	Vec<int> cusArr;

	for (int v : vei) {
		int vAngle = input.datas[v].polarAngle;
		if (cusArr.size() < ruinCusNum && secs[rti].isEnclosed(vAngle)
			&& secs[rtj].isEnclosed(vAngle)) {
			cusArr.push_back(v);
		}
	}

	auto vej = rPutCusInve(rts[rtj]);
	for (int v : vej) {
		int vAngle = input.datas[v].polarAngle;
		if (cusArr.size() < ruinCusNum && secs[rti].isEnclosed(vAngle)
			&& secs[rtj].isEnclosed(vAngle)) {
			cusArr.push_back(v);
		}
	}
	return cusArr;
}

#else
Vec<int> Solver::ruinGetRuinCusBySec(int ruinCusNum) {

	ruinCusNum = myRand->pick(1, ruinCusNum+1);

	Vec<CircleSector> secs(rts.cnt);
	for (int i = 0; i < rts.cnt; ++i) {
		secs[i] = rGetCircleSector(rts[i]);
	}

	Vec<int> rOrder(rts.cnt, 0);
	std::iota(rOrder.begin(), rOrder.end(), 0);
	myRand->shuffleVec(rOrder);

	int rti = -1;
	int rtj = -1;

	bool isFind = false;

	for (int i = 0; i < rts.cnt; ++i) {
		rti = rOrder[i];
		for (int j = i + 1; j < rts.cnt; ++j) {
			rtj = rOrder[j];
			bool is = CircleSector::overlap(secs[rti], secs[rtj]);
			if (is) {
				isFind = true;
				break;
			}
		}
		if (isFind) {
			break;
		}
	}

	if (!isFind) {
		return {};
	}

	auto vei = rPutCusInve(rts[rti]);

	Vec<int> cusArr;

	for (int v : vei) {
		int vAngle = input.datas[v].polarAngle;
		if (cusArr.size() < ruinCusNum && secs[rti].isEnclosed(vAngle)
			&& secs[rtj].isEnclosed(vAngle)) {
			cusArr.push_back(v);
		}
	}

	auto vej = rPutCusInve(rts[rtj]);
	for (int v : vej) {
		int vAngle = input.datas[v].polarAngle;
		if (cusArr.size() < ruinCusNum && secs[rti].isEnclosed(vAngle)
			&& secs[rtj].isEnclosed(vAngle)) {
			cusArr.push_back(v);
		}
	}
	return cusArr;
}
#endif // 0

void Solver::perturbBasedejepool(int ruinCusNum) {

	auto clone = *this;

	//auto ruinCus = ruinGetRuinCusBySec(ruinCusNum);
	auto ruinCus = ruinGetRuinCusByRound(ruinCusNum);
	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRid(customers[cus].routeID);
		rIds.insert(r.routeID);
		if (r.rCustCnt > 2) {
			rRemoveAtPos(r, cus);
			EPpush_back(cus);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRid(rid);
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rReCalRCost(r);
	}
	sumRtsCost();
	sumRtsPen();
	bool isej = ejectLocalSearch();
	if (isej) {
		reCalRtsCostAndPen();
	}
	else {
		*this = clone;
	}
	
}

bool Solver::doOneTimeRuinPer(int perturbkind,int ruinCusNum,int clearEPKind) {

	Vec<int> ruinCus;
	if (perturbkind == 0) {
		ruinCus = ruinGetRuinCusByRound(ruinCusNum);
	}
	else if (perturbkind == 1) {
		ruinCus = ruinGetRuinCusBySec(ruinCusNum);
	}
	else if(perturbkind==2){
		
		int avgLen = input.custCnt / rts.cnt;
		int Lmax = std::min<int>(globalCfg->ruinLmax, avgLen);
		int ruinKmax = 4 * ruinCusNum / (1 + Lmax) - 1;
		ruinKmax = std::min<int>(rts.cnt, ruinKmax);
		ruinKmax = std::max<int>(1, ruinKmax);

		ruinCus = ruinGetRuinCusBySting(ruinKmax, Lmax);
	}
	else {
		ERROR("no this kind of ruin");
	}

	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRid(customers[cus].routeID);
		rIds.insert(r.routeID);
		if (r.rCustCnt > 2) {
			rRemoveAtPos(r, cus);
			EPpush_back(cus);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRid(rid);
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rReCalRCost(r);
	}
	sumRtsCost();
	sumRtsPen();
	ruinClearEP(clearEPKind);

	if (penalty == 0) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool Solver::perturbBaseRuin(int perturbkind, int ruinCusNum,int clearEPKind) {

	ruinCusNum = std::min<int>(ruinCusNum,input.custCnt / 2);

	gamma = 1;
	//TODO[4][1]:������ܿ���ȥ�������֮ǰÿһ��·����cost��ά���Ļ�
	//TODO[4][2]:���ǽӵ��Ŷ�����Ͳ�̫����
	reCalRtsCostSumCost();

	Solver pClone = *this;
	Solver penMinSol;
	penMinSol.penalty = DisInf;

	int i = 0;

	bool perSuc = false;
	bool hasPenMinSol = false;

	for (i = 0 ;i < 10 ;++i) {
		bool noPen = doOneTimeRuinPer(perturbkind,ruinCusNum,clearEPKind);
		if (noPen) {
			if (RoutesCost != pClone.RoutesCost) {
				perSuc = true;
				break;
			}
			else {
				;
			}
		}
		else { // has pen
			if (this->penalty < penMinSol.penalty) {
				hasPenMinSol = true;
				penMinSol = *this;
			}
			*this = pClone;
		}
	}
	
	//static int all = 0;
	//static int sam = 0;
	//static int suc = 0;
	//static int pen = 0;
	//static int rep = 0;
	//static int nre = 0;

	//++all;
	//INFO("i:",i,"all:", all, "sam : ", sam, "suc : ", suc, "pen : ", pen, "rep : ", rep, "nre : ", nre,"ruinCusNum : ", ruinCusNum);
	
	if (perSuc) {
		//++suc;
		return true;
	}
	else {
		// TODO[-1]:�޸�������Ҫ��
		//return false;

		if (hasPenMinSol) { //�������гͷ��Ľ�
			//++pen;
			
			if (penMinSol.repair()) {
				//++rep;
				*this = penMinSol;
				return true;
			}
			else {
				//++nre;
				*this = pClone;
				return false;
			}

		}
		else {// ������10����ͬ�Ľ�
			//++sam;
			return false;
		}
	} 
	return false;
}

//TODO[-1]:���ܰѽ���
int Solver::ruinLocalSearchNotNewR(int ruinCusNum) {

	gamma = 1;
	//TODO[4][1]:������ܿ���ȥ�������֮ǰÿһ��·����cost��ά���Ļ�
	//TODO[4][2]:���ǽӵ��Ŷ�����Ͳ�̫����
	reCalRtsCostSumCost();

	auto solclone = *this;

	static ProbControl pcRuinkind(3);
	static ProbControl pcClEPkind(6);
	
	int retState = 0;

	DisType Before = RoutesCost;

	for (int conti = 1; conti < 20;++conti) {

		int kind = pcRuinkind.getIndexBasedData();
		int kindclep = pcClEPkind.getIndexBasedData();

		bool ispertutb = perturbBaseRuin(kind, ruinCusNum, kindclep);
		//debug(conti);
		if (ispertutb) {
			auto cuses = EAX::getDiffCusofPb(solclone, *this);
			if (cuses.size() > 0) {
				mRLLocalSearch(1,cuses);
			}
			if (RoutesCost < Before) {
				++pcRuinkind.data[kind];
				++pcClEPkind.data[kindclep];
			}
			break;
		}
		else {
			//*this = pBest;
			continue;
		}
	}
	
	return retState;
}

int Solver::CVB2ClearEPAllowNewR(int kind) {

	std::unordered_set<int> insRts;
	Vec<int> EPArr = rPutCusInve(EPr);

	auto cmp1 = [&](int a, int b) {
		return input.datas[a].DEMAND > input.datas[b].DEMAND;
	};
	auto cmp2 = [&](int a, int b) {
		return input.datas[a].DUEDATE - input.datas[a].READYTIME
			< input.datas[b].DUEDATE - input.datas[b].READYTIME;
	};

	auto cmp3 = [&](int a, int b) {
		return input.disOf[a][0] < input.disOf[b][0];
	};

	auto cmp4 = [&](int a, int b) {
		return input.datas[a].READYTIME > input.datas[b].READYTIME;
	};

	auto cmp5 = [&](int a, int b) {
		return input.datas[a].DUEDATE < input.datas[b].DUEDATE;
	};


	switch (kind) {
	case 0:
		myRand->shuffleVec(EPArr);
		break;
	case 1:
		std::sort(EPArr.begin(), EPArr.end(), cmp1);
		break;
	case 2:
		std::sort(EPArr.begin(), EPArr.end(), cmp2);
		break;
	case 3:
		std::sort(EPArr.begin(), EPArr.end(), cmp3);
		break;
	case 4:
		std::sort(EPArr.begin(), EPArr.end(), cmp4);
		break;
	case 5:
		std::sort(EPArr.begin(), EPArr.end(), cmp5);
		break;
	default:
		break;
	}

	for (int i = 0; i < EPArr.size(); ++i) {
		//for (int i = EPArr.size() - 1;i>=0;--i) {
		int pt = EPArr[i];
		EPrRemoveAtPos(pt);
		//++P[pt];
		auto bestPos = findBestPosForRuin(pt);
		
		if (bestPos.pen == 0) {
			Route& r = rts[bestPos.rIndex];
			insRts.insert(r.routeID);
			rInsAtPos(r, bestPos.pos, pt);
			rUpdateAvQfrom(r, pt);
			rUpdateZvQfrom(r, pt);
		}
		else {
			int rid = 0;
			for (; rid < rts.posOf.size(); ++rid) {
				if (rts.posOf[rid] == -1) {
					break;
				}
			}
			Route r1 = rCreateRoute(rid);
			rInsAtPosPre(r1, r1.tail, pt);
			rUpdateAvQfrom(r1, r1.head);
			rUpdateZvQfrom(r1, r1.tail);
			rts.push_back(r1);
			insRts.insert(rid);
		}
	}

	sumRtsPen();
	for (auto rId : insRts) {
		Route& r = rts.getRouteByRid(rId);
		rReCalRCost(r);
	}
	sumRtsCost();
	return 0;
}

int Solver::CVB2ruinLS(int ruinCusNum) {

	int avgLen = input.custCnt / rts.cnt;
	int Lmax = std::min<int>(globalCfg->ruinLmax, avgLen);
	int ruinKmax = 4 * ruinCusNum / (1 + Lmax) - 1;
	ruinKmax = std::min<int>(rts.cnt, ruinKmax);
	ruinKmax = std::max<int>(1, ruinKmax);

	static ProbControl pcRuKind(3);
	static ProbControl pcCLKind(6);

	Solver solClone = *this;
	int perturbkind = pcRuKind.getIndexBasedData();

	Vec<int> ruinCus;
	if (perturbkind == 0) {
		ruinCus = ruinGetRuinCusByRound(ruinCusNum);
	}
	else if (perturbkind == 1) {
		ruinCus = ruinGetRuinCusBySec(ruinCusNum);
	}
	else if (perturbkind == 2) {
		ruinCus = ruinGetRuinCusBySting(ruinKmax, Lmax);
	}
	else {
		ERROR("no this kind of ruin");
	}

	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRid(customers[cus].routeID);
		rIds.insert(r.routeID);
		rRemoveAtPos(r, cus);
		EPpush_back(cus);

		if (r.rCustCnt == 0) {
			if (rIds.count(r.routeID)>0) {
				rIds.erase(rIds.find(r.routeID));
			}
			removeOneRouteByRid(r.routeID);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRid(rid);
		rUpdateAvQfrom(r, r.head);
		rUpdateZvQfrom(r, r.tail);
		rReCalRCost(r);
	}

	sumRtsCost();
	sumRtsPen();

	int clearKind = pcCLKind.getIndexBasedData();

	CVB2ClearEPAllowNewR(clearKind);

	auto cuses = EAX::getDiffCusofPb(solClone, *this);
	if (cuses.size() > 0) {
		mRLLocalSearch(1, cuses);
	}
	reCalRtsCostAndPen();
	if (RoutesCost < solClone.RoutesCost) {
		++pcRuKind.data[perturbkind];
		++pcCLKind.data[clearKind];
	}
	return true;
}

//0 ��ʾ������������·��1��ʾ����������·
int Solver::Simulatedannealing(int kind,int iterMax, double temperature,int ruinNum) {

	//Solver pBest = *this;
	Solver s = *this;

	double j0 = temperature;
	double jf = 1;
	//temperature = j0;
	double c = pow(jf / j0, 1 / double(iterMax));

	//double temperature = j0;

	int iter = 1;

	while (++iter < iterMax){

		auto sStar = s;

		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == globalCfg->d15RecRL) {
				break;
			}
		}

		temperature *= c;
		//printf("tmp %lf \n", temperature);
		
		if (squIter * 10 > IntInf) {
			squIter = 1;
			for (auto& i : (*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}
		if (kind == 0) {
			//sStar.ruinLocalSearchNotNewR(globalCfg->ruinC_);
			sStar.ruinLocalSearchNotNewR(ruinNum);
		}
		else if (kind == 1) {
			//sStar.CVB2ruinLS(globalCfg->ruinC_);
			sStar.CVB2ruinLS(ruinNum);
		}
		
		bks->updateBKS(sStar);

		DisType delt = temperature * log(double(myRand->pick(1, 100000)) / (double)100000);
		//INFO("temperature:", temperature,"delt:",delt);
		if (sStar.RoutesCost < s.RoutesCost - delt) {
			//INFO("st.RoutesCost:",st.RoutesCost,"pBest.rts.cnt:", pBest.rts.cnt);
			s = sStar;
		}
		//if (sStar.RoutesCost < pBest.RoutesCost) {
		//	pBest = sStar;
		//}
	}

	*this = s;
	return true;

}

bool Solver::patternAdjustment(int Irand) {

	int I1000 = myRand->pick(globalCfg->Irand);
	if (Irand > 0) {
		I1000 = Irand;
	}

	int iter = 0;
	gamma = 0;

	Vec<int> kindSet = { 0,1,6,7,/*8,9,10,2,3,4,5*/ };

	int N = globalCfg->patternAdjustmentNnei;
	int m = std::min<int>(globalCfg->patternAdjustmentGetM, N);

	auto getDelt0MoveRandomly = [&]() {

		TwoNodeMove ret;

		for (int iter = 0; ++iter < 100;++iter) {

			int v = myRand->pick(input.custCnt) + 1;
			if (customers[v].routeID == -1) {
				continue;
			}

			m = std::max<int>(1, m);
			myRandX->getMN(N, m);
			Vec<int>& ve = myRandX->mpLLArr[N];
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				//TODO[-1]:����ĳ���addSTclose
				//int w = input.allCloseOf[v][wpos];
				int w = input.addSTclose[v][wpos];
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
						d = estimatevw(kind, v, w, 0);
					}

					#if ATTRIBUTETABU
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

	squIter += globalCfg->yearTabuLen + globalCfg->yearTabuRand;

	do {

		TwoNodeMove bestM = getDelt0MoveRandomly();
		
		if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {
			break;
		}

		updateYearTable(bestM);
		doMoves(bestM);
		++squIter;

	} while (++iter < I1000);

	sumRtsPen();

	return true;
}

Vec<Solver::eOneRNode> Solver::ejectFromPatialSol() {

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
		int tKmax = globalCfg->minKmax;
		//tKmax = globalCfg->maxKmax;

		while (tKmax <= globalCfg->maxKmax) {

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
				bool s2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();

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
		//eOneRNode retNode = ejectOneRouteOnlyP(r, 2, globalCfg->maxKmax);
		//auto en = ejectOneRouteMinPsumGreedy(r);

		if (retNode.ejeVe.size() == 0) {
			retNode = ejectOneRouteMinPsumGreedy(r);
		}
		//else {

		//	if (en.Psum < retNode.Psum) {
		//		//bool satisfy2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();
		//		bool satisfy2 = en.ejeVe.size() > globalCfg->maxKmax;
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
		lyhCheckTrue(retNode.ejeVe.size() > 0);
		#endif // CHECKING

		ret.push_back(retNode);
	}

	return ret;
}

Solver::eOneRNode Solver::ejectOneRouteOnlyHasPcMinP(Route& r, int Kmax) {

	eOneRNode noTabuN(r.routeID);
	noTabuN.Psum = 0;

	Vec<int> R = rPutCusInve(r);

	auto cmpMinP = [&](const int& a, const int& b) {

		if (input.P[a] == input.P[b]) {
			return input.datas[a].DEMAND > input.datas[b].DEMAND;
		}
		else {
			return input.P[a] > input.P[b];
		}
		return false;
	};

	auto cmpMinD = [&](const int& a, const int& b) -> bool {

		if (input.datas[a].DEMAND == input.datas[b].DEMAND) {
			return input.P[a] > input.P[b];
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
		noTabuN.Psum += input.P[ctop];
	}

	return noTabuN;

}

Solver::eOneRNode Solver::ejectOneRouteOnlyP(Route& r, int kind, int Kmax) {

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
				: std::max<DisType>(customers[n].avp, input.datas[n].READYTIME);
			customers[n].TW_X = std::max<DisType>(0, customers[n].avp - input.datas[n].DUEDATE);
			customers[n].TW_X += customers[pre].TW_X;
			pre = n;
			n = customers[n].next;
		}
	};

	auto delOneNodeInPreOrder = [&](int n) {

		rQ -= input.datas[n].DEMAND;
		etemp.ejeVe.push_back(n);
		etemp.Psum += input.P[n];

		int next = customers[n].next;
		customers[next].pre = customers[n].pre;
		updateAvfromSubRoute(next);
	};

	// ʹ�ù�ʽ�� ����v��avp����Ptw
	auto getPtwUseEq2 = [&](int v) {

		DisType avp = getAvpOf(v);
		DisType ptw = std::max<DisType>(0, avp - customers[v].zv) + customers[v].TWX_;
		int pre = customers[v].pre;
		ptw += customers[pre].TW_X;
		return ptw;
	};

	auto restoreOneNodePreOrder = [&](int n) {

		rQ += input.datas[n].DEMAND;
		etemp.ejeVe.pop_back();
		etemp.Psum -= input.P[n];

		int next = customers[n].next;
		customers[next].pre = n;
		updateAvfromSubRoute(n);
	};

	auto getPtwIfRemoveOneNode = [&](int head) {

		int pt = head;
		int pre = customers[pt].pre;
		int next = customers[pt].next;

		#if CHECKING
		lyhCheckTrue(pre != -1);
		lyhCheckTrue(next != -1);
		#endif // CHECKING

		DisType ptw = 0;

		while (next != -1) {

			//debug(pt)
			DisType avnp = customers[pre].av + input.datas[pre].SERVICETIME + input.disOf[reCusNo(pre)][reCusNo(next)];
			ptw = std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[pre].TW_X;

			if (customers[pre].TW_X > 0) { // ��֦ ɾ��ik֮�� ǰ���ʱ�䴰û������
				return etemp;
			}

			rQ -= input.datas[pt].DEMAND;
			etemp.ejeVe.push_back(pt);
			etemp.Psum += input.P[pt];

			if (ptw == 0 && rQ - input.Q <= 0) {
				updateEje();
			}

			rQ += input.datas[pt].DEMAND;
			etemp.ejeVe.pop_back();
			etemp.Psum -= input.P[pt];

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

			// ������ͬ����Psum �ķ��� >
			// ��������ͬ����Psum �ķ��� >=
			while (etemp.Psum + input.P[delv] > noTabuN.Psum && ve[k] < N) {
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

			// ������ͬ����Psum �ķ��� >
			// ��������ͬ����Psum �ķ��� >=
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

Solver::eOneRNode Solver::ejectOneRouteMinPsumGreedy
(Route& r, eOneRNode cutBranchNode) {

	eOneRNode ret(r.routeID);
	ret.Psum = 0;
	Vec<int> R = rPutCusInve(r);

	auto cmp = [&](const int& a, const int& b) ->bool {

		if (input.P[a] == input.P[b]) {
			/*return input.datas[a].DUEDATE - input.datas[a].READYTIME <
				input.datas[b].DUEDATE - input.datas[b].READYTIME;*/
			return input.datas[a].DEMAND > input.datas[b].DEMAND;
		}
		else {
			return input.P[a] > input.P[b];
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
		DisType ptw = std::max<DisType>(0, avnp - customers[nex].zv) + customers[nex].TWX_ + customers[pre].TW_X;

		if (ptw < curPtw) {

			rRemoveAtPos(r, ctop);
			rUpdateAvfrom(r, pre);
			rUpdateZvfrom(r, nex);
			curPtw = ptw;
			ret.ejeVe.push_back(ctop);
			ret.Psum += input.P[ctop];

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
		ret.Psum += input.P[ctop];
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

bool Solver::resetSol() {

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

bool Solver::EPNodesCanEasilyPut() {

	for (int EPIndex = 0; EPIndex < EPsize();) {
		#if CHECKING
		DisType oldpenalty = PtwNoWei + Pc;
		#endif // CHECKING

		Vec<int> arr = EPve();
		int top = arr[EPIndex];

		Position bestP = findBestPosInSol(top);
		//TODO[0]:�������findBestPosInSolForInit
		//Position bestP = findBestPosInSolForInit(top);

		if (bestP.pen == 0) {

			Route& r = rts[bestP.rIndex];

			input.P[top] += globalCfg->Pwei0;
			//EP(*yearTable)[top] = EPIter + globalCfg->EPTabuStep + myRand->pick(globalCfg->EPTabuRand);
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

bool Solver::ejectLocalSearch() {

	minEPcus = IntInf;
	squIter += globalCfg->yearTabuLen + globalCfg->yearTabuRand;
	int maxOfPval = -1;
	gamma = 0;

	int EpCusNoDown = 1;
	int iter = 1;

	while (iter < globalCfg->ejectLSMaxIter) {
		//while (1) {

		++iter;

		EPNodesCanEasilyPut();

		if (EPr.rCustCnt < minEPcus) {
			minEPcus = EPr.rCustCnt;
			EpCusNoDown = 1;
		}
		else {

			++EpCusNoDown;
			if (EpCusNoDown % 10000 == 0) {

				//globalCfg->minKmax = 1;
				// ���� minKmax ��1 2 ֮���л�
				globalCfg->minKmax = 3 - globalCfg->minKmax;
				INFO("globalCfg->minKmax:", globalCfg->minKmax);
			}
		}

		minEPcus = std::min<int>(minEPcus, EPr.rCustCnt);
		if (EPsize() == 0 && penalty == 0) {
			//debug(iter);
			break;
		}

		Vec<int> EPrVe = rPutCusInve(EPr);
		int top = EPrVe[myRand->pick(EPrVe.size())];

		Position bestP = findBestPosInSol(top);
		//TODO[0]:�������findBestPosInSolForInit
		//Position bestP = findBestPosInSolForInit(top);

		Route& r = rts[bestP.rIndex];
		EPremoveByVal(top);

		input.P[top] += globalCfg->Pwei0;
		maxOfPval = std::max<int>(input.P[top], maxOfPval);

		if (maxOfPval >= 1000) {
			maxOfPval = 0;
			for (auto& i : input.P) {
				i = i * 0.4 + 1;
				maxOfPval = std::max<int>(maxOfPval, i);
			}
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
		reCalRtsCostAndPen();
		lyhCheckTrue((oldpenalty + bestP.pen == PtwNoWei + Pc));
		if (oldpenalty + bestP.pen != PtwNoWei + Pc) {
			ERROR("oldpenalty:", oldpenalty);
			ERROR("bestP.pen:", bestP.pen);
			ERROR("PtwNoWei + Pc:", PtwNoWei + Pc);
		}
		#endif // CHECKING

		if (bestP.pen == 0) {
			continue;
		}
		else {

			#if CHECKING
			lyhCheckTrue(penalty > 0);
			if (penalty == 0) {
				ERROR("penalty == 0:", penalty == 0);
				ERROR("penalty == 0:", penalty == 0);
			}
			#endif // CHECKING

			bool squ = squeeze();
			

			if (squ == true) {
				continue;
			}
			else {

				auto& XSet = ejeNodesAfterSqueeze;

				for (eOneRNode& en : XSet) {
					for (int c : en.ejeVe) {
						/*int cpre = customers[c].pre > input.custCnt ? 0 : customers[c].pre;
						int cnext = customers[c].next > input.custCnt ? 0 : customers[c].next;
						(*yearTable)[cpre][c] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);
						(*yearTable)[c][cnext] = squIter + globalCfg->yearTabuLen + myRand->pick(globalCfg->yearTabuRand);*/
						input.P[c] += globalCfg->Pwei1;
						maxOfPval = std::max<DisType>(input.P[c], maxOfPval);

						//P[c] += std::max<DisType>(log(P[c]), globalCfg->Pwei1);
						//debug(std::max<DisType>(log(P[c]), globalCfg->Pwei1))
					}
				}

				doEject(XSet);
				int Irand = input.custCnt / EPr.rCustCnt / 4;
				Irand = std::max<int>(Irand, 100);
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

	//debug(iter);


	return (EPsize() == 0 && penalty == 0);
}

void Solver::minimizeRN(int ourTarget) {

	gamma = 0;

	lyhTimer.reStart();

	while (rts.cnt > ourTarget) {

		Solver sclone = *this;
		removeOneRouteByRid();

		std::fill(input.P.begin(), input.P.end(), 1);
		bool isDelete = ejectLocalSearch();
		if (isDelete) {
			if (rts.cnt == input.Qbound) {
				break;
			}
			//saveOutAsSintefFile();
			//INFO("rts.size():",rts.size());
		}
		else {
			*this = sclone;
			break;
		}
	}
	INFO("minRN,rts.size():",rts.size());
}

bool Solver::adjustRN(int ourTarget) {

	if (rts.cnt > ourTarget) {
		minimizeRN(ourTarget);
	}
	else if (rts.cnt < ourTarget) {
		
		while (rts.cnt < ourTarget) {

			int rId = -1;
			for (int i = 0; i < rts.posOf.size(); ++i) {
				if (rts.posOf[i] == -1) {
					rId = i;
					break;
				}
			}

			Route r1 = rCreateRoute(rId);
			int rIdChose = -1;
			for (int i = 0; i < rts.cnt; ++i) {
				if (rts[i].rCustCnt >= 2) {
					rIdChose = i;
					break;
				}
			}
			
			Route& r = rts[rIdChose];

			auto arr = rPutCusInve(r);
			for (int i = 0; i < arr.size(); ++i) {
				if (i % 2 == 0) {
					rRemoveAtPos(r, arr[i]);
					rInsAtPosPre(r1, r1.tail, arr[i]);
				}
			}
			rUpdateAvQfrom(r1, r1.head);
			rUpdateZvQfrom(r1, r1.tail);
			rUpdateAvQfrom(r, r.head);
			rUpdateZvQfrom(r, r.tail);
			rts.push_back(r1);
			sumRtsPen();
		}
	}
	
	//TODO[0]:Lmax��ruinLmax�Ķ���
	globalCfg->ruinLmax = 0;
	for (int i = 0; i < rts.cnt; ++i) {
		globalCfg->ruinLmax = std::max<int>(globalCfg->ruinLmax,rts[i].rCustCnt);
	}
	globalCfg->ruinC_ = (globalCfg->ruinLmax + 1);
	return true;
}
#if 0
Solver::TwoNodeMove Solver::naRepairGetMoves(std::function<bool(TwoNodeMove& t, TwoNodeMove& bestM)>updateBestM) {

	TwoNodeMove bestM;

	Vec<int> confRts;

	confRts.insert(confRts.begin(), PtwConfRts.ve.begin(), PtwConfRts.ve.begin() + PtwConfRts.cnt);
	confRts.insert(confRts.begin(), PcConfRts.ve.begin(), PcConfRts.ve.begin() + PcConfRts.cnt);
	//debug(confRts.size())
	for (int rId : confRts) {
		Route& r = rts.getRouteByRid(rId);
		Vec<int> cusV = rPutCusInve(r);
		for (int v : cusV) {

			if (customers[v].routeID == -1) {
				continue;
			}
			for (int wpos = 0; wpos < globalCfg->naRepairGetMovesNei; ++wpos) {

				int w = input.allCloseOf[v][wpos];
				if (customers[w].routeID == -1) {
					continue;
				}
				for (int i = 0; i < 15; ++i) {

					TwoNodeMove m;
					m = TwoNodeMove(v, w, i, estimatevw(i, v, w, 1));
					updateBestM(m, bestM);
				}
			}
		}
	}
	return bestM;
}
#endif // 0

bool Solver::repair() {

	for (int i = 0; i < rts.cnt; ++i) {
		rts[i].rWeight = 1;
	}

	resetConfRts();
	sumRtsPen();

	gamma = 1;

	squIter += globalCfg->yearTabuLen + globalCfg->yearTabuRand;

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

	int contiNotDe = 0;

	//static Vec<int> moveContribute(16,0);

	auto iter = 0;

	auto penBest = penalty;

	while (penalty > 0 && !lyhTimer.isTimeOut()) {
		++iter;
		if (contiNotDe > globalCfg->repairExitStep) {
			break;
		}
		//TODO[2][repair]:�����޸���������������ôѡ
		TwoNodeMove bestM = getMovesRandomly(updateBestM);

		if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {

			++contiNotDe;
			if (contiNotDe > globalCfg->repairExitStep) {
				break;
			}
			//maxCon = std::max<int>(maxCon,contiNotDe);
			if (bestM.v == 0 && bestM.w == 0) {
				break;
			}
			else {
				continue;
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

		if (penalty < penBest) {
			penBest = penalty;
			contiNotDe = 1;
		}
		else {
			++contiNotDe;
		}
	}
	//TODO[2][repair]:��ӡ�޸�����
	//printve(moveContribute);

	if (penalty == 0) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool Solver::mRLLocalSearch(int hasRange,Vec<int> newCus) {

	alpha = 1;
	beta = 1;
	gamma = 1;

	reCalRtsCostSumCost();

	TwoNodeMove MRLbestM;

	static Vec<int> contribution(16, 0);

	static Vec<int> moveKindOrder = { 0,1,2,3,4,5,6,7, 8/*,9,10*,/11,12,/*13,14 */,15};
	std::sort(moveKindOrder.begin(), moveKindOrder.end(), [&](int a, int b) {
		return contribution[a] > contribution[b];
	});
	//printve(moveKindOrder);
	auto maxIt = std::max_element(contribution.begin(), contribution.end());
	if (*maxIt > 1000) {
		for (auto& i : contribution) { i >>= 2; }
	}

	auto MRLUpdateM = [&](TwoNodeMove& m) {

		if (m.deltPen.deltPc <= 0 && m.deltPen.deltPtw <= 0) {
			if (m.deltPen.deltCost < MRLbestM.deltPen.deltCost) {
				MRLbestM = m;
			}
		}
	};
	
	#if CHECKING
	if (hasRange == 0 && newCus.size() > 0) {
		ERROR("hasRange == 0 && newCus.size() > 0");
	}

	if (hasRange == 1 && newCus.size() == 0) {
		ERROR("hasRange == 1 && newCus.size() == 0");
	}
	#endif // CHECKING

	if (hasRange == 0) {
		newCus = myRandX->getMN(input.custCnt + 1, input.custCnt + 1);
		myRand->shuffleVec(newCus);
	}

	auto getMovesGivenRange = [&](int range) {

		myRand->shuffleVec(newCus);

		MRLbestM.reSet();
		//auto wposOrder = myRandX->getMN(range, range);
		//unsigned shuseed = myRand->pickRandSeed();
		//std::shuffle(wposOrder.begin(), wposOrder.end(), std::default_random_engine(shuseed));

		for (int v : newCus) {
			if (customers[v].routeID == -1) {
				continue;
			}
			for (int wpos = 0; wpos < range; ++wpos) {

				//TODO[-1]:����ĳ���addSTclose
				//int w = input.allCloseOf[v][wpos];
				int w = input.addSTclose[v][wpos];
				//int w = input.sectorClose[v][wpos];
				if (customers[w].routeID == -1) {
					continue;
				}
				for (int kind : moveKindOrder) {

					//TwoNodeMove m = TwoNodeMove(v, w, kind, estimatevw(kind, v, w, 1));
					TwoNodeMove m;
					if (kind >= 9) {
						m = TwoNodeMove(v, w, kind, estimatevw(kind, v, w, 0));
					}
					else {
						m = TwoNodeMove(v, w, kind, estimatevw(kind, v, w, 1));
					}
					MRLUpdateM(m);

					if (MRLbestM.deltPen.deltCost < 0) {
						return MRLbestM;
					}
				}
			}
		}
		return MRLbestM;
	};

	auto getAllRange = [&](int range) {

		MRLbestM.reSet();

		for (int v : newCus) {
			if (customers[v].routeID == -1) {
				continue;
			}
			for (int wpos = 0; wpos < range; ++wpos) {
				//TODO[-1]:����ĳ���addSTclose
				int w = input.addSTclose[v][wpos];
				//int w = input.allCloseOf[v][wpos];
				//int w = input.sectorClose[v][wpos];
				if (customers[w].routeID == -1) {
					continue;
				}
				for (int kind : moveKindOrder) {

					TwoNodeMove m;
					m = TwoNodeMove(v, w, kind, estimatevw(kind, v, w, 1));
					MRLUpdateM(m);

					if (MRLbestM.deltPen.deltCost < 0) {
						return MRLbestM;
					}

				}
			}
		}
		return MRLbestM;
	};

	for (int i = 0; i < rts.cnt; ++i) {
		rts[i].rWeight = 1;
	}
	squIter += globalCfg->yearTabuLen + globalCfg->yearTabuRand;

	Vec<int>& ranges = globalCfg->mRLLocalSearchRange;

	while (!lyhTimer.isTimeOut()) {

		TwoNodeMove bestM;
		//TwoNodeMove bestM 
		for (int i = 0; i < ranges.size(); ++i) {
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

		//if (bestM.deltPen.PtwOnly > 0
		//	|| bestM.deltPen.PcOnly > 0
		//	|| bestM.deltPen.deltCost >= 0
		//	) {
		//	bestM = getAllRange(ranges.back());
		//}

		if (bestM.deltPen.PtwOnly > 0
			|| bestM.deltPen.PcOnly > 0
			|| bestM.deltPen.deltCost >= 0
			) {
			break;
		}

		++contribution[bestM.kind];

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
		if (!(costafterplus == RoutesCost)) {
			ERROR("costafterplus:", costafterplus);
			ERROR("RoutesCost:", RoutesCost);
			ERROR("RoutesCost:", RoutesCost);
		}

		lyhCheckTrue(penaltyafterupdatePen == penalty);
		lyhCheckTrue(costafterplus == RoutesCost);

		lyhCheckTrue(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty);
		lyhCheckTrue(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly == PtwNoWei + Pc);
		lyhCheckTrue(oldRcost + bestM.deltPen.deltCost == RoutesCost);
		bool ver = verify();
		lyhCheckTrue(ver > 0)

			if (!(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty)) {
				ERROR(1111);
			}
		#endif // CHECKING

	}
	//TODO[5]:������±����� ��Ϊ����������û�и���ÿһ��·����routeCost
	reCalRtsCostSumCost();
	//outVe(a);
	return true;
}

Output Solver::saveToOutPut() {

	Output output;

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

bool Solver::printDimacs() {

	for (int i = 0; i < rts.size(); ++i) {
		Route& r = rts[i];

		printf("Route #%d:", i + 1);
		int pt = customers[r.head].next;
		while (pt <= input.custCnt) {
			printf(" %d", pt);
			pt = customers[pt].next;
		}
		printf("\n");
	}
	
	//����ı�����Ҫ����һ��
	printf("Cost %.1lf\n", double(RoutesCost) / 10);
	fflush(stdout);
	return true;
}

bool Solver::saveOutAsSintefFile(std::string opt) {

	std::string s = "";

	MyString ms;

	s += "Instance name : " + input.example + "\n";
	s += "Authors : <the name of the authors>\n";
	s += "Date : <dd - mm - yy>\n";
	s += "Reference : <reference to publication of method>\n";
	s += "Solution\n";
	for (int i = 0; i < rts.cnt; i++) {
		auto cus = rPutCusInve(rts[i]);
		std::string r = "Route " + ms.int_str(i + 1) + " :";

		for (auto c : cus) {
			r += " " + ms.int_str(c);
		}
		r += "\n";
		s += r;
	}

	reCalRtsCostSumCost();

	std::ofstream rgbData;
	std::string wrPath = globalCfg->outputPath + opt
		+ input.example + "L" + std::to_string(RoutesCost) + ".txt";

	rgbData.open(wrPath, std::ios::app | std::ios::out);

	if (!rgbData) {
		ERROR("sintefoutput file open errno");
		return false;
	}

	rgbData << s;

	//debug(s);
	rgbData.close();

	return true;
}

Solver::~Solver() {};

BKS::BKS() {
	bestSolFound.penalty = DisInf;
	bestSolFound.RoutesCost = DisInf;
}

void BKS::reSet() {
	lastRec = DisInf;
	bestSolFound.penalty = DisInf;
	bestSolFound.RoutesCost = DisInf;
}

bool BKS::updateBKS(Solver& newSol, std::string opt) {
	if (newSol.RoutesCost < bestSolFound.RoutesCost) {
		lastRec = bestSolFound.RoutesCost;
		bestSolFound = newSol;
		//bestSolFound.printDimacs();
		INFO("new bks cost:", bestSolFound.RoutesCost,
			 opt,"rn:",bestSolFound.rts.cnt, "up:",lastRec - bestSolFound.RoutesCost);
		#if DIMACSGO
		bks->bestSolFound.printDimacs();
		#endif // DIMACSGO

		return true;
	}
	return false;
}


}