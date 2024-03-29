
#include "EAX.h"
#include "Input.h"
#include "Solver.h"


namespace hust{

Solver::Solver(Input* input,YearTable* yearTable,BKS* bks) :
	input(input),
	aps(input->aps),
	rts(input->customerNumer / 8),
	EP(input->customerNumer / 8),
	PtwConfRts(input->customerNumer / 8),
	PcConfRts(input->customerNumer / 8),
	yearTable(yearTable),
	bks(bks),
	random(&input->random),
	randomx(&input->randomx),
	timer(&input->timer)
{

	customers = Vector<Customer>( static_cast<int>((input->customerNumer + 1) * 1.5) );
	alpha = 1;
	beta = 1;
	gamma = 1;
	//iter = 1;
	penalty = 0;
	Ptw = 0;
	PtwNoWei = 0;
	Pc = 0;
	//minEPSize = inf;
	RoutesCost = DisInf;
}

Solver::Solver(const Solver& s) {

	this->input = s.input;
	this->rts = s.rts;
	this->PtwConfRts = s.PtwConfRts;
	this->PcConfRts = s.PcConfRts;
	this->customers = s.customers;
	this->penalty = s.penalty;
	this->Ptw = s.Ptw;
	this->PtwNoWei = s.PtwNoWei;
	this->Pc = s.Pc;
	this->alpha = s.alpha;
	this->beta = s.beta;
	this->gamma = s.gamma;
	this->RoutesCost = s.RoutesCost;
	this->EP = s.EP;
	this->aps = s.aps;
	this->random = s.random;
	this->randomx = s.randomx;
	this->bks = s.bks;
	this->timer = s.timer;
	this->yearTable = s.yearTable;

}

Solver& Solver::operator = (const Solver& s) {

	if (this != &s) {

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
		this->EP = s.EP;
		this->aps = s.aps;
		this->random = s.random;
		this->randomx = s.randomx;
		this->bks = s.bks;
		this->timer = s.timer;
		this->yearTable = s.yearTable;
	}
	return *this;
}

// route function
Route Solver::rCreateRoute(int id) {

	Route r(id);
	int index = input->customerNumer + rts.cnt * 2 + 1;

	if (index + 1 >= static_cast<int>(customers.size())) {
		int oldSize = static_cast<int>(customers.size());
		customers.resize(oldSize + oldSize / 2 + 1, customers[0]);
	}

	r.head = index;
	r.tail = index + 1;

	customers[r.head].prev = -1;
	customers[r.head].next = r.tail;
	customers[r.tail].prev = r.head;
	customers[r.tail].next = -1;

	customers[r.head].routeId = id;
	customers[r.tail].routeId = id;

	customers[r.head].av = customers[r.head].avp
		= input->datas[r.head].READYTIME;
	customers[r.head].QX_ = 0;
	customers[r.head].Q_X = 0;
	customers[r.head].TWX_ = 0;
	customers[r.head].TW_X = 0;

	customers[r.tail].zv = customers[r.tail].zvp
		= input->datas[r.tail].DUEDATE;
	customers[r.tail].QX_ = 0;
	customers[r.tail].Q_X = 0;
	customers[r.tail].TWX_ = 0;
	customers[r.tail].TW_X = 0;

	return r;

}

void Solver::rUpdatePc(Route& r) {

	int pt = r.head;
	r.rQ = 0;
	while (pt != -1) {
		r.rQ += input->datas[pt].DEMAND;
		pt = customers[pt].next;
	}
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);
}

bool Solver::rReset(Route& r) {

	r.rCustCnt = 0; //not include depot
	//r.routeId = -1;
	r.routeCost = 0;
	r.rPc = 0;
	r.rPtw = 0;
	r.rWeight = 1;

	int pt = r.head;

	while (pt != -1) {

		int ptnext = customers[pt].next;
		customers[pt].reset();
		pt = ptnext;
	}

	r.head = -1;
	r.tail = -1;

	return true;
}

bool Solver::rRemoveAllCustomersInRoute(Route& r) {

	r.routeCost = 0;
	r.rWeight = 1;

	Vector<int> ve = rPutCustomersInVector(r);
	for (int pt : ve) {
		rRemoveAtPosition(r, pt);
	}

	rUpdateAvQFrom(r, r.head);
	rUpdateZvQFrom(r, r.tail);

	return true;
}

bool Solver::rInsertAtPosition(Route& r, int pos, int node) {

	customers[node].routeId = r.routeId;
	r.rQ += input->datas[node].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);

	int anext = customers[pos].next;
	customers[node].next = anext;
	customers[node].prev = pos;
	customers[pos].next = node;
	customers[anext].prev = node;

	++r.rCustCnt;

	return true;
}

bool Solver::rInsAtPosPre(Route& r, int pos, int node) {

	customers[node].routeId = r.routeId;
	r.rQ += input->datas[node].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);

	int apre = customers[pos].prev;
	customers[node].prev = apre;
	customers[node].next = pos;
	customers[pos].prev = node;
	customers[apre].next = node;
	++r.rCustCnt;

	return true;
}

bool Solver::rRemoveAtPosition(Route& r, int a) {

	if (r.rCustCnt <= 0) {
		return false;
	}

	r.rQ -= input->datas[a].DEMAND;
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);

	Customer& temp = customers[a];
	Customer& tpre = customers[temp.prev];
	Customer& tnext = customers[temp.next];

	tnext.prev = temp.prev;
	tpre.next = temp.next;

	temp.next = -1;
	temp.prev = -1;
	temp.routeId = -1;
	r.rCustCnt--;
	return true;
}

void Solver::rPrevDisplay(Route& r) {

	int pt = r.tail;
	std::cout << "predisp: ";
	while (pt != -1) {
		std::cout << pt << " ";
		std::cout << ",";
		pt = customers[pt].prev;
	}
	std::cout << std::endl;
}

void Solver::rNextDisplay(Route& r) {

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

void Solver::rUpdateAvQFrom(Route& r, int vv) {

	int v = vv;
	if (v == r.head) {
		v = customers[v].next;
	}
	int pt = v;
	int ptpre = customers[pt].prev;

	while (pt != -1) {

		customers[pt].avp =
			customers[pt].av = customers[ptpre].av + input->getDisof2(ptpre,pt) + input->datas[ptpre].SERVICETIME;

		customers[pt].TW_X = customers[ptpre].TW_X;
		customers[pt].TW_X += std::max<DisType>(customers[pt].avp - input->datas[pt].DUEDATE, 0);

		if (customers[pt].avp <= input->datas[pt].DUEDATE) {
			customers[pt].av = std::max<DisType>(customers[pt].avp, input->datas[pt].READYTIME);
		}
		else {
			customers[pt].av = input->datas[pt].DUEDATE;
		}

		customers[pt].Q_X = customers[ptpre].Q_X + input->datas[pt].DEMAND;

		ptpre = pt;
		pt = customers[pt].next;
	}

	r.rPtw = customers[r.tail].TW_X;
	r.rQ = customers[r.tail].Q_X;
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);

}

void Solver::rUpdateZvQFrom(Route& r, int vv) {

	int v = vv;

	if (v == r.tail) {
		v = customers[v].prev;
	}

	int pt = v;
	int ptnext = customers[pt].next;

	while (pt != -1) {

		customers[pt].zvp =
			customers[pt].zv = customers[ptnext].zv - input->getDisof2(pt,ptnext) - input->datas[pt].SERVICETIME;

		customers[pt].TWX_ = customers[ptnext].TWX_;

		customers[pt].TWX_ += std::max<DisType>(input->datas[pt].READYTIME - customers[pt].zvp, 0);

		customers[pt].zv = customers[pt].zvp >= input->datas[pt].READYTIME ?
			std::min<DisType>(customers[pt].zvp, input->datas[pt].DUEDATE) : input->datas[pt].READYTIME;

		customers[pt].QX_ = customers[ptnext].QX_ + input->datas[pt].DEMAND;
		ptnext = pt;
		pt = customers[pt].prev;
	}
	r.rPtw = customers[r.head].TWX_;
	r.rQ = customers[r.head].QX_;
	r.rPc = std::max<DisType>(0, r.rQ - input->vehicleCapacity);
}

int Solver::rGetCustomerNumber(Route& r) {

	int pt = customers[r.head].next;
	int ret = 0;
	while (pt <= input->customerNumer) {
		++ret;
		pt = customers[pt].next;
	}
	return ret;
}

void Solver::rReCalculateRouteCost(Route& r) {
	int pt = r.head;
	int ptnext = customers[pt].next;
	r.routeCost = 0;
	while (pt != -1 && ptnext != -1) {
		r.routeCost += input->getDisof2(pt,ptnext);
		pt = ptnext;
		ptnext = customers[ptnext].next;
	}
}

Vector<int> Solver::rPutCustomersInVector(Route& r) {

	Vector<int> ret;
	if (r.rCustCnt > 0) {
		ret.reserve(r.rCustCnt);
	}

	int pt = customers[r.head].next;

	while (pt <= input->customerNumer) {
		ret.push_back(pt);
		pt = customers[pt].next;
	}

	return ret;
}

void Solver::rReCalCusNumAndSetCusrIdWithHeadrId(Route& r) {

	int pt = r.head;
	r.rCustCnt = 0;
	while (pt != -1) {
		customers[pt].routeId = r.routeId;
		r.tail = pt;
		if (pt <= input->customerNumer) {
			++r.rCustCnt;
		}
		pt = customers[pt].next;
	}
}

void Solver::reCalRtsCostAndPen() {

	Pc = 0;
	Ptw = 0;
	PtwNoWei = 0;
	RoutesCost = 0;

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		rReCalCusNumAndSetCusrIdWithHeadrId(r);
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);

		PtwNoWei += r.rPtw;
		Ptw += r.rWeight * r.rPtw;
		Pc += r.rPc;
		RoutesCost += r.routeCost;
	}
	penalty = alpha * Ptw + beta * Pc;
}

void Solver::reCalRtsCostSumCost() {

	RoutesCost = 0;
	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		rReCalculateRouteCost(r);
		RoutesCost += r.routeCost;
	}
}

CircleSector Solver::rGetCircleSector(Route& r) {

	CircleSector ret;
	auto ve = rPutCustomersInVector(r);
	ret.initialize(input->datas[ve.front()].polarAngle);
	for (int j = 1; j < static_cast<int>(ve.size()); ++j) {
		ret.extend(input->datas[ve[j]].polarAngle);
	}
	return ret;
}

void Solver::sumRoutesPenalty() {

	Pc = 0;
	Ptw = 0;
	PtwNoWei = 0;

	for (int i = 0; i < rts.cnt; ++i) {

		Route& r = rts[i];
		PtwNoWei += r.rPtw;
		Ptw += r.rWeight * r.rPtw;
		Pc += r.rPc;
	}
	penalty = alpha * Ptw + beta * Pc;
}

void Solver::sumRoutesCost() {
	RoutesCost = 0;
	for (int i = 0; i < rts.cnt; ++i) {
		RoutesCost += rts[i].routeCost;
	}
}

DisType Solver::updatePenalty(const DeltaPenalty& delt) {

	Pc += delt.PcOnly;
	Ptw += delt.deltPtw;
	PtwNoWei += delt.PtwOnly;
	penalty = alpha * Ptw + beta * Pc;
	return penalty;
}

Solver::Position Solver::findBestPositionInSolution(int w) {

	Position bestPos;

	int sameCnt = 0;
	auto updatePool = [&](Position& pos) {

		if (pos.pen < bestPos.pen) {
			bestPos = pos;
			sameCnt = 1;
		}
		else if(pos.pen == bestPos.pen) {
			++sameCnt;
			//if ( pos.cost < bestPos.cost && random->pick(100)<90) {
			//	bestPos = pos;
			//}
			if (random->pick(sameCnt) == 0) {
				bestPos = pos;
			}
		}
	};

	for (int i = 0; i < rts.cnt; ++i) {
		//debug(i)
		Route& rt = rts[i];

		int v = rt.head;
		int vj = customers[v].next;

		DisType oldrPc = rts[i].rPc;
		DisType rPc = std::max<DisType>(0, rt.rQ + input->datas[w].DEMAND - input->vehicleCapacity);
		rPc = rPc - oldrPc;

		while (v != -1 && vj != -1) {

			DisType oldrPtw = rts[i].rPtw;

			DisType rPtw = customers[v].TW_X;
			rPtw += customers[vj].TWX_;

			DisType awp = customers[v].av + input->datas[v].SERVICETIME + input->getDisof2(v,w);
			rPtw += std::max<DisType>(0, awp - input->datas[w].DUEDATE);
			DisType aw =
				awp <= input->datas[w].DUEDATE ? std::max<DisType>(input->datas[w].READYTIME, awp) : input->datas[w].DUEDATE;

			DisType avjp = aw + input->datas[w].SERVICETIME + input->getDisof2(w,vj);
			rPtw += std::max<DisType>(0, avjp - input->datas[vj].DUEDATE);
			DisType avj =
				avjp <= input->datas[vj].DUEDATE ? std::max<DisType>(input->datas[vj].READYTIME, avjp) : input->datas[vj].DUEDATE;
			rPtw += std::max<DisType>(0, avj - customers[vj].zv);

			rPtw = rPtw - oldrPtw;

			DisType cost = input->getDisof2(v,w)
				+ input->getDisof2(w,vj)
				- input->getDisof2(v,vj);

			int vre = v > input->customerNumer ? 0 : v;
			int vjre = vj > input->customerNumer ? 0 : vj;

			int year = yearTable->table[vre][w] + yearTable->table[w][vjre];

			Position pos;
			pos.rIndex = i;
			pos.cost = cost;
			pos.pen = rPtw + rPc;
			pos.pos = v;
			pos.year = year;
			pos.secDis = abs(input->datas[w].polarAngle - input->datas[v].polarAngle);
			updatePool(pos);

			v = vj;
			vj = customers[vj].next;
		}
	}
	//if(sameCnt>0)
	//Logger::INFO("sameCnt:", sameCnt);
	return bestPos;

}

Solver::Position Solver::findBestPositionInSolutionForInitial(int w) {

	//int quMax = aps->initFindPosPqSize;

	Vector<CircleSector> secs(rts.cnt);
	for (int i = 0; i < rts.cnt; ++i) {
		secs[i] = rGetCircleSector(rts[i]);
	}

	Position bestPos;

	for (int i = 0; i < rts.cnt; ++i) {
		//debug(i)
		Route& rt = rts[i];

		int v = rt.head;
		int vj = customers[v].next;

		DisType rtPc = std::max<DisType>(0, rt.rQ + input->datas[w].DEMAND - input->vehicleCapacity);
		//rtPc = rtPc - rt.rPc;

		if (rtPc > bestPos.pen) {
			continue;
		}

		int secDis = CircleSector::disofpointandsec
		(input->datas[w].polarAngle, secs[i]);

		//if (secDis > bestPos.secDis) {
		//	continue;
		//}

		while (v != -1 && vj != -1) {

			DisType rtPtw = getPtwWithVectorOfCustomersArg(3, v,w,vj );

			rtPtw = rtPtw - rt.rPtw;

			DisType cost = input->getDisof2(v, w)
				+ input->getDisof2(w, vj)
				- input->getDisof2(v, vj);

			Position pt(i,v, rtPtw + rtPc,cost,secDis);

			if (pt.pen < bestPos.pen) {
				bestPos = pt;
			}
			else if (pt.pen == bestPos.pen) {

				if (pt.cost < bestPos.cost && random->pickDouble(0.0,1.0) < aps->initWinkRate) {
                    bestPos = pt;
				}
			}

			v = vj;
			vj = customers[vj].next;
		}
	}

	if (bestPos.secDis > vd4fpi || bestPos.cost > input->getDisof2(0,w) + input->getDisof2(w, 0)) {
		return Position();
	}

	return bestPos;
}

Solver::Position Solver::findBestPositionForRuin(int w) {

	Position ret;

	auto updatePool = [&](Position& pos) {

		if (random->pickDouble(0.0, 1.0) < aps->ruinWinkRate) {

			if (pos.pen < ret.pen) {
				ret = pos;
			}
			else if (pos.pen == ret.pen) {

				if (pos.cost < ret.cost) {
					ret = pos;
				}
			}
		}
	};

	auto& rtsIndexOrder = randomx->getMN(rts.cnt, rts.cnt);
	random->shuffleVec(rtsIndexOrder);
	//sort(rtsIndexOrder.begin(), rtsIndexOrder.end(), [&](int x, int y) {
	//	return rts[x].rQ < rts[y].rQ;
	//	});
	//printve(rtsIndexOrder);

	for (int i : rtsIndexOrder) {

		Route& r = rts[i];

		DisType oldrPc = rts[i].rPc;
		DisType rPc = std::max<DisType>(0, r.rQ + input->datas[w].DEMAND - input->vehicleCapacity);
		rPc = rPc - oldrPc;

		if (rPc > ret.pen) {
			continue;
		}

		Vector<int> a = rPutCustomersInVector(r);
		a.push_back(r.tail);

		for (int vj : a) {

			int v = customers[vj].prev;

			//if (random->pick(2) == 0) {
			//	continue;
			//}

			DisType oldrPtw = rts[i].rPtw;
			DisType rPtw = getPtwWithVectorOfCustomersArg(3, v,w,vj );
			
			rPtw = rPtw - oldrPtw;

			DisType cost = input->getDisof2(v,w)
				+ input->getDisof2(w,vj)
				- input->getDisof2(v,vj);

			Position posTemp(i,v, rPtw + rPc,cost);
			//posTemp.year = year;
			//posTemp.secDis = abs(input->datas[w].polarAngle - input->datas[v].polarAngle);

			updatePool(posTemp);

		}
	}

	if (ret.cost > input->getDisof2(0,w) + input->getDisof2(w, 0)) {
		return Position();
	}
	return ret;
}

void Solver::initSolutionBySecOrder() {

	Vector<int>que1(input->customerNumer);
	std::iota(que1.begin(), que1.end(), 1);

	auto cmp0 = [&](int x, int y) {
		return input->datas[x].polarAngle < input->datas[y].polarAngle;
	};
	std::sort(que1.begin(), que1.end(), cmp0);

	int rid = 0;

	int indexBeg = random->pick(input->customerNumer);

	//int indexBeg = 100;
	// NOTE[0]:init��+inupt.customerNumer-1 is means reverse traversal
	int deltstep = input->customerNumer - 1;
	if (random->pick(2) == 0) {
		deltstep = 1;
	}

	for (int i = 0; i < input->customerNumer; ++i) {
		indexBeg += deltstep;
		int tp = que1[indexBeg % input->customerNumer];
		
		Position bestP = findBestPositionInSolutionForInitial(tp);
		//Position bestP = findBestPosForRuin(tp);

		if (bestP.rIndex != -1 && bestP.pen == 0) {
			rInsertAtPosition(rts[bestP.rIndex], bestP.pos, tp);
			rUpdateAvQFrom(rts[bestP.rIndex], tp);
			rUpdateZvQFrom(rts[bestP.rIndex], tp);
		}
		else {
			Route r1 = rCreateRoute(rid++);
			rInsAtPosPre(r1, r1.tail, tp);
			rUpdateAvQFrom(r1, tp);
			rUpdateZvQFrom(r1, tp);
			rts.push_back(r1);
		}
	}

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		rUpdateZvQFrom(r, r.tail);
		rUpdateAvQFrom(r, r.head);
		rReCalculateRouteCost(r);
	}
	sumRoutesPenalty();
}

void Solver::initSolutionSortOrder(int kind) {

	Vector<int>que1(input->customerNumer);
	std::iota(que1.begin(), que1.end(), 1);

	auto cmp1 = [&](int x, int y) {
		return input->getDisof2(0,x) > input->getDisof2(0,y);
	};

	auto cmp2 = [&](int x, int y) {
		return input->datas[x].READYTIME > input->datas[y].READYTIME;
	};

	auto cmp3 = [&](int x, int y) {
		return input->datas[x].DUEDATE < input->datas[y].DUEDATE;
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
		Logger::ERROR("no this kind of initMaxRoute");
	}

	int rid = 0;
	for (int tp : que1) {

		//Position bestP = findBestPosForRuin(tp);
		Position bestP = findBestPositionInSolutionForInitial(tp);
		//Position bestP = findBestPosInSol(tp);

		if (bestP.rIndex != -1 && bestP.pen == 0) {
			rInsertAtPosition(rts[bestP.rIndex], bestP.pos, tp);
			rUpdateAvQFrom(rts[bestP.rIndex], tp);
			rUpdateZvQFrom(rts[bestP.rIndex], tp);
		}
		else {
			Route r1 = rCreateRoute(rid++);
			rInsAtPosPre(r1, r1.tail, tp);
			rUpdateAvQFrom(r1, r1.head);
			rUpdateZvQFrom(r1, r1.tail);
			rts.push_back(r1);
		}
		//saveOutAsSintefFile("k_" + std::to_string(kind));
	}

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);
	}
	sumRoutesPenalty();
}

void Solver::initSolutionMaxRoute() {

	Vector<int>que1(input->customerNumer);
	std::iota(que1.begin(), que1.end(), 1);
	
	//OPT[init max route]:can shuffle
	//random->shuffleVec(que1);

	auto cmp = [&](int x, int y) {
		return input->datas[x].polarAngle < input->datas[y].polarAngle;
	};
	std::sort(que1.begin(), que1.end(), cmp);

	int rid = 0;

	do {

		int tp = -1;
		Position bestP;
		bool isSucceed = false;
		int eaIndex = -1;

		for (int i = 0; i < static_cast<int>(que1.size()); ++i) {
			int cus = que1[i];
			//Position tPos = findBestPosForRuin(cus);
			Position tPos = findBestPositionInSolutionForInitial(cus);
			//Position tPos = findBestPosInSol(cus);

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
			rInsertAtPosition(rts[bestP.rIndex], bestP.pos, tp);
			rUpdateAvQFrom(rts[bestP.rIndex], rts[bestP.rIndex].head);
			rUpdateZvQFrom(rts[bestP.rIndex], rts[bestP.rIndex].tail);
			continue;
		}

		Route r1 = rCreateRoute(rid++);
		rInsAtPosPre(r1, r1.tail, que1[0]);
		que1.erase(que1.begin());
		rUpdateAvQFrom(r1, r1.head);
		rUpdateZvQFrom(r1, r1.tail);
		rts.push_back(r1);

	} while (!que1.empty());

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];

		rUpdateZvQFrom(r, r.tail);
		rUpdateAvQFrom(r, r.head);
		rReCalculateRouteCost(r);
	}
	sumRoutesPenalty();
}

void Solver::initSolutionRandomly() {

	rts.reset();

	Vector<int> orderCustomers(input->customerNumer);
	std::iota(orderCustomers.begin(), orderCustomers.end(), 1);
	
	//OPT[-1]:random order
	random->shuffleVec(orderCustomers);

	int routeId = 0;

	Route routeNew = rCreateRoute(routeId++);
	rts.push_back(routeNew);

	for(int c: orderCustomers) {

		auto& r = rts.getRouteByRouteId(routeId-1);

		rInsAtPosPre(r, r.tail, c);
		rUpdateAvQFrom(r, r.head);
		if (r.rPc == 0 && r.rPtw == 0) {
			;
		}
		else {
			rRemoveAtPosition(r,c);
			Route routeNew = rCreateRoute(routeId++);
			rts.push_back(routeNew);
			auto& r = rts.getRouteByRouteId(routeId-1);
			rInsAtPosPre(r, r.tail, c);
		}
	}
}

void Solver::initSolution(int kind) {//  find kind to init solution

	rts.reset();

	if (kind == 0) {
		initSolutionBySecOrder();
	}
	else if(kind <= 3){
		initSolutionSortOrder(kind);
	}
	else if (kind == 4) {
		initSolutionRandomly();
	}
	else if (kind == 5) {
		initSolutionMaxRoute();
	}
	
	else {
		Logger::ERROR("no this kind of init");
	}
	reCalRtsCostAndPen();

}

DeltaPenalty Solver::estimatevw(int kind, int v, int w, int oneR) {
	
	switch (kind) {
	case 0:return twoOptStarOpenvv_(v, w);
	case 1:return twoOptStarOpenvvj(v, w);
	case 2:return outOnevToww_(v, w, oneR);
	case 3:return outOnevTowwj(v, w, oneR);
	case 4:return inOnevv_(v, w, oneR);
	case 5:return inOnevvj(v, w, oneR);
	case 6:return swapOneOnevw_(v, w, oneR);
	case 7:return swapOneOnevwj(v, w, oneR);
	case 8:return swapOneOnevw(v, w, oneR);
	case 9:return swapThreeTwovvjvjjwwj(v, w, oneR);
	case 10:return swapThreeOnevvjvjjw(v, w, oneR);
	case 11:return swapTwoOnevvjw(v, w, oneR);
	case 12:return swapOneTwovwwj(v, w, oneR);
	case 13:return outTwovvjTowwj(v, w, oneR);
	case 14:return outTwovv_Toww_(v, w, oneR);
	case 15: return reversevw(v, w);
	default:
		Logger::ERROR("estimate no this kind move");
		break;
	}
	DeltaPenalty bestM;
	return bestM;
}

//open interval(twbegin��twend) 
// twbegin��twend the value is correct 
// twbegin��twend are allowed be depot 
DisType Solver::getaRangeOffPtw(int twbegin, int twend) {

	DisType newwvPtw = customers[twbegin].TW_X;
	newwvPtw += customers[twend].TWX_;

	DisType lastav = customers[twbegin].av;
	int ptpre = twbegin;
	int pt = customers[ptpre].next;

	for (; pt != -1;) {
		DisType avp = lastav + input->datas[ptpre].SERVICETIME + input->getDisof2(ptpre, pt);
		newwvPtw += std::max<DisType>(0, avp - input->datas[pt].DUEDATE);
		lastav = avp > input->datas[pt].DUEDATE ? input->datas[pt].DUEDATE :
			std::max<DisType>(avp, input->datas[pt].READYTIME);
		if (pt == twend) {
			break;
		}
		ptpre = pt;
		pt = customers[pt].next;
	}

	lyhCheckTrue(pt == twend);

	newwvPtw += std::max<DisType>(0, lastav - customers[twend].zv);
	return newwvPtw;
}

DeltaPenalty Solver::twoOptStarOpenvv_(int v, int w) { //0
	//debug(0)

	DeltaPenalty bestM;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (rv.routeId == rw.routeId) {
		return bestM;
	}

	int v_ = customers[v].prev;
	int wj = customers[w].next;

	if (v_ > input->customerNumer && wj > input->customerNumer) {
		return bestM;
	}

	auto getDeltPtw = [&]()->void {

		DisType vPtw = rv.rPtw * rv.rWeight;
		DisType wPtw = rw.rPtw * rw.rWeight;

		DisType newwvPtw = getPtwWithVectorOfCustomersArg(2,w,v);
		// (v-) -> (w+)
		DisType newv_wjPtw = getPtwWithVectorOfCustomersArg(2, v_,wj );

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

		bestM.deltPc = std::max<DisType>(0, rwQ - input->vehicleCapacity) +
			std::max<DisType>(0, rvQ - input->vehicleCapacity) -
			rv.rPc - rw.rPc;
		bestM.PcOnly = bestM.deltPc;
		bestM.deltPc *= beta;
	};

	auto getRcost = [&]() {

		DisType delt = 0;
		delt -= input->getDisof2(v_,v);
		delt -= input->getDisof2(w,wj);

		delt += input->getDisof2(w,v);
		delt += input->getDisof2(v_,wj);
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

	//if (bestM.deltPtw == DisInf) {
	//	Logger::INFO(11111);
	//}
	return bestM;
}

DeltaPenalty Solver::twoOptStarOpenvvj(int v, int w) { //1

	DeltaPenalty bestM;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (rv.routeId == rw.routeId) {
		return bestM;
	}

	int vj = customers[v].next;
	int w_ = customers[w].prev;

	if (vj > input->customerNumer && w_ > input->customerNumer) {
		return bestM;
	}

	auto getDeltPtw = [&]()->void {

		DisType vPtw = rv.rPtw * rv.rWeight;
		DisType wPtw = rw.rPtw * rw.rWeight;

		// v->w
		DisType newwvPtw = getPtwWithVectorOfCustomersArg(2, v,w);

		// (w-) -> (v+)
		DisType neww_vjPtw = getPtwWithVectorOfCustomersArg(2,w_,vj);

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

		bestM.deltPc = std::max<DisType>(0, rvwQ - input->vehicleCapacity)
			+ std::max<DisType>(0, rw_vjQ - input->vehicleCapacity)
			- rv.rPc - rw.rPc;
		bestM.PcOnly = bestM.deltPc;
		bestM.deltPc *= beta;
	};

	auto getRcost = [&]() {

		DisType delt = 0;
		delt -= input->getDisof2(v,vj);
		delt -= input->getDisof2(w_,w);

		delt += input->getDisof2(v,w);
		delt += input->getDisof2(w_,vj);

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

DeltaPenalty Solver::outOnevToww_(int v, int w, int oneR) { //2

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	DeltaPenalty bestM;

	int v_ = customers[v].prev;
	int vj = customers[v].next;
	int w_ = customers[w].prev;

	if (v_ > input->customerNumer && vj > input->customerNumer) {
		return bestM;
	}

	if (rw.routeId == rv.routeId) {

		if (oneR == 0) {
			return bestM;
		}

        if (v == w || v == w_) {
            return bestM;
        }
	}

	auto getDeltPtw = [&]()->void {

		DisType vPtw = rv.rPtw * rv.rWeight;
		DisType wPtw = rw.rPtw * rw.rWeight;

		DisType newv_vjPtw = 0;
		DisType newvwPtw = 0;

		if (rw.routeId == rv.routeId) {

            int frontCustomer = getFrontofTwoCus(v, w);
            //int back = (front == v ? w : v);

            lyhCheckTrue(frontCustomer == v || frontCustomer == w);
            //OutOnevToww_
            int twbegin = -1, twend = -1;
            if (frontCustomer == v) {
                twbegin = v_;
                twend = w;
            }
            else {
                twbegin = w_;
                twend = vj;
            }

            rRemoveAtPosition(rv, v);
            rInsertAtPosition(rv, w_, v);

            newvwPtw = getaRangeOffPtw(twbegin, twend);

            rRemoveAtPosition(rv, v);
            rInsertAtPosition(rv, v_, v);

			bestM.PtwOnly = newvwPtw - rw.rPtw;
			bestM.deltPtw = (newvwPtw - rw.rPtw) * rw.rWeight * alpha;

		}
		else {
			
			newvwPtw = getPtwWithVectorOfCustomersArg(3, w_,v,w );
			newv_vjPtw = getPtwWithVectorOfCustomersArg(2, v_,vj );

			bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;
		}
		return;
	};

	auto getDeltPc = [&]() {

		if (rv.routeId == rw.routeId) {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {
			bestM.deltPc = std::max<DisType>(0, rw.rQ + input->datas[v].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, rv.rQ - input->datas[v].DEMAND - input->vehicleCapacity)
				- rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// inset v to w and (w-)

		DisType delt = 0;
		delt -= input->getDisof2(v,vj);
		delt -= input->getDisof2(v_,v);
		delt -= input->getDisof2(w_,w);

		delt += input->getDisof2(w_,v);
		delt += input->getDisof2(v,w);
		delt += input->getDisof2(v_,vj);

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

DeltaPenalty Solver::outOnevTowwj(int v, int w, int oneR) { //3

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	DeltaPenalty bestM;

	int v_ = customers[v].prev;
	int vj = customers[v].next;
	int wj = customers[w].next;

	if (v_ > input->customerNumer && vj > input->customerNumer) {
		return bestM;
	}

	if (rw.routeId == rv.routeId) {

		if (oneR == 0) {
			return bestM;
		}

        if (v == w || v == wj) {
            return bestM;
        }
	}

	auto getDeltPtw = [&]()->void {

		DisType vPtw = rv.rPtw * rv.rWeight;
		DisType wPtw = rw.rPtw * rw.rWeight;
		DisType newv_vjPtw = 0;
		DisType newvwPtw = 0;

		if (rw.routeId == rv.routeId) {

            int frontCustomer = getFrontofTwoCus(v, w);
            // OutOnevTowwj
            int twbegin = -1, twend = -1;
            if (frontCustomer == v) {
                twbegin = v_;
                twend = wj;
            }
            else {
                twbegin = w;
                twend = vj;
            }

            rRemoveAtPosition(rv, v);
            rInsertAtPosition(rv, w, v);

            newvwPtw = getaRangeOffPtw(twbegin, twend);

            rRemoveAtPosition(rv, v);
            rInsertAtPosition(rv, v_, v);

			bestM.PtwOnly = newvwPtw - rw.rPtw;
			bestM.deltPtw = (newvwPtw - rw.rPtw) * rw.rWeight * alpha;

		}
		else {

			// insert v to (w,wj)
			newvwPtw += getPtwWithVectorOfCustomersArg(3, w,v,wj );
			newv_vjPtw += getPtwWithVectorOfCustomersArg(2, v_,vj );

			bestM.PtwOnly = newvwPtw + newv_vjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newvwPtw * rw.rWeight + newv_vjPtw * rv.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;
		}
		return;
	};
	// insert v to w and (w+)

	auto getDeltPc = [&]() {

		if (rv.routeId == rw.routeId) {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {
			bestM.deltPc = std::max<DisType>(0, rw.rQ + input->datas[v].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, rv.rQ - input->datas[v].DEMAND - input->vehicleCapacity)
				- rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// inset v to w and (w+)
		DisType delt = 0;
		delt -= input->getDisof2(v,vj);
		delt -= input->getDisof2(v_,v);
		delt -= input->getDisof2(w,wj);

		delt += input->getDisof2(w,v);
		delt += input->getDisof2(v,wj);
		delt += input->getDisof2(v_,vj);

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

DeltaPenalty Solver::inOnevv_(int v, int w, int oneR) { //4

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	DeltaPenalty bestM;

	int w_ = customers[w].prev;
	int wj = customers[w].next;
	int v_ = customers[v].prev;

	if (w_ > input->customerNumer && wj > input->customerNumer) {
		return bestM;
	}

	// insert w to v and (v-)
	if (rw.routeId == rv.routeId) {
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
		if (rw.routeId == rv.routeId) {

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

			rRemoveAtPosition(rw, w);
			rInsertAtPosition(rw, v_, w);

			//Logger::INFO("twbegin:", twbegin);
			//Logger::INFO("twend:", twend);
			
			newwvPtw = getaRangeOffPtw(twbegin, twend);
			//newwvPtw = rUpdateAvfrom(rw, rw.head);

			rRemoveAtPosition(rw, w);
			rInsertAtPosition(rw, w_, w);

			//rUpdateAvfrom(rw, rw.head);

			bestM.PtwOnly = newwvPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {

			// insert w to v and (v-)
			newwvPtw = getPtwWithVectorOfCustomersArg(3, v_, w,v );
			// insert w to (v,v-)
			neww_wjPtw = getPtwWithVectorOfCustomersArg(2, w_,w );

			bestM.PtwOnly = newwvPtw + neww_wjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rv.rWeight + neww_wjPtw * rw.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;

		}

	};

	auto getDeltPc = [&]() {

		if (rv.routeId == rw.routeId) {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {
			bestM.deltPc = std::max<DisType>(0, rw.rQ - input->datas[w].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, rv.rQ + input->datas[w].DEMAND - input->vehicleCapacity)
				- rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}

	};

	auto getRcost = [&]() {

		// inset w to v and (v-)
		DisType delt = 0;
		delt -= input->getDisof2(v_,v);
		delt -= input->getDisof2(w_,w);
		delt -= input->getDisof2(w,wj);

		delt += input->getDisof2(v_, w);
		delt += input->getDisof2(w,v);
		delt += input->getDisof2(w_,wj);

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

DeltaPenalty Solver::inOnevvj(int v, int w, int oneR) { // 5

	// insert w to (v,v+)
	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	DeltaPenalty bestM;

	int w_ = customers[w].prev;
	int wj = customers[w].next;
	int vj = customers[v].next;

	if (w_ > input->customerNumer && wj > input->customerNumer) {
		return bestM;
	}

	if (rw.routeId == rv.routeId) {
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
		if (rw.routeId == rv.routeId) {

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

			rRemoveAtPosition(rw, w);
			rInsertAtPosition(rw, v, w);

			newwvPtw = getaRangeOffPtw(twbegin, twend);

			rRemoveAtPosition(rw, w);
			rInsertAtPosition(rw, w_, w);

			bestM.PtwOnly = newwvPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;

		}
		else {

			// insert w to (v,v+)
			newwvPtw = getPtwWithVectorOfCustomersArg(3,v,w,vj);
			neww_wjPtw = getPtwWithVectorOfCustomersArg(2, w_,wj );

			bestM.PtwOnly = newwvPtw + neww_wjPtw - rv.rPtw - rw.rPtw;
			bestM.deltPtw = newwvPtw * rv.rWeight + neww_wjPtw * rw.rWeight - vPtw - wPtw;
			bestM.deltPtw *= alpha;
		}
	};

	auto getDeltPc = [&]() {

		if (rv.routeId == rw.routeId) {
			bestM.PcOnly = 0;
			bestM.deltPc = 0;
		}
		else {
			// insert w to (v,v+)
			bestM.deltPc = std::max<DisType>(0, rw.rQ - input->datas[w].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, rv.rQ + input->datas[w].DEMAND - input->vehicleCapacity)
				- rv.rPc - rw.rPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// insert w to (v,v+)
		DisType delt = 0;
		delt -= input->getDisof2(v,vj);
		delt -= input->getDisof2(w_,w);
		delt -= input->getDisof2(w,wj);

		delt += input->getDisof2(v,w);
		delt += input->getDisof2(w,vj);
		delt += input->getDisof2(w_,wj);

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

DeltaPenalty Solver::swapOneOnevw(int v, int w, int oneR) { // 8

	DeltaPenalty bestM;
	// exchange v and (w)
	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	int wj = customers[w].next;
	int v_ = customers[v].prev;
	int vj = customers[v].next;
	int w_ = customers[w].prev;

	if (rv.routeId == rw.routeId) {

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

		if (rv.routeId == rw.routeId) {

			if (v == w) {
				return;
			}

			if (v_ == w) {
				// (v--) (v-) (v) (v+) ===> (v--) v (v-) vj
				int v__ = customers[v_].prev;
				newwPtw = newvPtw = getPtwWithVectorOfCustomersArg(4, v__,v,v_,vj );
			}
			else if (w_ == v) {
				// (w--)->(w-)->(w)->(w+) ===> w__,w,w_,wj
				int w__ = customers[w_].prev;
				newwPtw = newvPtw = getPtwWithVectorOfCustomersArg(4, w__,w,w_,wj );
			}
			else {
				
				// swapOneOnevw
				int fr = getFrontofTwoCus(v, w);

				// swapOneOnevw
				rRemoveAtPosition(rw, w);
				rRemoveAtPosition(rw, v);

				rInsertAtPosition(rw, v_, w);
				rInsertAtPosition(rw, w_, v);

				if (fr == v) {
					newwPtw = newvPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newwPtw = newvPtw = getaRangeOffPtw(w_, vj);
				}

				rRemoveAtPosition(rw, w);
				rRemoveAtPosition(rw, v);

				rInsertAtPosition(rw, w_, w);
				rInsertAtPosition(rw, v_, v);
			}

			bestM.PtwOnly = newwPtw - rw.rPtw;
			bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {

			newvPtw = getPtwWithVectorOfCustomersArg(3, v_,w,vj );
			newwPtw += getPtwWithVectorOfCustomersArg(3,w_,v,wj );

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

		if (rv.routeId == rw.routeId) {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {

			bestM.deltPc = std::max<DisType>(0, wQ - input->datas[w].DEMAND + input->datas[v].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, vQ - input->datas[v].DEMAND + input->datas[w].DEMAND - input->vehicleCapacity)
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

			delt -= input->getDisof2(v,vj);
			delt -= input->getDisof2(w_,w);
			delt -= input->getDisof2(w,v);

			delt += input->getDisof2(v,w);
			delt += input->getDisof2(w_,v);
			delt += input->getDisof2(w,vj);
		}
		else if (w_ == v) {

			// v w
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(v,w);

			delt += input->getDisof2(w,v);
			delt += input->getDisof2(v,wj);
			delt += input->getDisof2(v_,w);
		}
		else {

			delt -= input->getDisof2(v,vj);
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(w_,w);

			delt += input->getDisof2(w_, v);
			delt += input->getDisof2(v,wj);
			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(w,vj);

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

DeltaPenalty Solver::swapOneOnevw_(int v, int w, int oneR) { // 6

	// exchange v and (w_)
	DeltaPenalty bestM;

	int w_ = customers[w].prev;

	if (w_ > input->customerNumer || v == w_) {
		return bestM;
	}
	return swapOneOnevw(v, w_, oneR);

}

DeltaPenalty Solver::swapOneOnevwj(int v, int w, int oneR) { // 7

	// exchange v and (w+)

	DeltaPenalty bestM;

	int wj = customers[w].next;

	if (wj > input->customerNumer || v == wj) {
		return bestM;
	}
	return swapOneOnevw(v, wj, oneR);
}

DeltaPenalty Solver::swapTwoOnevvjw(int v, int w, int oneR) { // 11 swap(2,1)

	// exchange vvj and (w)

	DeltaPenalty bestM;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	int wj = customers[w].next;
	int vj = customers[v].next;

	if (vj > input->customerNumer) {
		return bestM;
	}

	int vjj = customers[vj].next;

	int v_ = customers[v].prev;
	int w_ = customers[w].prev;

	if (rv.routeId == rw.routeId) {
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

		if (rv.routeId == rw.routeId) {

			if (v_ == w) {
				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(5,w_,v,vj,w,vjj );
			}
			else if (w_ == vj) {

				// exchange vvj and (w)
				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(5, v_,w,v,vj,wj);
			}
			else {

				// exchange vvj and (w)
				int fr = getFrontofTwoCus(v, w);

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rw, w);

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, vjj);
				}

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rv, w);

				rInsertAtPosition(rv, v_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, w_, w);
			}
			bestM.PtwOnly = newwPtw - rw.rPtw;
			bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {

			// (v-)->(w)->(vjj)
			newvPtw = getPtwWithVectorOfCustomersArg(3, v_,w,vjj );
			// (w-) -> (v) -> (v+) -> (wj)
			newwPtw = getPtwWithVectorOfCustomersArg(4,w_,v,vj,wj);
			
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

		if (rv.routeId == rw.routeId) {

			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {

			bestM.deltPc =
				std::max<DisType>(0, wQ - input->datas[w].DEMAND + input->datas[v].DEMAND + input->datas[vj].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, vQ - input->datas[v].DEMAND - input->datas[vj].DEMAND + input->datas[w].DEMAND - input->vehicleCapacity)
				- vPc - wPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// exchange vvj and (w)
		DisType delt = 0;
		if (v_ == w) {
			// w - (v vj)
			delt -= input->getDisof2(w,v);
			delt -= input->getDisof2(w_,w);
			delt -= input->getDisof2(vj,vjj);
			//w v vj -> v vj w
			delt += input->getDisof2(vj,w);
			delt += input->getDisof2(w,vjj);
			delt += input->getDisof2(w_,v);
		}
		else if (w_ == vj) {
			// (v vj) w w+ =>
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(vj,w);

			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(vj, wj);
			delt += input->getDisof2(w,v);
		}
		else {
			// (v vj)  w_ (w) w+ =>
			delt -= input->getDisof2(vj,vjj);
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(w_,w);

			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(w,vjj);
			delt += input->getDisof2(w_, v);
			delt += input->getDisof2(vj,wj);
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

DeltaPenalty Solver::swapOneTwovwwj(int v, int w, int oneR) { // 12 swap(1,2)

	// exchange v and (ww+)
	DeltaPenalty bestM;

	int wj = customers[w].next;

	if (wj > input->customerNumer) {
		return bestM;
	}
	return swapTwoOnevvjw(w, v, oneR);

}

DeltaPenalty Solver::swapThreeTwovvjvjjwwj(int v, int w, int oneR) { // 9 swap(3,2)

	// exchange vvjvjj and (ww+)
	DeltaPenalty bestM;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	int wj = customers[w].next;
	int vj = customers[v].next;

	if (vj > input->customerNumer || wj > input->customerNumer) {
		return bestM;
	}
	int vjj = customers[vj].next;

	if (vjj > input->customerNumer) {
		return bestM;
	}

	int v3j = customers[vjj].next;

	int wjj = customers[wj].next;

	int v_ = customers[v].prev;
	int w_ = customers[w].prev;

	//exchange vvjvjj and (ww + )
	if (rw.routeId == rv.routeId) {
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

		if (rv.routeId == rw.routeId) {
			//exchange vvjvjj and (ww + )
			if (v_ == wj) {
				//w_ w wj v vj vjj v3j ==> w_,  v,vj,vjj , w,wj, v3j
 				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(7, w_,v,vj,vjj, w,wj, v3j);
			}
			else if (w_ == vjj) {
				//exchange vvjvjj and (ww + )
				// v_ v vj vjj/w_ w wj wjj ===> 
				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(7, v_, w,wj, v,vj,vjj,wjj);
			}
			else {

				//exchange vvjvjj and (ww + )
				int fr = getFrontofTwoCus(v, w);

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rv, vjj);
				rRemoveAtPosition(rw, w);
				rRemoveAtPosition(rw, wj);

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, wj);

				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wjj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);
				}

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rv, vjj);
				rRemoveAtPosition(rv, w);
				rRemoveAtPosition(rv, wj);

				rInsertAtPosition(rv, v_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);

				rInsertAtPosition(rv, w_, w);
				rInsertAtPosition(rv, w, wj);

			}

			bestM.PtwOnly = newwPtw - rw.rPtw;
			bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {
			//v vj vjj
			// (v-)->(w)->(w+)->(v3j)
			newvPtw = getPtwWithVectorOfCustomersArg(4, v_,w,wj,v3j);
			// (w-) -> (v) -> (vj) -> (vjj)-> (wjj)
			newwPtw += getPtwWithVectorOfCustomersArg(5, w_,v,vj,vjj,wjj);
			
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

		if (rv.routeId == rw.routeId) {
			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {
			bestM.deltPc =
				std::max<DisType>(0, wQ - input->datas[w].DEMAND - input->datas[wj].DEMAND + input->datas[v].DEMAND + input->datas[vj].DEMAND + input->datas[vjj].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, vQ - input->datas[v].DEMAND - input->datas[vj].DEMAND - input->datas[vjj].DEMAND + input->datas[w].DEMAND + input->datas[wj].DEMAND - input->vehicleCapacity)
				- vPc - wPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// exchange v vj vjj and (ww+)
		DisType delt = 0;

		if (v3j == w) {
			delt -= input->getDisof2(vjj,w);
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(wj,wjj);

			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(wj,v);
			delt += input->getDisof2(vjj,wjj);
		}
		else if (wj == v_) {
			// (ww+) and v vj vjj and 

			delt -= input->getDisof2(w_,w);
			delt -= input->getDisof2(vjj,v3j);
			delt -= input->getDisof2(wj,v);

			delt += input->getDisof2(vjj,w);
			delt += input->getDisof2(w_,v);
			delt += input->getDisof2(wj,v3j);
		}
		else {
			delt -= input->getDisof2(vjj,v3j);
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(wj,wjj);
			delt -= input->getDisof2(w_,w);

			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(wj,v3j);
			delt += input->getDisof2(w_,v);
			delt += input->getDisof2(vjj,wjj);
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

DeltaPenalty Solver::swapThreeOnevvjvjjw(int v, int w, int oneR) { // 10 swap(3,1)

	// exchange vvjvjj and (w)
	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	DeltaPenalty bestM;

	int wj = customers[w].next;
	int vj = customers[v].next;

	if (vj > input->customerNumer) {
		return bestM;
	}
	int vjj = customers[vj].next;

	if (vjj > input->customerNumer) {
		return bestM;
	}
	int v3j = customers[vjj].next;

	int v_ = customers[v].prev;
	int w_ = customers[w].prev;

	if (rv.routeId == rw.routeId) {

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

		if (rv.routeId == rw.routeId) {

			//return bestM;
			// exchange vvjvjj and (w)
			 // w_ (w) (v vj vjj)  v3j
			if (v == wj) {
				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(6, w_, v,vj,vjj,w ,v3j );
			}
			else if (w_ == vjj) {
				// v_ (v vj vjj/w_) (w) wj
				newvPtw = newwPtw = getPtwWithVectorOfCustomersArg(6, v_ , w, v,vj,vjj, wj);
			}
			else {

				int fr = getFrontofTwoCus(v, w);

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rv, vjj);
				rRemoveAtPosition(rw, w);

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);

				if (fr == v) {
					newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
				}
				else {
					newvPtw = newwPtw = getaRangeOffPtw(w_, v3j);
				}

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rv, vj);
				rRemoveAtPosition(rv, vjj);
				rRemoveAtPosition(rv, w);

				rInsertAtPosition(rv, v_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);
				rInsertAtPosition(rv, w_, w);
			}

			bestM.PtwOnly = newwPtw - rw.rPtw;
			bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {
			//v vj vjj
			// (v-)->(w)->(v3j)
			newvPtw = getPtwWithVectorOfCustomersArg(3, v_,w,v3j );
			// (w-) -> (v) -> (vj) -> (vjj)-> (wj)
			newwPtw = getPtwWithVectorOfCustomersArg(5, w_,v,vj,vjj,wj );
			
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

		if (rv.routeId == rw.routeId) {

			bestM.deltPc = 0;
			bestM.PcOnly = 0;

		}
		else {

			bestM.deltPc =
				std::max<DisType>(0, wQ - input->datas[w].DEMAND + input->datas[v].DEMAND + input->datas[vj].DEMAND + input->datas[vjj].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, vQ - input->datas[v].DEMAND - input->datas[vj].DEMAND - input->datas[vjj].DEMAND + input->datas[w].DEMAND - input->vehicleCapacity)
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
			delt -= input->getDisof2(w,v);
			delt -= input->getDisof2(w_,w);
			delt -= input->getDisof2(vjj,v3j);

			delt += input->getDisof2(vjj,w);
			delt += input->getDisof2(w,v3j);
			delt += input->getDisof2(w_,v);
		}
		else if (w_ == vjj) {

			//v vj vjj w -> w v vj vjj 
			delt -= input->getDisof2(vjj,w);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(v_,v);

			delt += input->getDisof2(w,v);
			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(vjj,wj);
		}
		else {
			delt -= input->getDisof2(vjj,v3j);
			delt -= input->getDisof2(v_,v);
			delt -= input->getDisof2(w,wj);
			delt -= input->getDisof2(w_,w);

			delt += input->getDisof2(v_,w);
			delt += input->getDisof2(w,v3j);
			delt += input->getDisof2(w_,v);
			delt += input->getDisof2(vjj,wj);
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

DeltaPenalty Solver::outTwovvjTowwj(int v, int w, int oneR) {  //13 out two

	DeltaPenalty bestM;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	int v_ = customers[v].prev;
	int vj = customers[v].next;

	if (vj > input->customerNumer || vj == w) {
		return bestM;
	}

	int vjj = customers[vj].next;
	int wj = customers[w].next;

	if (v_ > input->customerNumer && vjj > input->customerNumer) {
		return bestM;
	}
	if (wj == v) {
		return bestM;
	}

	if (rw.routeId == rv.routeId) {
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
		if (rw.routeId == rv.routeId) {

			//return bestM;
			if (v == w || vj == w) {
				return;
			}

			int fr = getFrontofTwoCus(v, w);

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);

			rInsertAtPosition(rv, w, v);
			rInsertAtPosition(rv, v, vj);

			if (fr == v) {
				newvPtw = newwPtw = getaRangeOffPtw(v_, wj);
			}
			else {
				newvPtw = newwPtw = getaRangeOffPtw(w, vjj);
			}

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);

			rInsertAtPosition(rv, v_, v);
			rInsertAtPosition(rv, v, vj);

			bestM.PtwOnly = newwPtw - rw.rPtw;
			bestM.deltPtw = newwPtw * rw.rWeight - wPtw;
			bestM.deltPtw *= alpha;
		}
		else {

			// insert (v,vj) to between w and wj
			// w -> v -> vj -> (wj)
			newwPtw = getPtwWithVectorOfCustomersArg(4, w,v,vj,wj );
			// link v- and vjj
			newvPtw = getPtwWithVectorOfCustomersArg(2, v_,vjj);
			
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

		if (rv.routeId == rw.routeId) {

			bestM.deltPc = 0;
			bestM.PcOnly = 0;
		}
		else {

			bestM.deltPc =
				std::max<DisType>(0, wQ + input->datas[v].DEMAND + input->datas[vj].DEMAND - input->vehicleCapacity)
				+ std::max<DisType>(0, vQ - input->datas[v].DEMAND - input->datas[vj].DEMAND - input->vehicleCapacity)
				- vPc - wPc;
			bestM.PcOnly = bestM.deltPc;
			bestM.deltPc *= beta;
		}
	};

	auto getRcost = [&]() {

		// outrelocate v vj To w wj
		DisType delt = 0;
		delt -= input->getDisof2(vj,vjj);
		delt -= input->getDisof2(v_,v);
		delt -= input->getDisof2(w,wj);

		delt += input->getDisof2(w,v);
		delt += input->getDisof2(vj,wj);
		delt += input->getDisof2(v_,vjj);

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

DeltaPenalty Solver::outTwovv_Toww_(int v, int w, int oneR) {  //14 out two

	DeltaPenalty bestM;

	int v_ = customers[v].prev;
	if (v_ > input->customerNumer) {
		return bestM;
	}

	/*Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);*/

	int w_ = customers[w].prev;
	int v__ = customers[v_].prev;
	int vj = customers[v].next;

	if (v__ > input->customerNumer && vj > input->customerNumer) {
		return bestM;
	}

	//v- v || w_ w
	return outTwovvjTowwj(v_, w_, oneR);

}

DeltaPenalty Solver::reversevw(int v, int w) {//15 reverse

	DeltaPenalty bestM;

	int vId = customers[v].routeId;
	int wId = customers[w].routeId;

	if (vId > input->customerNumer || wId > input->customerNumer || vId != wId || v == w) {
		return bestM;
	}

	Route& r = rts.getRouteByRouteId(customers[v].routeId);

	int front = getFrontofTwoCus(v, w);
	int back = (front == v ? w : v);

	int f_ = customers[front].prev;
	int bj = customers[back].next;

	auto getDeltPtw = [&]()->void {

		int fj = customers[front].next;
		if (fj == back) {
			bestM = swapOneOnevw(v, w, 1);
			return;
		}
		int fjj = customers[fj].next;
		if (fjj == back) {
			bestM = swapOneOnevw(v, w, 1);
			return;
		}

		DisType newPtw = 0;
		DisType lastav = 0;
		int lastv = 0;

		lastv = back;
		DisType lastavp = customers[f_].av + input->datas[f_].SERVICETIME + input->getDisof2(f_,back);
		newPtw += customers[f_].TW_X;
		newPtw += std::max<DisType>(0, lastavp - input->datas[lastv].DUEDATE);
		lastav = lastavp > input->datas[lastv].DUEDATE ? input->datas[lastv].DUEDATE :
			std::max<DisType>(lastavp, input->datas[lastv].READYTIME);

		int pt = customers[lastv].prev;
		while (pt != -1) {

			DisType aptp = lastav + input->datas[lastv].SERVICETIME + input->getDisof2(lastv,pt);
			newPtw += std::max<DisType>(0, aptp - input->datas[pt].DUEDATE);

			DisType apt = aptp > input->datas[pt].DUEDATE ? input->datas[pt].DUEDATE :
				std::max<DisType>(aptp, input->datas[pt].READYTIME);
			lastv = pt;
			lastav = apt;
			if (pt == front) {
				break;
			}
			pt = customers[pt].prev;
		}

		DisType abjp = lastav + input->datas[lastv].SERVICETIME + input->getDisof2(lastv,bj);
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

		delt -= input->getDisof2(f_, front);
		delt -= input->getDisof2(back, bj);
		
		delt += input->getDisof2(f_, back);
		delt += input->getDisof2(front, bj);

		for (int pt = front; pt !=-1 ; pt = customers[pt].next) {
			int ptnext = customers[pt].next;
			delt -= input->getDisof2(pt, ptnext);
			delt += input->getDisof2(ptnext, pt);
			if (ptnext == back) {
				break;
			}
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

DeltaPenalty Solver::nOptStar(Vector<int>& nodes) { //16 Nopt*

	DeltaPenalty bestM;

	DisType newPtwNoWei = 0;
	DisType newPtw = 0;
	DisType newPc = 0;

	auto linkvw = [&](int vt, int wt) {

		DisType newwvPtw = 0;
		newwvPtw += vt > 0 ? customers[vt].TW_X : 0;
		newwvPtw += wt > 0 ? customers[wt].TWX_ : 0;
		DisType awp = (vt > 0 ? customers[vt].av + input->datas[vt].SERVICETIME : 0)
			+ input->getDisof2(vt,wt);

		newwvPtw += std::max<DisType>(awp - (wt > 0 ? customers[wt].zv : input->datas[0].DUEDATE), 0);
		newPtwNoWei += newwvPtw;

		newPc += std::max<DisType>(0,
			(vt > 0 ? customers[vt].Q_X : 0) + (wt > 0 ? customers[wt].QX_ : 0) - input->vehicleCapacity);
		return newwvPtw;
	};

	DisType oldPtwNoWei = 0;
	DisType oldPtw = 0;
	DisType oldPc = 0;
	for (std::size_t i = 0; i < nodes.size(); i += 2) {
		int rId = customers[(nodes[i] == 0 ? nodes[i + 1] : nodes[i])].routeId;
		Route& r = rts.getRouteByRouteId(rId);
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
		doTwoOptStar(M); break;
	case 2:
	case 3:
	case 13:
	case 14:
		doOutRelocate(M); break;
	case 4:
	case 5:
		doInRelocate(M); break;
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
		doExchange(M); break;
	case 15:
		doReverse(M); break;
	default:
		Logger::ERROR("doNopt(M) error");
		return false;
		break;
	}
	return false;
}

bool Solver::doTwoOptStar(TwoNodeMove& M) {

	int v = M.v;
	int w = M.w;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (M.kind == 0) {

		int vpre = customers[v].prev;
		int wnext = customers[w].next;

		int pt = v;

		customers[w].next = v;
		customers[v].prev = w;

		customers[vpre].next = wnext;
		customers[wnext].prev = vpre;

		rv.rCustCnt = 0;
		pt = customers[rv.head].next;
		while (pt != -1) {
			++rv.rCustCnt;
			customers[pt].routeId = rv.routeId;
			pt = customers[pt].next;
		}
		rv.rCustCnt--;

		pt = customers[rw.head].next;
		rw.rCustCnt = 0;
		while (pt != -1) {
			++rw.rCustCnt;
			customers[pt].routeId = rw.routeId;
			pt = customers[pt].next;
		}
		rw.rCustCnt--;

		std::swap(rv.tail, rw.tail);

		rUpdateAvQFrom(rw, w);
		rUpdateZvQFrom(rw, v);

		rUpdateAvQFrom(rv, vpre);
		rUpdateZvQFrom(rv, wnext);

		return true;

	}
	else if (M.kind == 1) {

		int vnext = customers[v].next;
		int wpre = customers[w].prev;

		customers[v].next = w;
		customers[w].prev = v;
		customers[wpre].next = vnext;
		customers[vnext].prev = wpre;

		rv.rCustCnt = 0;
		int pt = customers[rv.head].next;
		while (pt != -1) {
			++rv.rCustCnt;
			customers[pt].routeId = rv.routeId;
			pt = customers[pt].next;
		}
		rv.rCustCnt--;

		rw.rCustCnt = 0;
		pt = customers[rw.head].next;
		while (pt != -1) {
			++rw.rCustCnt;
			customers[pt].routeId = rw.routeId;
			pt = customers[pt].next;
		}
		rw.rCustCnt--;

		std::swap(rv.tail, rw.tail);

		rUpdateAvQFrom(rv, v);
		rUpdateZvQFrom(rv, w);

		rUpdateAvQFrom(rw, wpre);
		rUpdateZvQFrom(rw, vnext);

		return true;

	}
	else {
		Logger::ERROR("no this two opt * move!");
		return false;
	}
	return false;
}

bool Solver::doOutRelocate(TwoNodeMove& M) {

	int v = M.v;
	int w = M.w;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (M.kind == 2) {

		int vpre = customers[v].prev;
		int vnext = customers[v].next;

		rRemoveAtPosition(rv, v);
		rInsertAtPosition(rw, customers[w].prev, v);

		if (rv.routeId == rw.routeId) {

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);

		}
		else {

			rUpdateAvQFrom(rv, vpre);
			rUpdateZvQFrom(rv, vnext);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);
		}
	}
	else if (M.kind == 3) {

		int vpre = customers[v].prev;
		int vnext = customers[v].next;

		rRemoveAtPosition(rv, v);
		rInsertAtPosition(rw, w, v);

		if (rv.routeId == rw.routeId) {

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {

			rUpdateAvQFrom(rv, vpre);
			rUpdateZvQFrom(rv, vnext);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);
		}
	}
	else if (M.kind == 13) {
		// outTwovvjTowwj
		int vj = customers[v].next;

		if (rw.routeId == rv.routeId) {

			if (v == w || vj == w) {
				return false;
			}

			rRemoveAtPosition(rw, v);
			rRemoveAtPosition(rw, vj);
			rInsertAtPosition(rw, w, v);
			rInsertAtPosition(rw, v, vj);

			rUpdateAvQFrom(rw, rw.head);
			rUpdateZvQFrom(rw, rw.tail);

		}
		else {
			// outTwovvjTowwj
			int vjj = customers[vj].next;
			int v_ = customers[v].prev;

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);

			rInsertAtPosition(rw, w, v);
			rInsertAtPosition(rw, v, vj);

			rUpdateAvQFrom(rv, v_);
			rUpdateZvQFrom(rv, vjj);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, vj);
		}
	}
	else if (M.kind == 14) {

		int v_ = customers[v].prev;
		int v__ = customers[v_].prev;
		int vj = customers[v].next;

		if (rw.routeId == rv.routeId) {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, v_);

			rInsAtPosPre(rv, w, v);
			rInsAtPosPre(rv, v, v_);

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, v_);

			rInsAtPosPre(rw, w, v);
			rInsAtPosPre(rw, v, v_);

			rUpdateAvQFrom(rv, v__);
			rUpdateZvQFrom(rv, vj);

			rUpdateAvQFrom(rw, v_);
			rUpdateZvQFrom(rw, v);
		}
	}
	else {
		Logger::ERROR("no this inrelocate move");
		return false;
	}
	return true;

	return true;
}

bool Solver::doInRelocate(TwoNodeMove& M) {

	int v = M.v;
	int w = M.w;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (M.kind == 4) {
		// inset w to (v-)->(v)

		int wnext = customers[w].next;
		int wpre = customers[w].prev;

		rRemoveAtPosition(rw, w);
		rInsertAtPosition(rv, customers[v].prev, w);

		if (rv.routeId == rw.routeId) {

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, w);

			rUpdateAvQFrom(rw, wpre);
			rUpdateZvQFrom(rw, wnext);

		}

	}
	else if (M.kind == 5) {

		int wnext = customers[w].next;
		int wpre = customers[w].prev;

		rRemoveAtPosition(rw, w);
		rInsertAtPosition(rv, v, w);

		if (rv.routeId == rw.routeId) {

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);

		}
		else {

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, w);

			rUpdateAvQFrom(rw, wpre);
			rUpdateZvQFrom(rw, wnext);
		}
	}
	return true;

}

bool Solver::doExchange(TwoNodeMove& M) {

	int v = M.v;
	int w = M.w;

	Route& rv = rts.getRouteByRouteId(customers[v].routeId);
	Route& rw = rts.getRouteByRouteId(customers[w].routeId);

	if (M.kind == 6) {
		// exchange v and (w_)
		int w_ = customers[w].prev;
		if (w_ > input->customerNumer || v == w_) {
			return false;
		}

		int w__ = customers[w_].prev;
		int v_ = customers[v].prev;

		if (rv.routeId == rw.routeId) {

			if (v == w__) {

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rw, w_);

				rInsertAtPosition(rv, v_, w_);
				rInsertAtPosition(rw, w_, v);
			}
			else {
				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rw, w_);

				rInsertAtPosition(rw, w__, v);

				// exchange v and (w_)
				if (w == v) {
					rInsertAtPosition(rv, v, w_);
				}
				else {
					rInsertAtPosition(rv, v_, w_);
				}
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);

		}
		else {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rw, w_);

			rInsertAtPosition(rw, w__, v);
			rInsertAtPosition(rv, v_, w_);

			rUpdateAvQFrom(rv, w_);
			rUpdateZvQFrom(rv, w_);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);

		}
		return true;
	}
	else if (M.kind == 7) {
		// exchange v and (w+)
		int wj = customers[w].next;
		/*if (wj > input->customerNumer || v == wj) {
			return false;
		}*/

		int wjj = customers[wj].next;
		int v_ = customers[v].prev;

		if (rv.routeId == rw.routeId) {
			if (v == wj) {
				return false;
			}
			else if (v == wjj) {

				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rw, wj);

				rInsertAtPosition(rv, w, v);
				rInsertAtPosition(rw, v, wj);
			}
			else {
				rRemoveAtPosition(rv, v);
				rRemoveAtPosition(rw, wj);

				rInsertAtPosition(rv, v_, wj);

				if (v == w) {
					rInsertAtPosition(rw, wj, v);
				}
				else {
					rInsertAtPosition(rw, w, v);
				}
			}
			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
			return true;
		}
		else {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rw, wj);

			rInsertAtPosition(rw, w, v);
			rInsertAtPosition(rv, v_, wj);

			rUpdateAvQFrom(rv, wj);
			rUpdateZvQFrom(rv, wj);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);

			return true;
		}
		return true;
	}
	else if (M.kind == 8) {

		int v_ = customers[v].prev;
		int w_ = customers[w].prev;

		rRemoveAtPosition(rv, v);
		rRemoveAtPosition(rw, w);

		if (rv.routeId == rw.routeId) {
			//return bestM;

			if (v_ == w) {
				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, w);
			}
			else if (w_ == v) {
				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, v);
			}
			else {
				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w_, v);
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {

			rInsertAtPosition(rv, v_, w);
			rInsertAtPosition(rw, w_, v);

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, w);
			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);

		}
	}
	else if (M.kind == 9) {

		//swapThreeTwovvjvjjwwj swap(3,2)
		int wj = customers[w].next;
		int vj = customers[v].next;
		int vjj = customers[vj].next;
		int v_ = customers[v].prev;
		int w_ = customers[w].prev;

		if (rv.routeId == rw.routeId) {

			/*if (v == w || v == wj || vj == w || vj == wj || vjj == w || vjj == wj) {
				return false;
			}*/

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rv, vjj);
			rRemoveAtPosition(rw, w);
			rRemoveAtPosition(rw, wj);

			if (v_ == wj) {

				rInsertAtPosition(rw, w_, v);
				rInsertAtPosition(rw, v, vj);
				rInsertAtPosition(rw, vj, vjj);

				rInsertAtPosition(rv, vjj, w);
				rInsertAtPosition(rv, w, wj);

			}
			else if (w_ == vjj) {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, wj);
				rInsertAtPosition(rv, wj, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);

			}
			else {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, wj);

				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);

		}
		else {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rv, vjj);
			rRemoveAtPosition(rw, w);
			rRemoveAtPosition(rw, wj);

			rInsertAtPosition(rv, v_, w);
			rInsertAtPosition(rv, w, wj);
			rInsertAtPosition(rw, w_, v);
			rInsertAtPosition(rw, v, vj);
			rInsertAtPosition(rw, vj, vjj);

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
			rUpdateAvQFrom(rw, rw.head);
			rUpdateZvQFrom(rw, rw.tail);
		}
	}
	else if (M.kind == 10) {

		//swapThreeOnevvjvjjw swap(3,1)
		int v_ = customers[v].prev;
		int vj = customers[v].next;
		int vjj = customers[vj].next;
		int wj = customers[w].next;
		int w_ = customers[w].prev;

		if (rv.routeId == rw.routeId) {

			// exchange vvjvjj and (w)
			/*if (v == w || vj == w || vjj == w) {
				return false;
			}*/

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rv, vjj);
			rRemoveAtPosition(rw, w);

			if (v == wj) {

				rInsertAtPosition(rw, w_, v);
				rInsertAtPosition(rw, v, vj);
				rInsertAtPosition(rw, vj, vjj);
				rInsertAtPosition(rv, vjj, w);

			}
			else if (w_ == vjj) {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);

			}
			else {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);
				rInsertAtPosition(rv, vj, vjj);
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rv, vjj);
			rRemoveAtPosition(rw, w);

			rInsertAtPosition(rv, v_, w);
			rInsertAtPosition(rw, w_, v);
			rInsertAtPosition(rw, v, vj);
			rInsertAtPosition(rw, vj, vjj);

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, w);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, vjj);
		}
	}
	else if (M.kind == 11) {
		//swapTwoOnevvjw swap(2,1)
		int v_ = customers[v].prev;
		int vj = customers[v].next;
		int w_ = customers[w].prev;

		if (rv.routeId == rw.routeId) {

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rw, w);

			if (v_ == w) {
				// w -> v -> v+
				rInsertAtPosition(rw, w_, v);
				rInsertAtPosition(rw, v, vj);
				rInsertAtPosition(rv, vj, w);
			}
			else if (w_ == vj) {
				// v -> (v+) -> w
				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, v);
				rInsertAtPosition(rv, v, vj);
			}
			else {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w_, v);
				rInsertAtPosition(rv, v, vj);
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {
			//swapTwoOnevvjw
			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rv, vj);
			rRemoveAtPosition(rw, w);

			rInsertAtPosition(rv, v_, w);

			rInsertAtPosition(rw, w_, v);
			rInsertAtPosition(rw, v, vj);

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, w);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, vj);

		}
	}
	else if (M.kind == 12) {
		// swapOneTwovwwj
		int v_ = customers[v].prev;
		int wj = customers[w].next;
		int w_ = customers[w].prev;

		if (rv.routeId == rw.routeId) {

			/*if (v == w || v == wj) {
				return false;
			}*/

			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rw, w);
			rRemoveAtPosition(rw, wj);

			// (w) -> (w+)-> (v) 
			if (v_ == wj) {

				rInsertAtPosition(rw, w_, v);
				rInsertAtPosition(rv, v, w);
				rInsertAtPosition(rv, w, wj);

			}
			else if (w_ == v) {

				// (v) -> (w) -> (w+)
				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, wj);
				rInsertAtPosition(rv, wj, v);

			}
			else {

				rInsertAtPosition(rv, v_, w);
				rInsertAtPosition(rv, w, wj);
				rInsertAtPosition(rv, w_, v);
			}

			rUpdateAvQFrom(rv, rv.head);
			rUpdateZvQFrom(rv, rv.tail);
		}
		else {
			// swapOneTwovwwj
			rRemoveAtPosition(rv, v);
			rRemoveAtPosition(rw, w);
			rRemoveAtPosition(rw, wj);

			rInsertAtPosition(rv, v_, w);
			rInsertAtPosition(rv, w, wj);
			rInsertAtPosition(rw, w_, v);

			rUpdateAvQFrom(rv, w);
			rUpdateZvQFrom(rv, wj);

			rUpdateAvQFrom(rw, v);
			rUpdateZvQFrom(rw, v);

		}
	}
	else {
		Logger::ERROR("no this inrelocate move");
		return false;
	}
	return true;
}

bool Solver::doReverse(TwoNodeMove& M) {

	int v = M.v;
	int w = M.w;

	Route& r = rts.getRouteByRouteId(customers[v].routeId);

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
		back = customers[back].prev;
	}

	int f_ = customers[front].prev;
	int bj = customers[back].next;

	Vector<int> ve;
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
		std::swap(customers[pt].next, customers[pt].prev);
	}

	customers[front].next = bj;
	customers[back].prev = f_;

	customers[f_].next = back;
	customers[bj].prev = front;

	rUpdateAvQFrom(r, back);
	rUpdateZvQFrom(r, front);

	return true;
}

Vector<int> Solver::getPtwNodes(Route& r, int ptwKind) {

	Vector<int> ptwNodes;
	ptwNodes.reserve(r.rCustCnt);

	lyhCheckTrue(r.rPtw > 0);

	auto getPtwNodesByFirstPtw = [&]() {

		int startNode = customers[r.tail].prev;
		int endNode = customers[r.head].next;

		int pt = customers[r.head].next;
		while (pt != -1) {
			if (customers[pt].avp > input->datas[pt].DUEDATE) {
				endNode = pt;
				break;
			}
			pt = customers[pt].next;
		}
		if (endNode > input->customerNumer) {
			endNode = customers[r.tail].prev;
		}

		pt = customers[r.tail].prev;
		while (pt != -1) {
			if (customers[pt].zvp < input->datas[pt].READYTIME) {
				startNode = pt;
				break;
			}
			pt = customers[pt].prev;
		}
		if (startNode > input->customerNumer) {
			startNode = customers[r.head].next;
		}

		int v = startNode;
		while (v <= input->customerNumer) {
			ptwNodes.push_back(v);
			if (v == endNode) {
				break;
			}
			v = customers[v].next;
		}

		if (v != endNode) {
			v = customers[r.head].next;
			while (v <= input->customerNumer) {
				ptwNodes.push_back(v);
				if (v == endNode) {
					break;
				}
				v = customers[v].next;
			}
		}

		lyhCheckTrue(v == endNode);

	};

	auto getPtwNodesByLastPtw = [&]() {

		int startNode = customers[r.tail].prev;
		int endNode = customers[r.head].next;

		int pt = customers[r.head].next;
		while (pt != -1) {
			if (customers[pt].avp > input->datas[pt].DUEDATE) {
				endNode = pt;
			}
			pt = customers[pt].next;
		}
		if (endNode > input->customerNumer) {
			endNode = customers[r.tail].prev;
		}

		//pt = customers[r.tail].prev;
		//while (pt != -1) {
		//	if (customers[pt].zvp < input->datas[pt].READYTIME) {
		//		startNode = pt;
		//	}
		//	pt = customers[pt].prev;
		//}
		//if (startNode > input->customerNumer ) {
		//	startNode = customers[r.head].next;
		//}
		startNode = customers[r.head].next;

		pt = startNode;
		while (pt <= input->customerNumer) {
			ptwNodes.push_back(pt);
			if (pt == endNode) {
				break;
			}
			pt = customers[pt].next;
		}
	};

	auto findRangeCanDePtw1 = [&]() {

		int startNode = customers[r.tail].prev;
		int endNode = customers[r.head].next;

		int pt = customers[r.head].next;
		bool stop = false;
		while (pt != -1) {

			if (customers[pt].avp > input->datas[pt].DUEDATE) {
				endNode = pt;
				stop = true;
				break;
			}
			pt = customers[pt].next;
		}
		if (stop == false) {
			endNode = customers[r.tail].prev;
		}

		pt = customers[r.tail].prev;
		stop = false;
		while (pt <= input->customerNumer) {

			if (customers[pt].zvp < input->datas[pt].READYTIME) {
				startNode = pt;
				stop = true;
				break;
			}
			pt = customers[pt].prev;
		}

		if (stop == false) {
			startNode = customers[r.head].next;
		}

		if (customers[startNode].prev <= input->customerNumer) {
			startNode = customers[startNode].prev;
		}

		if (customers[endNode].next <= input->customerNumer) {
			endNode = customers[endNode].next;
		}

		pt = startNode;
		while (pt <= input->customerNumer) {
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
		// OPT[1]: Tips for acceleration,
		//find last:begin from back of route,find first:begin from front of route
		getPtwNodesByLastPtw();
	}

	lyhCheckTrue(ptwNodes.size() > 0);

	return ptwNodes;
}

TwoNodeMove Solver::getMovesRandomly
(std::function<bool(TwoNodeMove& t, TwoNodeMove& bestM)>updateBestM) {

	TwoNodeMove bestM;

	auto _2optEffectively = [&](int v) {

		int vid = customers[v].routeId;

		int v_ = customers[v].prev;
		if (v_ > input->customerNumer) {
			v_ = 0;
		}

		double broaden = aps->broaden;

		int v_pos = input->addSTJIsxthcloseOf[v][v_];
		//int v_pos = input->jIsxthcloseOf[v][v_];
		if (v_pos == 0) {
			v_pos += aps->broadenWhenPos_0;
		}
		else {
			v_pos = static_cast<int>(v_pos * broaden);
		}
		v_pos = std::min<int>(v_pos, input->customerNumer);

		for (int wpos = 0; wpos < v_pos; ++wpos) {

			int w = input->addSTclose[v][wpos];
			//int w = input->allCloseOf[v][wpos];
			int wid = customers[w].routeId;

			if (wid == -1 || wid == vid) {
				continue;
			}

			//if (customers[w].av + input->datas[w].SERVICETIME + input->getDisof2(v,w) >= customers[v].avp) {
			//	continue;
			//}

			TwoNodeMove m0(v, w, 0, twoOptStarOpenvv_(v, w));
			updateBestM(m0, bestM);

		}

		int vj = customers[v].next;
		if (vj > input->customerNumer) {
			vj = 0;
		}
		int vjpos = input->addSTJIsxthcloseOf[v][vj];
		//int vjpos = input->jIsxthcloseOf[v][vj];
		if (vjpos == 0) {
			vjpos += aps->broadenWhenPos_0;
		}
		else {
			vjpos = static_cast<int>(vjpos * broaden);
		}
		vjpos = std::min<int>(vjpos, input->customerNumer);

		for (int wpos = 0; wpos < vjpos; ++wpos) {

			int w = input->addSTclose[v][wpos];
			//int w = input->allCloseOf[v][wpos];
			int wid = customers[w].routeId;
			
			if (wid == -1 || wid == vid) {
				continue;
			}

			//if (customers[w].zv - input->getDisof2(v,w) - input->datas[v].SERVICETIME <= customers[v].zvp) {
			//	continue;
			//}

			TwoNodeMove m1(v, w, 1, twoOptStarOpenvvj(v, w));
			updateBestM(m1, bestM);
		}

	};

	auto exchangevwEffectively = [&](int v) {

		int vpre = customers[v].prev;
		if (vpre > input->customerNumer) {
			vpre = 0;
		}

		int vpos1 = input->addSTJIsxthcloseOf[vpre][v];

		double broaden = aps->broaden;

		int devided = 7;
		//int devided = 4;

		if (vpos1 == 0) {
			vpos1 += aps->broadenWhenPos_0;
		}
		else {
			vpos1 = static_cast<int>(vpos1 * broaden);
		}

		vpos1 = std::min<int>(vpos1, input->customerNumer);

		if (vpos1 > 0) {

			int N = vpos1;
			int m = std::max<int>(1, N / devided);
			Vector<int>& ve = randomx->getMN(N, m);
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				int w = input->addSTclose[vpre][wpos];
				int vrId = customers[v].routeId;
				int wrId = customers[w].routeId;
				if (customers[w].routeId == -1 || wrId == vrId) {
					continue;
				}
				TwoNodeMove m8(v, w, 8, swapOneOnevw(v, w, 0));
				updateBestM(m8, bestM);

				TwoNodeMove m9(v, w, 9, swapThreeTwovvjvjjwwj(v, w,0));
				updateBestM(m9,bestM);

				//TwoNodeMove m10(v, w, 10, swapThreeOnevvjvjjw(v, w, 0));
				//updateBestM(m10,bestM);

				TwoNodeMove m11(v, w, 11, swapTwoOnevvjw(v, w, 0));
				updateBestM(m11,bestM);

				//TwoNodeMove m12(v, w, 12, swapOneTwovwwj(v, w, 0));
				//updateBestM(m12,bestM);
			}
		}


		int vnext = customers[v].next;
		if (vnext > input->customerNumer) {
			vnext = 0;
		}
		int vpos2 = input->addSTJIsxthcloseOf[vnext][v];

		if (vpos2 == 0) {
			vpos2 += aps->broadenWhenPos_0;
		}
		else {
			vpos2 = static_cast<int>(vpos2*broaden);
		}

		vpos2 = std::min<int>(vpos2, input->customerNumer);

		if (vpos2 > 0) {

			int N = vpos2;
			int m = std::max<int>(1, N / devided);

			m = std::max<int>(1, m);
			randomx->getMN(N, m);
			Vector<int>& ve = randomx->mpLLArr[N];

			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];
				wpos %= N;
				int w = input->addSTclose[vnext][wpos];
				int vrId = customers[v].routeId;
				int wrId = customers[w].routeId;
				if (customers[w].routeId == -1 || wrId == vrId) {
					continue;
				}

				TwoNodeMove m8(v, w, 8, swapOneOnevw(v, w, 0));
				updateBestM(m8, bestM);

				//TwoNodeMove m9(v, w, 9, swapThreeTwovvjvjjwwj(v, w, 0));
				//updateBestM(m9,bestM);

				//TwoNodeMove m10(v, w, 10, swapThreeOnevvjvjjw(v, w, 0));
				//updateBestM(m10,bestM);

				TwoNodeMove m11(v, w, 11, swapTwoOnevvjw(v, w, 0));
				updateBestM(m11,bestM);

				//TwoNodeMove m12(v, w, 12, swapOneTwovwwj(v, w, 0));
				//updateBestM(m12,bestM);
			}
		}

	};

	auto outrelocateEffectively = [&](int v) {

		int devided = 7;
		//int devided = 5;

		Vector<int>& relatedToV = input->iInNeicloseOfUnionNeiCloseOfI[v];

		int N = static_cast<int>(relatedToV.size());
		int m = std::max<int>(1, N / devided);

		Vector<int>& ve = randomx->getMN(N, m);
		for (int i = 0; i < m; ++i) {
			int wpos = ve[i];
			int w = relatedToV[wpos];

			int wrId = customers[w].routeId;
			if (wrId == -1) {
				continue;
			}

			if (wrId != customers[v].routeId) {
				TwoNodeMove m2(v, w, 2, outOnevToww_(v, w, 0));
				updateBestM(m2, bestM);
				TwoNodeMove m14(v, w, 14, outTwovv_Toww_(v, w, 0));
				updateBestM(m14, bestM);
				//TwoNodeMove m3(v, w, 3, outOnevTowwj(v, w,0));
				//updateBestM(m3, bestM);
				//TwoNodeMove m13(v, w, 13, outTwovvjTowwj(v, w, 0));
				//updateBestM(m13,bestM);
			}
		}
	};

	bestM.reset();

	int rId = -1;
	if (PtwConfRts.cnt > 0) {
		int index = -1;
		index = random->pick(PtwConfRts.cnt);
		rId = PtwConfRts.ve[index];
	}
	else if (PcConfRts.cnt > 0) {
		int index = -1;
		index = random->pick(PcConfRts.cnt);
		rId = PcConfRts.ve[index];
	}
	else {
		Logger::ERROR("error on conf route");
	}

	Route& r = rts.getRouteByRouteId(rId);

	lyhCheckTrue(r.rPc > 0 || r.rPtw > 0);

	auto rUpdateAvFromLimitLen = [&](int x, int len) {

		int xnext = customers[x].next;

		while (xnext != -1 && --len >= 0) {

			customers[xnext].avp =
				customers[xnext].av = customers[x].av + input->getDisof2(x, xnext) + input->datas[x].SERVICETIME;

			customers[xnext].TW_X = customers[x].TW_X
				+ std::max<DisType>(customers[xnext].avp - input->datas[xnext].DUEDATE, 0);

			customers[xnext].av = customers[xnext].avp <= input->datas[xnext].DUEDATE ?
				std::max<DisType>(customers[xnext].avp, input->datas[xnext].READYTIME) :
				input->datas[xnext].DUEDATE;

			x = xnext;
			xnext = customers[xnext].next;
		}
	};


	auto intraRouteMovesEffectively = [&](int v) ->void {

		int maxL = std::max<int>(5, r.rCustCnt / 3);

		int v_ = customers[v].prev;
		int vj = customers[v].next;

		removeCustomer(v);
		rUpdateAvFromLimitLen(v_, maxL);
		customers[v].next = vj;
		customers[v].prev = v_;

		int w = v_;

		for (int i = 0; i < maxL; ++i) {
			w = customers[w].next;
			int wj = customers[w].next;
			if (wj == -1) {
				break;
			}
			DeltaPenalty delta;
			delta.deltPc = delta.PcOnly = 0;
			delta.PtwOnly = getPtwWithVectorOfCustomersArg(3, w, v, wj) - r.rPtw;
			delta.deltPtw = delta.PtwOnly * r.rWeight * alpha;
			TwoNodeMove m3(v, w, 3, delta);
			updateBestM(m3, bestM);
		}

		rInsertAtPosition(r, v_, v);
		rUpdateAvFromLimitLen(v_, maxL + 1);

		maxL = std::max<int>(5, r.rCustCnt / 5);
		w = v;
		for (int i = 1; i <= maxL; ++i) {
			w = customers[w].next;
			//if (input->isDepot(w)) {
			//	break;
			//}
			if (w > input->customerNumer) {
				break;
			}

			TwoNodeMove m8(v, w, 8, swapOneOnevw(v, w, 1));
			updateBestM(m8, bestM);
			//if (i >= 2) {
			//	TwoNodeMove m3 = makeMove(v, w, 3);
			//	updateBestM(m3, bestM);
			//}
			if (i >= 3) {
				
				TwoNodeMove m15(v, w, 15, reversevw(v, w));
				updateBestM(m15, bestM);
			}
		}
	};

	if (r.rPtw > 0) {

		const Vector<int>&& ptwNodes = getPtwNodes(r, 0);

		for (int v : ptwNodes) {

			_2optEffectively(v);
			outrelocateEffectively(v);
			exchangevwEffectively(v);
			intraRouteMovesEffectively(v);
		}
	}
	else {

		auto rArr = rPutCustomersInVector(r);
		for (int v : rArr) {

			Vector<int>& relatedToV = input->iInNeicloseOfUnionNeiCloseOfI[v];
			int N = static_cast<int>(relatedToV.size());
			int m = N / 7;
			//int m = N;
			m = std::max<int>(1, m);

			Vector<int>& ve = randomx->getMN(N, m);
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				int w = relatedToV[wpos];
				int vrId = customers[v].routeId;
				int wrId = customers[w].routeId;

				if (customers[w].routeId == -1 || wrId == vrId) {
					continue;
				}

				TwoNodeMove m0(v, w, 0, twoOptStarOpenvv_(v, w));
				updateBestM(m0, bestM);
				TwoNodeMove m1(v, w, 1, twoOptStarOpenvvj(v, w));
				updateBestM(m1, bestM);

				TwoNodeMove m2(v, w, 2, outOnevToww_(v, w, 0));
				updateBestM(m2, bestM);
				TwoNodeMove m3(v, w, 3, outOnevTowwj(v, w, 0));
				updateBestM(m3, bestM);

				//TwoNodeMove m4(v, w, 4, inOnevv_(v, w));
				//updateBestM(m4,bestM);
				//TwoNodeMove m5(v, w, 5, inOnevvj(v, w));
				//updateBestM(m5,bestM);

				TwoNodeMove m6(v, w, 6, swapOneOnevw_(v, w, 0));
				updateBestM(m6, bestM);
				TwoNodeMove m7(v, w, 7, swapOneOnevwj(v, w, 0));
				updateBestM(m7, bestM);

				TwoNodeMove m8(v, w, 8, swapOneOnevw(v, w, 0));
				updateBestM(m8, bestM);

				/*TwoNodeMove m9(v, w, 9, swapThreeTwovvjvjjwwj(v, w,0));
				updateBestM(m9,bestM);*/

				//TwoNodeMove m10(v, w, 10, swapThreeOnevvjvjjw(v, w, 0));
				//updateBestM(m10, bestM);

				TwoNodeMove m11(v, w, 11, swapTwoOnevvjw(v, w, 0));
				updateBestM(m11, bestM);

				/*TwoNodeMove m12(v, w, 12, swapOneTwovwwj(v, w, 0));
				updateBestM(m12,bestM);*/

				TwoNodeMove m13(v, w, 13, outTwovvjTowwj(v, w, 0));
				updateBestM(m13, bestM);
				/*TwoNodeMove m14(v, w, 14, outTwovv_Toww_(v, w, 0));
				updateBestM(m14,bestM);*/
			}
		}

	}

	return bestM;

}

bool Solver::resetConfRts() {

	PtwConfRts.reset();
	PcConfRts.reset();

	for (int i = 0; i < rts.cnt; ++i) {
		if (rts[i].rPtw > 0) {
			PtwConfRts.insert(rts[i].routeId);
		}
		else if (rts[i].rPc > 0) {
			PcConfRts.insert(rts[i].routeId);
		}
	}
	return true;
};

bool Solver::resetConfRtsByOneMove(Vector<int> ids) {

	for (int id : ids) {
		PtwConfRts.removeVal(id);
		PcConfRts.removeVal(id);
	}

	for (int id : ids) {
		Route& r = rts.getRouteByRouteId(id);
		if (r.rPtw > 0) {
			PtwConfRts.insert(r.routeId);
		}
		else if (r.rPc > 0) {
			PcConfRts.insert(r.routeId);
		}
	}

	//resetConfRts();
	return true;
};

void Solver::doEjection(Vector<eOneRNode>& XSet) {

	for (eOneRNode& en : XSet) {

		Route& r = rts.getRouteByRouteId(en.rId);

		for (int node : en.ejeVe) {
			rRemoveAtPosition(r, node);
			EP.insert(node);
		}
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);

		lyhCheckTrue(r.rPc == 0 && r.rPtw == 0);
	}

	resetConfRts();
	sumRoutesPenalty();

	lyhCheckTrue(penalty == 0);
	lyhCheckTrue(PtwConfRts.cnt == 0);
	lyhCheckTrue(PcConfRts.cnt == 0);
}

bool Solver::managerCusMem(Vector<int>& releaseNodes) {

	//printve(releaseNodes);
	int useEnd = input->customerNumer + (rts.cnt + 1) * 2 + 1;

	for (int i : releaseNodes) {

		for (int j = useEnd - 1; j > i; --j) {

			if (customers[j].routeId != -1) {

				customers[i] = customers[j];

				Route& r = rts.getRouteByRouteId(customers[j].routeId);

				int jn = customers[j].next;
				if (jn != -1) {
					customers[jn].prev = i;
					r.head = i;
				}

				int jp = customers[j].prev;
				if (jp != -1) {
					customers[jp].next = i;
					r.tail = i;
				}

				customers[j].reset();
				break;
				--useEnd;
			}
		}
	}
	return true;
}

bool Solver::removeOneRouteByRouteId(int rId) {

	if (rId == -1) {
		int index = random->pick(rts.cnt);
		rId = rts[index].routeId;
	}

	Route& rt = rts.getRouteByRouteId(rId);

	Vector<int> rtVe = rPutCustomersInVector(rt);
	Vector<int> releasedNodes = { rt.head,rt.tail };
	rReset(rt);
	rts.removeIndex(rts.posOf[rId]);
	
	for (int pt : rtVe) {
		EP.insert(pt);
	}

	sumRoutesPenalty();
	resetConfRts();
	managerCusMem(releasedNodes);

	return true;
}

DisType Solver::verify() {

	Vector<int> visitCnt(input->customerNumer + 1, 0);

	int cusCnt = 0;
	DisType routesCost = 0;

	Ptw = 0;
	Pc = 0;

	for (int i = 0; i < rts.cnt; ++i) {

		Route& r = rts[i];

		//cusCnt += r.rCustCnt;
		rUpdateAvQFrom(r, r.head);
		rUpdatePc(r);
		Ptw += r.rPtw;
		Pc += r.rPc;

		Vector<int> cusve = rPutCustomersInVector(r);
		for (int pt : cusve) {
			++cusCnt;
			++visitCnt[pt];
			if (visitCnt[pt] > 1) {
				return -1;
			}
		}
		rReCalculateRouteCost(r);
		routesCost += r.routeCost;
		//debug(routesCost)
	}

	for (int i = 0; i < rts.cnt; ++i) {
		rts[i].rWeight = 1;
	}

	sumRoutesPenalty();

	if (Ptw > 0 && Pc == 0) {
		return -2;
	}
	if (Ptw == 0 && Pc > 0) {
		return -3;
	}
	if (Ptw > 0 && Pc > 0) {
		return -4;
	}
	if (cusCnt != input->customerNumer) {
		return -5;
	}
	return routesCost;
}

DeltaPenalty Solver::getDeltIfRemoveOneNode(Route& r, int pt) {

	int prev = customers[pt].prev;
	int next = customers[pt].next;

	DeltaPenalty d;

	DisType avnp = customers[prev].av + input->datas[prev].SERVICETIME + input->getDisof2(prev,next);
	d.PtwOnly = std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[prev].TW_X;
	d.PtwOnly = d.PtwOnly - r.rPtw;
	d.PcOnly = std::max<DisType>(0, r.rQ - input->datas[pt].DEMAND - input->vehicleCapacity);
	d.PcOnly = d.PcOnly - r.rPc;
	return d;

};

bool Solver::addWeightToRoute(TwoNodeMove& bestM) {

	if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly >= 0) {

		for (int i = 0; i < PtwConfRts.cnt; ++i) {
			Route& r = rts.getRouteByRouteId(PtwConfRts.ve[i]);

			//auto cus = rPutCusInve(r);
			//for (int c: cus) {
			//	auto d = getDeltIfRemoveOneNode(r, c);
			//	if (d.PtwOnly < 0) {
			//		int cpre = customers[c].prev > input->customerNumer ? 0 : customers[c].prev;
			//		int cnext = customers[c].next > input->customerNumer ? 0 : customers[c].next;
			//		/*debug(c)
			//		debug(d.PtwOnly)
			//		debug(d.PcOnly)*/
			//		(*yearTable)[c][cnext] = iter + aps->yearTabuLen;
			//		(*yearTable)[cpre][c] = iter + aps->yearTabuLen;
			//	}
			//}

			r.rWeight += aps->routeWeightUpStep;
			Ptw += r.rPtw * aps->routeWeightUpStep;
		}
		penalty = alpha * Ptw + beta * Pc;


	}
	return true;
};

bool Solver::routeWeightedRepair() {

	List<Solver> bestPool;
	bestPool.push_back(*this);

	Solver sclone = *this;

	auto updateBestPool = [&](DisType Pc, DisType PtwNoWei) {

		bool isUpdate = false;
		bool dominate = false;

		auto it = bestPool.begin();
		for (it = bestPool.begin(); it != bestPool.end();) {

			if ( (Pc < (*it).Pc && PtwNoWei < (*it).PtwNoWei)
				|| (Pc <= (*it).Pc && PtwNoWei < (*it).PtwNoWei)
				|| (Pc < (*it).Pc && PtwNoWei <= (*it).PtwNoWei)
				) {

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
			sumRoutesPenalty();

			Vector<eOneRNode> reteNode = ejectFromPatialSol();

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
				if (random->pick(bestCnt) == 0) {
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
	sumRoutesPenalty();

	//int deTimeOneTurn = 0;
	//int contiTurnNoDe = 0;

	yearTable->squIterGrowUp(aps->yearTabuLen + aps->yearTabuRand);

	DisType pBestThisTurn = DisInf;
	int stagnation = 0;

	auto updateBestM = [&](TwoNodeMove& t, TwoNodeMove& bestM)->bool {

		/*LL tYear = getYearOfMove(t);
		bool isTabu = (iter <= tYear);
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
			if (yearTable->getYearOfMove(this,t) < yearTable->getYearOfMove(this,bestM)) {
				bestM = t;
			}
		}

		return true;
	};

	while (penalty > 0 && !timer->isTimeOut()) {
	//while (penalty > 0) {

		TwoNodeMove bestM = getMovesRandomly(updateBestM);

		if (bestM.deltPen.PcOnly == DisInf || bestM.deltPen.PtwOnly == DisInf) {
			if (stagnation == aps->maxStagnationIterOfRepair) {
				break;
			}
			Logger::INFO("squeeze fail find move");
			Logger::INFO("yearTable->iter", yearTable->iter);
			++stagnation;
			continue;
		}

		#if LYH_CHECKING
		if (bestM.deltPen.PcOnly == DisInf || bestM.deltPen.PtwOnly == DisInf) {
			Logger::ERROR("squeeze fail find move");
			Logger::ERROR("yearTable->iter:",yearTable->iter);
			++stagnation;
			continue;
		}
		Vector<Vector<int>> oldRoutes;
		Vector<int> oldrv;
		Vector<int> oldrw;

		DisType oldpenalty = penalty;
		DisType oldPtwOnly = PtwNoWei;
		DisType oldPcOnly = Pc;

		Route& rv = rts.getRouteByRouteId(customers[bestM.v].routeId);
		Route& rw = rts.getRouteByRouteId(customers[bestM.w].routeId);
		oldrv = rPutCustomersInVector(rv);
		oldrw = rPutCustomersInVector(rw);

		#endif // LYH_CHECKING

		yearTable->updateYearTable(this, bestM);
		int rvId = customers[bestM.v].routeId;
		int rwId = customers[bestM.w].routeId;

		doMoves(bestM);

		yearTable->squIterGrowUp(1);
		/*solTabuTurnSolToBitArr();
		solTabuUpBySolToBitArr();*/

		updatePenalty(bestM.deltPen);

		#if LYH_CHECKING
		DisType penaltyaferup = penalty;
		sumRoutesPenalty();

		lyhCheckTrue(penaltyaferup == penalty)
			bool penaltyWeiError =
			!(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty);
		bool penaltyError =
			!(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly == PtwNoWei + Pc);

		lyhCheckTrue(!penaltyWeiError);
		lyhCheckTrue(!penaltyError);

		if (penaltyWeiError || penaltyError) {

			Logger::ERROR("rv.routeId == rw.routeId:",rv.routeId == rw.routeId);
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
		}
		#endif // LYH_CHECKING

		//resetConfRts();
		resetConfRtsByOneMove({ rvId,rwId });
		addWeightToRoute(bestM);

		if (penalty < pBestThisTurn) {
			stagnation = 0;
			pBestThisTurn = penalty;
		}
		else {
			++stagnation;
		}

		//bool isDown = updateBestPool(Pc, PtwNoWei);
		updateBestPool(Pc, PtwNoWei);

		if (stagnation == aps->maxStagnationIterOfRepair) {
			break;
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
		sumRoutesPenalty();

		return true;
	}
	else {

		bestPool.push_back(sclone);
		//debug(bestPool.size());
		auto minIter = getMinPsumSolIter();
		//debug(index)
		*this = *minIter;

		#if LYH_CHECKING
		DisType oldp = penalty;
		sumRoutesPenalty();
		lyhCheckTrue(oldp == penalty);
		#endif // LYH_CHECKING		

		return false;
	}
	return true;
}

void Solver::ruinClearEP(int kind) {

	// save the route ids��where only change these routes,only update data of them
	std::unordered_set<int> insRts;
	
	Vector<int> EPArr = EP.putElementInVector();

	auto cmp1 = [&](int a, int b) {
		return input->datas[a].DEMAND > input->datas[b].DEMAND;
	};
	auto cmp2 = [&](int a, int b) {
		return input->datas[a].DUEDATE - input->datas[a].READYTIME
			< input->datas[b].DUEDATE - input->datas[b].READYTIME;
	};

	auto cmp3 = [&](int a, int b) {
		return input->getDisof2(a,0) < input->getDisof2(b,0);
	};
	
	auto cmp4 = [&](int a, int b) {
		return input->datas[a].READYTIME > input->datas[b].READYTIME;
	};
	
	auto cmp5 = [&](int a, int b) {
		return input->datas[a].DUEDATE < input->datas[b].DUEDATE;
	};


	switch (kind) {
	case 0:
		random->shuffleVec(EPArr);
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

	for (int i = 0; i < static_cast<int>(EPArr.size()); ++i) {
		//for (int i = EPArr.size() - 1;i>=0;--i) {
		int pt = EPArr[i];
		EP.removeVal(pt);
		//++P[pt];
		auto bestFitPos = findBestPositionForRuin(pt);

		if (bestFitPos.rIndex == -1) {
			int rid = getARouteIdCanUsed();

			Route r1 = rCreateRoute(rid);
			rInsAtPosPre(r1, r1.tail, pt);
			rUpdateAvQFrom(r1, r1.head);
			rUpdateZvQFrom(r1, r1.tail);
			rts.push_back(r1);
			insRts.insert(rid);
		}
		else {
			Route& r = rts[bestFitPos.rIndex];
			insRts.insert(r.routeId);
			rInsertAtPosition(r, bestFitPos.pos, pt);
			rUpdateAvQFrom(r, pt);
			rUpdateZvQFrom(r, pt);
		}
	}

	sumRoutesPenalty();
	for (auto rId : insRts) {
		Route& r = rts.getRouteByRouteId(rId);
		rReCalculateRouteCost(r);
	}
	sumRoutesCost();
	//sumRtsPen();
}

int Solver::ruinGetSplitDepth(int maxDept) {

	if (maxDept < 1) {
		return -1;
	}

	static Vector<int> a = { 10000 };
	static Vector<int> s = { 10000 };
	if (maxDept >= static_cast<int>(a.size())) {
		int oldS = static_cast<int>(a.size());
		a.resize(maxDept + 2);
		s.resize(maxDept + 2);
		for (int i = oldS; i <= maxDept; ++i) {
			a[i] = a[i - 1] - a[i - 1] / 64;
			s[i] = s[i - 1] + a[i];
		}
	}
	int rd = random->pick(1, 1000001);
	auto index = lower_bound(s.begin(), s.begin() + maxDept - 1, rd) - s.begin();
	return static_cast<int>(index + 1);
}

Vector<int> Solver::ruinGetRuinCusBySting(int ruinKmax, int ruinLmax) {

	std::unordered_set<int> uset;

	int ruinK = random->pick(1, ruinKmax + 1);
	
	int v = random->pick(1, input->customerNumer + 1);
	while (customers[v].routeId == -1) {
		v = random->pick(input->customerNumer) + 1;
	}

	UnorderedSet<int> rIdSet;
	UnorderedSet<int> begCusSet;
	rIdSet.insert(customers[v].routeId);
	begCusSet.insert(v);

	int wpos = 0;
	while (static_cast<int>(rIdSet.size()) < ruinK) {
		int w = input->addSTclose[v][wpos];
		int wrId = customers[w].routeId;
		if (wrId != -1 && rIdSet.count(wrId) == 0) {
			rIdSet.insert(wrId);
			begCusSet.insert(w);
		}
		++wpos;
	}

	auto splitAndmiddle = [&](int beg) {
		Route& r = rts.getRouteByRouteId(customers[beg].routeId);
		auto a = rPutCustomersInVector(r);
		int n = r.rCustCnt;
		int index = static_cast<int>(std::find(a.begin(), a.end(),beg) - a.begin());

		//refinement m+t put back t customers

		int ruinL = random->pick(1, ruinLmax + 1);

		int m = std::min<int>(r.rCustCnt, ruinL);

		int t = 0;
		if (random->pickDouble(0.0,1.0) < aps->ruinSplitRate) {

			//if (n-m > 0) {
			//	t = ruinGetSplitDepth(n-m);
			//}
			if (n - m > 0) {
				t = random->pick(1, n - m + 1);
			}
		}
		int s = m + t;

		int strbeglowbound = std::max<int>(0, index - s + 1);
		int strbegupbound = std::min<int>(index,n-s);
		int strbeg = random->pick(strbeglowbound, strbegupbound+1);

		int frontStr = random->pick(1,m+1);
		int endStr = m - frontStr;

		//Vector<int> farr;
		for (int i = 0; i < frontStr;++i) {
			uset.insert(a[strbeg+i]);
			//farr.push_back(a[strbeg + i]);
		}

		//Vector<int> eArr;
		for (int i = 0; i < endStr; ++i) {
			uset.insert(a[strbeg + t + i]);
			//eArr.push_back(a[strbeg + frontStr + t + i]);
		}
	};

	for (int beg : begCusSet) {
		splitAndmiddle(beg);
	}

	Vector<int> runCus;
	runCus.reserve(uset.size());
	for (int c : uset) {
		runCus.push_back(c);
	}

	return runCus;
}

Vector<int> Solver::ruinGetRuinCusByRound(int ruinCusNum) {

	int left = std::max<int>( static_cast<int>(ruinCusNum * 0.7), 1);
	int right = std::min<int>(input->customerNumer - 1, static_cast<int>(ruinCusNum * 1.3));
	ruinCusNum = random->pick(left, right + 1);

	//=======
	//ruinCusNum = std::min<int>(ruinCusNum,input->customerNumer-1);

	int v = random->pick(input->customerNumer) + 1;
	while (customers[v].routeId == -1) {
		v = random->pick(input->customerNumer) + 1;
	}

	Vector<int> runCus;
	runCus.reserve(ruinCusNum);
	runCus.push_back(v);

	for (int i = 0; i < ruinCusNum; ++i) {
		int wpos = i;
		int w = input->addSTclose[v][wpos];
		if (customers[w].routeId != -1) {
			runCus.push_back(w);
		}
	}
	return runCus;
}

Vector<int> Solver::ruinGetRuinCusByRand(int ruinCusNum) {

	int left = std::max<int>(static_cast<int>(ruinCusNum * 0.7), 1);
	int right = std::min<int>(input->customerNumer - 1, static_cast<int>(ruinCusNum * 1.3));
	ruinCusNum = random->pick(left, right + 1);

	auto& arr = randomx->getMN(input->customerNumer, ruinCusNum);
	random->shuffleVec(arr);
	Vector<int> ret;
	for (int i = 0; i < ruinCusNum; ++i) {
		ret.push_back(arr[i]+1);
	}
	return ret;
}

Vector<int> Solver::ruinGetRuinCusByRandOneR(int ruinCusNum) {

	//int index = 0;
	//for (int i = 1; i < rts.cnt; ++i) {
	//	if (rts[i].rCustCnt < rts[index].rCustCnt) {
	//		index = i;
	//	}
	//}
    (void)ruinCusNum;
	int index = random->pick(rts.cnt);
	Route& r = rts[index];
	auto arr = rPutCustomersInVector(r);
	return arr;
}

Vector<int> Solver::ruinGetRuinCusBySec(int ruinCusNum) {

	//ruinCusNum = random->pick(1, ruinCusNum+1);

	ruinCusNum = std::min<int>(ruinCusNum, input->customerNumer - 1);

	Vector<CircleSector> secs(rts.cnt);
	for (int i = 0; i < rts.cnt; ++i) {
		secs[i] = rGetCircleSector(rts[i]);
	}

	Vector<int> rOrder(rts.cnt, 0);
	std::iota(rOrder.begin(), rOrder.end(), 0);
	random->shuffleVec(rOrder);

	Vector< Vector<int> > overPair;

	for (int i = 0; i < rts.cnt; ++i) {
		for (int j = i + 1; j < rts.cnt; ++j) {
			bool isover = CircleSector::overlap(secs[rOrder[i]], secs[rOrder[j]]);
			if (isover) {
				overPair.push_back({ rOrder [i],rOrder [j]});
			}
		}
	}

	UnorderedSet<int> cusSet;

	for (auto& apair : overPair) {

		if (static_cast<int>(cusSet.size()) >= ruinCusNum) {
			break;
		}

		int si = apair[0];
		int sj = apair[1];

		auto vei = rPutCustomersInVector(rts[si]);
		auto vej = rPutCustomersInVector(rts[sj]);

		for (int v : vei) {
			int vAngle = input->datas[v].polarAngle;
			if (static_cast<int>(cusSet.size()) >= ruinCusNum) {
				break;
			}
			if (secs[si].isEnclosed(vAngle) && secs[sj].isEnclosed(vAngle)) {
				cusSet.insert(v);
			}
			
		}
		
		for (int v : vej) {
			int vAngle = input->datas[v].polarAngle;
			if (static_cast<int>(cusSet.size()) >= ruinCusNum) {
				break;
			}
			if (secs[si].isEnclosed(vAngle) && secs[sj].isEnclosed(vAngle)) {
				cusSet.insert(v);
			}
		}
	}

	Vector<int> cusArr = putEleInVec(cusSet);

	//Logger::INFO("cusArr.size():", cusArr.size(),"runNum:",ruinCusNum);
	
	return cusArr;
}

void Solver::perturbBasedEjectionPool(int ruinCusNum) {

	auto clone = *this;

	auto ruinCus = ruinGetRuinCusBySec(ruinCusNum);
	//auto ruinCus = ruinGetRuinCusByRound(ruinCusNum);

	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRouteId(customers[cus].routeId);
		rIds.insert(r.routeId);
		if (r.rCustCnt > 2) {
			rRemoveAtPosition(r, cus);
			EP.insert(cus);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRouteId(rid);
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);
	}
	sumRoutesCost();
	sumRoutesPenalty();
	bool isej = AWLS();
	if (isej) {
		;
	}
	else {
		*this = clone;
	}
	
}

bool Solver::doOneTimeRuinPer(int perturbkind, int ruinCusNum, int clearEPKind) {

	Vector<int> ruinCus = getRuinCustomers(perturbkind,ruinCusNum);

	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRouteId(customers[cus].routeId);
		rIds.insert(r.routeId);
		if (r.rCustCnt > 2) {
			rRemoveAtPosition(r, cus);
			EP.insert(cus);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRouteId(rid);
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);
	}
	sumRoutesCost();
	sumRoutesPenalty();
	ruinClearEP(clearEPKind);

	if (penalty == 0) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool Solver::perturbBasedRuin(int perturbkind, int ruinCusNum,int clearEPKind) {

	ruinCusNum = std::min<int>(ruinCusNum,input->customerNumer / 2);

	gamma = 1;
	//OPT[4][1]:this recalucate may can be delete
	reCalRtsCostSumCost();

	Solver pClone = *this;
	Solver penMinSol = *this;
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
		else { // has penalty
			if (this->penalty < penMinSol.penalty) {
				hasPenMinSol = true;
				penMinSol = *this;
			}
			*this = pClone;
		}
	}
	
	if (perSuc) {
		return true;
	}
	else {

		if (hasPenMinSol) { 
			
			if (penMinSol.repair()) {

				*this = penMinSol;
				return true;
			}
			else {

				*this = pClone;
				return false;
			}

		}
		else {

			return false;
		}
	} 
	reCalRtsCostSumCost();
	return false;
}

int Solver::ruinLocalSearchNotNewR(int ruinCusNum) {

	gamma = 1;
	//OPT[ruin local search not new route]:this recalucate may be can be deletes
	reCalRtsCostSumCost();

	auto solclone = *this;

	static ProbControl pcRuinkind(5,random);
	static ProbControl pcClEPkind(6, random);
	
	int retState = 0;

	DisType Before = RoutesCost;

	for (int conti = 1; conti < 20;++conti) {

		int kind = pcRuinkind.getIndexBasedData();
		int kindclep = pcClEPkind.getIndexBasedData();

		bool ispertutb = perturbBasedRuin(kind, ruinCusNum, kindclep);
		//debug(conti);
		if (ispertutb) {
			auto cuses = EAX::getDiffCusofPb(solclone, *this);
			if (cuses.size() > 0) {
				simpleLocalSearch(1,cuses);
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

void Solver::reInsertCustomersIntoSolution(int kind) {

	std::unordered_set<int> insRts;
	Vector<int> EPArr = EP.putElementInVector();

	auto cmp1 = [&](int a, int b) {
		return input->datas[a].DEMAND > input->datas[b].DEMAND;
	};
	auto cmp2 = [&](int a, int b) {
		return input->datas[a].DUEDATE - input->datas[a].READYTIME
			< input->datas[b].DUEDATE - input->datas[b].READYTIME;
	};

	auto cmp3 = [&](int a, int b) {
		return input->getDisof2(a,0) < input->getDisof2(b,0);
	};

	auto cmp4 = [&](int a, int b) {
		return input->datas[a].READYTIME > input->datas[b].READYTIME;
	};

	auto cmp5 = [&](int a, int b) {
		return input->datas[a].DUEDATE < input->datas[b].DUEDATE;
	};


	switch (kind) {
	case 0:
		random->shuffleVec(EPArr);
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

	for (int i = 0; i < static_cast<int>(EPArr.size()); ++i) {
		//for (int i = EPArr.size() - 1;i>=0;--i) {
		int pt = EPArr[i];
		EP.removeVal(pt);
		//++P[pt];
		auto bestPos = findBestPositionForRuin(pt);
		
		if (bestPos.pen == 0) {
			Route& r = rts[bestPos.rIndex];
			insRts.insert(r.routeId);
			rInsertAtPosition(r, bestPos.pos, pt);
			rUpdateAvQFrom(r, pt);
			rUpdateZvQFrom(r, pt);
		}
		else {

			int rid = getARouteIdCanUsed();
			
			Route r1 = rCreateRoute(rid);
			rInsAtPosPre(r1, r1.tail, pt);
			rUpdateAvQFrom(r1, r1.head);
			rUpdateZvQFrom(r1, r1.tail);
			rts.push_back(r1);
			insRts.insert(rid);
		}
	}

	reCalRtsCostAndPen();

	lyhCheckTrue(RoutesCost == verify());
}

int Solver::CVB2ruinLS(int ruinCusNum) {

	static ProbControl pcRuKind(5, random);
	//static ProbControl pcRuKind(3);
	static ProbControl pcCLKind(6, random);

	
	Solver solClone = *this;
	int perturbkind = pcRuKind.getIndexBasedData();

	Vector<int> ruinCus = getRuinCustomers(perturbkind,ruinCusNum);
	
	std::unordered_set<int> rIds;
	for (int cus : ruinCus) {
		Route& r = rts.getRouteByRouteId(customers[cus].routeId);
		rIds.insert(r.routeId);
		rRemoveAtPosition(r, cus);
		EP.insert(cus);

		if (r.rCustCnt == 0) {
			if (rIds.count(r.routeId)>0) {
				rIds.erase(rIds.find(r.routeId));
			}
			removeOneRouteByRouteId(r.routeId);
		}
	}

	for (auto rid : rIds) {
		Route& r = rts.getRouteByRouteId(rid);
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);
	}

	sumRoutesCost();
	sumRoutesPenalty();

	//reCalRtsCostAndPen();

	int clearKind = pcCLKind.getIndexBasedData();

	reInsertCustomersIntoSolution(clearKind);
	bks->updateBKSAndPrint(*this, "out and in");

	auto cuses = EAX::getDiffCusofPb(solClone, *this);
	if (cuses.size() > 0) {
		simpleLocalSearch(1, cuses);
	}

	if (RoutesCost < solClone.RoutesCost) {
		++pcRuKind.data[perturbkind];
		++pcCLKind.data[clearKind];
	}
	return true;
}

void Solver::refinement(int kind,int iterMax, double temperature,int ruinNum) {

	Solver pBest = *this;
	Solver s = *this;

	int iter = 1;

	double j0 = temperature;
	double jf = 1;
	double c = pow(jf / j0, 1 / double(iterMax * 3));
	temperature = j0;

	while (++iter < iterMax){

		auto sStar = s;

		//!!!!!!globalRepairSquIter();

		if (kind == 0) {
			//sStar.ruinLocalSearchNotNewR(aps->ruinC_);
			sStar.ruinLocalSearchNotNewR(ruinNum);
		}
		else if (kind == 1) {
			sStar.CVB2ruinLS(ruinNum);
		}
		
		bks->updateBKSAndPrint(sStar,"from refinement sStart");

		auto deltDouble = temperature * log(double(random->pick(1, 100000)) / (double)100000);
		DisType delt = static_cast<DisType>(deltDouble);

		if (sStar.RoutesCost < s.RoutesCost - delt) {
			s = sStar;
		}

		temperature *= c;

		if (sStar.RoutesCost < pBest.RoutesCost) {
			pBest = sStar;
			iter = 1;
			//temperature = j0;
		}
	}

	*this = pBest;

}

Vector<int> Solver::getRuinCustomers(int perturbkind, int ruinCusNum) {

	Vector<int> ruinCus;
	if (perturbkind == 0) {
		ruinCus = ruinGetRuinCusByRound(ruinCusNum);
	}
	else if (perturbkind == 1) {
		ruinCus = ruinGetRuinCusBySec(ruinCusNum);
	}
	else if (perturbkind == 2) {
		int avgLen = input->customerNumer / rts.cnt;
		int Lmax = std::min<int>(aps->ruinLmax, avgLen);
		int ruinKmax = 4 * ruinCusNum / (1 + Lmax) - 1;
		ruinKmax = std::min<int>(rts.cnt - 1, ruinKmax);
		ruinKmax = std::max<int>(1, ruinKmax);
		ruinCus = ruinGetRuinCusBySting(ruinKmax, Lmax);
	}
	else if (perturbkind == 3) {
		ruinCus = ruinGetRuinCusByRand(ruinCusNum);
	}
	else if (perturbkind == 4) {
		ruinCus = ruinGetRuinCusByRandOneR(ruinCusNum);
	}
	else {
		Logger::ERROR("no this kind of refinement");
	}
	return ruinCus;
}

void Solver::perturb(int Irand) {

	int I1000 = aps->perturbIrand;
	if (Irand > 0) {
		I1000 = Irand;
	}

	DisType beforeGamma = gamma;
	int iter = 0;
	gamma = 0;

	Vector<int> kindSet = { 0,1,6,7/*,8,9,10,2,3,4,5*/ };
	int patternAdjustmentGetM = 10;

	int N = aps->perturbNeiborRange;
	int m = std::min<int>(patternAdjustmentGetM, N);

	auto getDelt0MoveRandomly = [&]() {

		TwoNodeMove ret;

		for (int iter = 0; ++iter < 100;++iter) {

			int v = random->pick(input->customerNumer) + 1;
			if (customers[v].routeId == -1) {
				continue;
			}

			m = std::max<int>(1, m);
			randomx->getMN(N, m);
			Vector<int>& ve = randomx->mpLLArr[N];
			for (int i = 0; i < m; ++i) {
				int wpos = ve[i];

				int w = input->addSTclose[v][wpos];
				if (customers[w].routeId == -1
					//|| customers[w].routeId == customers[v].routeId
					) {
					continue;
				}

				for (int kind : kindSet) {

					DeltaPenalty d;
					if (kind == 6 || kind == 7) {
						d = estimatevw(kind, v, w, 1);
					}
					else {
						d = estimatevw(kind, v, w, 0);
					}

					if (d.deltPc + d.deltPtw == 0) {
						TwoNodeMove move(v, w, kind, d);
						ret = move;
						if (yearTable->iter >= yearTable->getYearOfMove(this,move)) {
							return move;
						}
					}

				}
			}
		}
		return ret;
	};

	yearTable->squIterGrowUp(aps->yearTabuLen + aps->yearTabuRand);

	do {

		TwoNodeMove bestM = getDelt0MoveRandomly();
		
		if (bestM.deltPen.deltPc + bestM.deltPen.deltPtw > 0) {
			break;
		}

		yearTable->updateYearTable(this, bestM);
		doMoves(bestM);
		yearTable->squIterGrowUp(1);

	} while (++iter < I1000);

	sumRoutesPenalty();
	if (beforeGamma == 1) {
		reCalRtsCostSumCost();
		gamma = beforeGamma;
	}
}

Vector<Solver::eOneRNode> Solver::ejectFromPatialSol() {

	Vector<eOneRNode>ret;

	Vector<int>confRSet;

	confRSet.reserve(PtwConfRts.cnt + PcConfRts.cnt);
	for (int i = 0; i < PtwConfRts.cnt; ++i) {
		confRSet.push_back(PtwConfRts.ve[i]);
	}
	for (int i = 0; i < PcConfRts.cnt; ++i) {
		confRSet.push_back(PcConfRts.ve[i]);
	}

	for (int id : confRSet) {

		Route& r = rts.getRouteByRouteId(id);

		eOneRNode retNode;
		int tKmax = aps->minKmax;
		//int tKmax = aps->maxKmax;

		while (tKmax <= aps->maxKmax) {

			auto en = ejectOneRouteOnlyP(r, tKmax);

			if (retNode.ejeVe.size() == 0) {
				retNode = en;
			}
			else {

				//bool s3 = en.Psum * retNode.ejeVe.size() < retNode.Psum * en.ejeVe.size();

				if (aps->psizemulpsum) {
					bool s1 = en.Psum < retNode.Psum;
					bool s2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();
					if (s1 && s2) {
						retNode = en;
					}
				}
				else {
					bool s1 = en.Psum < retNode.Psum;
					if (s1) {
						retNode = en;
					}
				}
			}
			++tKmax;
		}

		//debug(retNode.ejeVe.size())
		//eOneRNode retNode = ejectOneRouteOnlyP(r, 2, aps->maxKmax);
		auto en = ejectOneRouteMinPsumGreedy(r, retNode);

		if (aps->psizemulpsum) {

			bool s1 = en.Psum < retNode.Psum;
			bool s2 = en.ejeVe.size() * en.Psum < retNode.Psum* retNode.ejeVe.size();

			if (s1 && s2) {
				//Logger::INFO("s1 && s2");
				retNode = en;
			}
		}
		else {

			bool s1 = en.Psum < retNode.Psum;
			if (s1) {
				//Logger::INFO("s1");
				retNode = en;
			}
		}

		//if (retNode.ejeVe.size() == 0) {
		//	retNode = ejectOneRouteMinPsumGreedy(r, retNode);
		//}

		lyhCheckTrue(retNode.ejeVe.size() > 0);

		ret.push_back(retNode);
	}

	return ret;
}

Solver::eOneRNode Solver::ejectOneRouteOnlyHasPcMinP(Route& r) {

	eOneRNode noTabuN(r.routeId);
	noTabuN.Psum = 0;

	Vector<int> R = rPutCustomersInVector(r);
#if 0
	auto cmpMinP = [&](const int& a, const int& b) {
		if (input->P[a] == input->P[b]) {
			return input->datas[a].DEMAND > input->datas[b].DEMAND;
		}
		else {
			return input->P[a] > input->P[b];
		}
		return false;
	};
#endif 

	auto cmpMinD = [&](const int& a, const int& b) -> bool {

		if (input->datas[a].DEMAND == input->datas[b].DEMAND) {
			return input->P[a] > input->P[b];
		}
		else {
			return input->datas[a].DEMAND > input->datas[b].DEMAND;
		}
		return false;
	};

	std::priority_queue<int, Vector<int>, decltype(cmpMinD)> qu(cmpMinD);

	for (const int& c : R) {
		qu.push(c);
	}

	DisType rPc = r.rPc;
	DisType rQ = r.rQ;

	while (rPc > 0) {

		int ctop = qu.top();

		qu.pop();
		rQ -= input->datas[ctop].DEMAND;
		rPc = std::max<DisType>(0, rQ - input->vehicleCapacity);
		noTabuN.ejeVe.push_back(ctop);
		noTabuN.Psum += input->P[ctop];
	}

	return noTabuN;

}

Solver::eOneRNode Solver::ejectOneRouteOnlyP(Route& r, int Kmax) {

	eOneRNode noTabuN(r.routeId);

	//int sameCnt = 1;
	eOneRNode etemp(r.routeId);
	etemp.Psum = 0;

	DisType rQ = r.rQ;

	auto updateEje = [&]() {
		if (noTabuN.ejeVe.size() == 0) {
			noTabuN = etemp;
		}
		else {

			if (aps->psizemulpsum) {
				if (etemp.Psum < noTabuN.Psum) {
					if (etemp.ejeVe.size() * etemp.Psum < noTabuN.Psum * noTabuN.ejeVe.size()) {
						//if (etemp.ejeVe.size() <= noTabuN.ejeVe.size()) {
						noTabuN = etemp;
					}
				}
			}
			else {
				if (etemp.Psum < noTabuN.Psum) {
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
		//	/*if (random->pick(sameCnt) == 0) {
		//		
		//	}*/
		//}
	};

	auto getAvpOf = [&](int v) {

		int prev = customers[v].prev;
		DisType avp = customers[prev].av +
			input->datas[prev].SERVICETIME + input->getDisof2(prev,v);
		return avp;
	};

	auto updateAvfromSubRoute = [&](int n) {

		int prev = customers[n].prev;

		while (n != -1) {

			customers[n].avp = customers[prev].av + input->datas[prev].SERVICETIME + input->getDisof2(prev,n);
			customers[n].av = customers[n].avp > input->datas[n].DUEDATE ? input->datas[n].DUEDATE
				: std::max<DisType>(customers[n].avp, input->datas[n].READYTIME);
			customers[n].TW_X = std::max<DisType>(0, customers[n].avp - input->datas[n].DUEDATE);
			customers[n].TW_X += customers[prev].TW_X;
			prev = n;
			n = customers[n].next;
		}
	};

	auto delOneNodeInPreOrder = [&](int n) {

		rQ -= input->datas[n].DEMAND;
		etemp.ejeVe.push_back(n);
		etemp.Psum += input->P[n];

		int next = customers[n].next;
		customers[next].prev = customers[n].prev;
		updateAvfromSubRoute(next);
	};

	auto getPtwUseEq2 = [&](int v) {

		DisType avp = getAvpOf(v);
		DisType ptw = std::max<DisType>(0, avp - customers[v].zv) + customers[v].TWX_;
		int prev = customers[v].prev;
		ptw += customers[prev].TW_X;
		return ptw;
	};

	auto restoreOneNodePreOrder = [&](int n) {

		rQ += input->datas[n].DEMAND;
		etemp.ejeVe.pop_back();
		etemp.Psum -= input->P[n];

		int next = customers[n].next;
		customers[next].prev = n;
		updateAvfromSubRoute(n);
	};

	auto getPtwIfRemoveOneNode = [&](int head) {

		int pt = head;
		int prev = customers[pt].prev;
		int next = customers[pt].next;

		lyhCheckTrue(prev != -1);
		lyhCheckTrue(next != -1);

		DisType ptw = 0;

		while (next != -1) {

			//debug(pt)
			DisType avnp = customers[prev].av + input->datas[prev].SERVICETIME + input->getDisof2(prev,next);
			ptw = std::max<DisType>(0, avnp - customers[next].zv) + customers[next].TWX_ + customers[prev].TW_X;

			if (customers[prev].TW_X > 0) { // prune after delte ik, the ptw still exist
				return etemp;
			}

			rQ -= input->datas[pt].DEMAND;
			etemp.ejeVe.push_back(pt);
			etemp.Psum += input->P[pt];

			if (ptw == 0 && rQ - input->vehicleCapacity <= 0) {
				updateEje();
			}

			rQ += input->datas[pt].DEMAND;
			etemp.ejeVe.pop_back();
			etemp.Psum -= input->P[pt];

			prev = pt;
			pt = next;
			next = customers[next].next;
		}
		return etemp;

	};

	Vector<int> R = rPutCustomersInVector(r);

	Vector<int> ptwArr;

	if (r.rPtw > 0) {

		ptwArr = R;
		Kmax = std::min<int>(Kmax,static_cast<int>(ptwArr.size()) - 1);
	}
	else if (r.rPc > 0) {

		return ejectOneRouteOnlyHasPcMinP(r);
		ptwArr = R;
		Kmax = std::min<int>(Kmax,static_cast<int>(ptwArr.size()) - 1);

	}
	else {
		return {};
	}

	int k = 0;
	int N = static_cast<int>(ptwArr.size()) - 1;
	Vector<int> ve(Kmax + 1, -1);

	//etemp = getPtwIfRemoveOneNode(r.head);
	//updateEje();

	do {

		if (k < Kmax && ve[k] < N) { // k increase

			++k;
			ve[k] = ve[k - 1] + 1;

			int delv = ptwArr[ve[k]];

			// only compare the Psum  > , not conside the equal
			while (etemp.Psum + input->P[delv] > noTabuN.Psum && ve[k] < N) {
				++ve[k];
				delv = ptwArr[ve[k]];
			}

			delOneNodeInPreOrder(delv);

			DisType ptw = getPtwUseEq2(customers[delv].next);

			if (ptw == 0 && rQ - input->vehicleCapacity <= 0) {
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

			// only compare the Psum  > , not conside the equal
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

			if (ptw == 0 && rQ - input->vehicleCapacity <= 0) {
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

	return noTabuN;
}

Solver::eOneRNode Solver::ejectOneRouteMinPsumGreedy
(Route& r, eOneRNode cutBranchNode) {

	eOneRNode ret(r.routeId);
	ret.Psum = 0;
	Vector<int> R = rPutCustomersInVector(r);

	auto cmpP = [&](const int& a, const int& b) ->bool {

		if (input->P[a] == input->P[b]) {
			/*return input->datas[a].DUEDATE - input->datas[a].READYTIME <
				input->datas[b].DUEDATE - input->datas[b].READYTIME;*/
			return input->datas[a].DEMAND > input->datas[b].DEMAND;
		}
		else {
			return input->P[a] > input->P[b];
		}
		return false;
	};
	
	//auto cmpD = [&](const int& a, const int& b) ->bool {
	//	if (input->datas[a].DEMAND == input->datas[b].DEMAND) {
	//		return input->P[a] > input->P[b];
	//	}
	//	else {
	//		return input->datas[a].DEMAND > input->datas[b].DEMAND;
	//	}
	//	return false;
	//};

	std::priority_queue<int, Vector<int>, decltype(cmpP)> qu(cmpP);

	for (const int& c : R) {
		qu.push(c);
	}

	DisType curPtw = r.rPtw;

	Vector<int> noGoodToPtw;
	noGoodToPtw.reserve(R.size());

	while (curPtw > 0) {

		int ctop = qu.top();
		qu.pop();

		if (ret.Psum >= cutBranchNode.Psum) {
			return eOneRNode();
		}

		int prev = customers[ctop].prev;
		int nex = customers[ctop].next;
		DisType avnp = customers[prev].av + input->datas[prev].SERVICETIME + input->getDisof2(prev,nex);
		DisType ptw = std::max<DisType>(0, avnp - customers[nex].zv) + customers[nex].TWX_ + customers[prev].TW_X;

		if (ptw < curPtw) {

			rRemoveAtPosition(r, ctop);
			rUpdateAvQFrom(r, prev);
			rUpdateZvQFrom(r, nex);
			curPtw = ptw;
			ret.ejeVe.push_back(ctop);
			ret.Psum += input->P[ctop];

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
		rRemoveAtPosition(r, ctop);
		ret.ejeVe.push_back(ctop);
		ret.Psum += input->P[ctop];
	}

	//debug(r.rCustCnt)
	rRemoveAllCustomersInRoute(r);
	//debug(r.rCustCnt)

	for (int v : R) {
		rInsAtPosPre(r, r.tail, v);
	}
	rUpdateAvQFrom(r, r.head);
	rUpdateZvQFrom(r, r.tail);

	return ret;
}

bool Solver::resetSolver() {

	alpha = 1;
	beta = 1;
	gamma = 1;
	//iter = 0;
	penalty = 0;
	Ptw = 0;
	PtwNoWei = 0;
	Pc = 0;
	//minEPSize = inf;
	RoutesCost = 0;

	for (int i = 0; i < rts.cnt; ++i) {
		rReset(rts[i]);
	}
	rts.reset();
	EP.reset();
	return true;
}

void Solver::simpleClearEP() {

	for (int EPIndex = 0; EPIndex < EP.size();) {

		Vector<int> arr = EP.putElementInVector();
		int top = arr[EPIndex];

		Position bestP = findBestPositionInSolution(top);

		if (bestP.pen == 0) {

			Route& r = rts[bestP.rIndex];

			input->P[top] += aps->customersWeight1;
			//EP(*yearTable)[top] = EPIter + aps->EPTabuStep + random->pick(aps->EPTabuRand);
			EP.removeVal(top);

			rInsertAtPosition(r, bestP.pos, top);
			rUpdateAvQFrom(r, top);
			rUpdateZvQFrom(r, top);

			sumRoutesPenalty();
			resetConfRts();

		}
		else {
			++EPIndex;
		}
	}
}

bool Solver::AWLS() {

	minEPcus = IntInf;
	yearTable->squIterGrowUp(aps->yearTabuLen + aps->yearTabuRand);
	int maxOfPval = 0;

	DisType beforeGamma = gamma;
	gamma = 0;

	int EpCusNoDown = 1;
	int iter = 1;
	
	while (iter < aps->ejectLSMaxIter && !timer->isTimeOut()) {
	//while (!timer->isTimeOut()) {
		//while (1) {
		
		++iter;

		simpleClearEP();

		if (EP.size() < minEPcus) {
			minEPcus = EP.size();
			EpCusNoDown = 1;
		}
		else {

			++EpCusNoDown;
			if (EpCusNoDown % 10000 == 0) {

				//aps->minKmax = 1;
				// change minKmax between 1 and 2
				//aps->minKmax = 3 - aps->minKmax;
				Logger::INFO("aps->minKmax:", aps->minKmax);
			}
		}

		minEPcus = std::min<int>(minEPcus, EP.size());
		if (EP.size() == 0 && penalty == 0) {
			//debug(iter);
			break;
		}

		int top = EP.randomPeek(random);

		Position bestP = findBestPositionInSolution(top);

		Route& r = rts[bestP.rIndex];
		EP.removeVal(top);

		input->P[top] += aps->customersWeight1;
		maxOfPval = std::max<int>(input->P[top], maxOfPval);

		if (maxOfPval >= 100) {
			maxOfPval = 0;
			for (auto& i : input->P) {
				i = static_cast<int>(i * 0.5) + 1;
				maxOfPval = std::max<int>(maxOfPval, i);
			}
		}

		#if LYH_CHECKING
		DisType oldpenalty = PtwNoWei + Pc;
		#endif

		rInsertAtPosition(r, bestP.pos, top);
		rUpdateAvQFrom(r, top);
		rUpdateZvQFrom(r, top);
		sumRoutesPenalty();
		resetConfRts();

		#if LYH_CHECKING
		reCalRtsCostAndPen();
		lyhCheckTrue((oldpenalty + bestP.pen == PtwNoWei + Pc));
		if (oldpenalty + bestP.pen != PtwNoWei + Pc) {
			Logger::ERROR("oldpenalty:", oldpenalty);
			Logger::ERROR("bestP.pen:", bestP.pen);
			Logger::ERROR("PtwNoWei + Pc:", PtwNoWei + Pc);
		}
		#endif // LYH_CHECKING

		if (bestP.pen == 0) {
			continue;
		}
		else {

			#if LYH_CHECKING
			lyhCheckTrue(penalty > 0);
			if (penalty == 0) {
				Logger::ERROR("penalty == 0:", penalty == 0);
				Logger::ERROR("penalty == 0:", penalty == 0);
			}
			#endif // LYH_CHECKING

			if (routeWeightedRepair() == true) {
				continue;
			}
			else {

				auto& XSet = ejeNodesAfterSqueeze;

				for (eOneRNode& en : XSet) {
					for (int c : en.ejeVe) {
						
						input->P[c] += aps->customersWeight2;
						maxOfPval = std::max<int>(input->P[c], maxOfPval);

					}
				}

				doEjection(XSet);
				int Irand = input->customerNumer / EP.cnt / 4;
				Irand = std::max<int>(Irand, input->aps->perturbIrand);
				perturb(Irand);
				//perturb();
			}
		}
	}

	if (beforeGamma == 1) {
		gamma = 1;
		reCalRtsCostSumCost();
	}

	return (EP.size() == 0 && penalty == 0);
}

bool Solver::minimizeRouteNumber(int ourTarget) {

	DisType beforeGamma = gamma;
	gamma = 0;

	while (rts.cnt > ourTarget && !timer->isTimeOut()) {

		Solver sclone = *this;
		removeOneRouteByRouteId();

		std::fill(input->P.begin(), input->P.end(), 1);
		bool isDelete = AWLS();
		if (isDelete) {

			//saveOutAsSintefFile();
			Logger::INFO("rts.cnt:", rts.cnt);
			if (rts.cnt == input->Qbound) {
				break;
			}
		}
		else {
			*this = sclone;
			break;
		}
	}

	if (beforeGamma) {
		gamma = beforeGamma;
		reCalRtsCostSumCost();
	}
	if (rts.cnt == ourTarget) {
		return true;
	}
	return false;
	//Logger::INFO("minRN,rts.cnt:",rts.cnt);
}

Solver::Position Solver::findBestPosToSplit(Route& r) {

	auto arr = rPutCustomersInVector(r);

	Position ret;

	for (int i = 0; i + 1 < static_cast<int>(arr.size()); ++i) {
		int v = arr[i];
		int vj = arr[i + 1];
		DisType delt = 0;
		delt -= input->getDisof2(v,vj);
		delt += input->getDisof2(0,v);
		delt += input->getDisof2(0,vj);
		if (delt < ret.cost) {
			ret.cost = delt;
			ret.pos = v;
		}
	}
	return ret;
};

int Solver::getARouteIdCanUsed(){

	int rId = -2;
	for (int i = 0; i < static_cast<int>(rts.posOf.size()); ++i) {
		if (rts.posOf[i] == -1) {
			rId = i;
			break;
		}
	}
	if (rId == -2) {
		rId =static_cast<int>(rts.posOf.size());
	}
	return rId;
};

int Solver::splitLocalSearch() {

	for (int i = 0; i < rts.cnt; ++i) {

		Route& r = rts[i];
		Position vsp = findBestPosToSplit(r);

		if (vsp.cost > 0) {
			continue;
		}

		auto arr = rPutCustomersInVector(r);

		int rId = getARouteIdCanUsed();
		Route r1 = rCreateRoute(rId);

		for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
			rRemoveAtPosition(r, arr[i]);
			rInsAtPosPre(r1, r1.tail, arr[i]);
			if (arr[i] == vsp.pos) {
				break;
			}
		}

		rUpdateAvQFrom(r1, r1.head);
		rUpdateZvQFrom(r1, r1.tail);
		rUpdateAvQFrom(r, r.head);
		rUpdateZvQFrom(r, r.tail);
		rReCalculateRouteCost(r);
		rReCalculateRouteCost(r1);
		rts.push_back(r1);

		sumRoutesCost();
		sumRoutesPenalty();
	}

	return 0;
}

bool Solver::adjustRouteNumber(int ourTarget) {

	if (rts.cnt > ourTarget) {
		minimizeRouteNumber(ourTarget);
	}
	else if (rts.cnt < ourTarget) {
		
		while (rts.cnt < ourTarget) {

			int index = -1;

			for (int i = 0; i < rts.cnt; ++i) {
				Route& ri = rts[i];

				if ( ri.rCustCnt>=2) {
					if (index == -1) {
						index = i;
					}
					else if (ri.routeCost > rts[index].routeCost) {
						index = i;
					}
				}
			}

			int rId = getARouteIdCanUsed();
			Route r1 = rCreateRoute(rId);

			Route& r = rts[index];

			auto vsp = findBestPosToSplit(r);

			auto arr = rPutCustomersInVector(r);

			for (int i = 0; i < static_cast<int>(arr.size()); ++i) {
				rRemoveAtPosition(r, arr[i]);
				rInsAtPosPre(r1, r1.tail, arr[i]);
				if (arr[i] == vsp.pos) {
					break;
				}
			}

			rUpdateAvQFrom(r1, r1.head);
			rUpdateZvQFrom(r1, r1.tail);
			rUpdateAvQFrom(r, r.head);
			rUpdateZvQFrom(r, r.tail);
			rReCalculateRouteCost(r);
			rReCalculateRouteCost(r1);
			rts.push_back(r1);

			sumRoutesCost();
			sumRoutesPenalty();
			
		}
		//Logger::INFO("split adjust rn rts.cnt:", rts.cnt);
	}
	else {
		//Logger::INFO("no need adjust rn rts.cnt:", rts.cnt);
	}
	
	if (rts.cnt == ourTarget) {
		return true;
	}
	else {
		return false;
	}
}

bool Solver::repair() {

	for (int i = 0; i < rts.cnt; ++i) {
		rts[i].rWeight = 1;
	}

	resetConfRts();
	sumRoutesPenalty();

	gamma = 1;

	yearTable->squIterGrowUp(aps->yearTabuLen + aps->yearTabuRand);

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

	//static Vector<int> moveContribute(16,0);

	auto iter = 0;

	auto penBest = penalty;


	int repairExitStep = 5;

	while (penalty > 0) {
		++iter;
		if (contiNotDe > repairExitStep) {
			break;
		}
		//OPT[repair]:how to choose neiboorhood may be improved
		TwoNodeMove bestM = getMovesRandomly(updateBestM);

		if (bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly > 0) {

			++contiNotDe;
			if (contiNotDe > repairExitStep) {
				break;
			}
			//maxCon = std::max<int>(maxCon,stagnation);
			if (bestM.v == 0 && bestM.w == 0) {
				break;
			}
			else {
				continue;
			}
		}

		//update(*yearTable)(bestM);
		int rvId = customers[bestM.v].routeId;
		int rwId = customers[bestM.w].routeId;

		doMoves(bestM);
		
		yearTable->squIterGrowUp(1);

		sumRoutesPenalty();

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

	reCalRtsCostAndPen();

	if (penalty == 0) {
		return true;
	}
	else {
		return false;
	}
	return false;
}

bool Solver::simpleLocalSearch(int hasRange,Vector<int> newCus) {

	alpha = 1;
	beta = 1;
	gamma = 1;

	//reCalRtsCostSumCost();

	TwoNodeMove MRLbestM;

	static Vector<int> moveKindOrder = { 0,1,2,3,4,5,6,7, 8,/*9,10,*/ 11,/*12,*/13,/*14,*/15};
	//static Vector<int> moveKindOrder = { 0,1,2,3,4,5,6,7, 8,9,10,11,12,13,14,15};

	static Vector<int> contribution(16, 0);
	Vector<int> contricus(input->customerNumer + 1, 0);
	//auto maxIt = std::max_element(contribution.begin(), contribution.end());
	for (auto& i : contribution) { i = (i >> 1) + 1; }
	//std::sort(moveKindOrder.begin(), moveKindOrder.end(), [&](int a, int b) {
	//	return contribution[a] > contribution[b];
	//});

	//printve(moveKindOrder);

	auto MRLUpdateM = [&](TwoNodeMove& m) {

		if (m.deltPen.deltPc <= 0 && m.deltPen.deltPtw <= 0) {
			if (m.deltPen.deltCost < MRLbestM.deltPen.deltCost) {
				MRLbestM = m;
			}
		}
	};
	
	lyhCheckTrue((hasRange == 0 && newCus.size() == 0)
		|| (hasRange == 1 && newCus.size() > 0));

	if (hasRange == 0) {
		newCus = randomx->getMN(input->customerNumer + 1, input->customerNumer + 1);
	}

	random->shuffleVec(newCus);
	std::queue<int> qu;
	for (int i : newCus) {
		qu.push(i);
	}

	auto getMovesGivenRange = [&](int range) {

		MRLbestM.reset();
		bool isFind = false;

		//OPT[simple local search]:sort the neiborhood by attribution
		std::sort(moveKindOrder.begin(), moveKindOrder.end(), [&](int a, int b) {
			return contribution[a] > contribution[b];
		});
		//random->shuffleVec(moveKindOrder);

		int n = static_cast<int>(qu.size());
		for (int i = 0; i < n; ++i) {
			int v = qu.front();
			qu.pop();
			qu.push(v);

		//random->shuffleVec(newCus);
		//for(int v:newCus){

			if (customers[v].routeId == -1) {
				continue;
			}

			int beg = 0;

			if (range == aps->neiborRange[0]) {
			}
			else if (range == aps->neiborRange[1]) {
				beg = aps->neiborRange[0];
			}

			for (int i = beg; i < range; ++i) {

				int wpos = i;

				int w = input->addSTclose[v][wpos];

				if (customers[w].routeId == -1) {
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
						isFind = true;
						break;
						//return MRLbestM;
					}
				}
				if (isFind) {
					break;
				}
			}

			if (isFind) {
				break;
			}
		}
		return MRLbestM;
	};

	for (int i = 0; i < rts.cnt; ++i) {
		rts[i].rWeight = 1;
	}
	yearTable->squIterGrowUp(aps->yearTabuLen + aps->yearTabuRand);

	Vector<int>& ranges = aps->neiborRange;

	while (true) {

		TwoNodeMove bestM;
		//TwoNodeMove bestM 
		for (int i = 0; i < static_cast<int>(ranges.size()); ++i) {
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

		if (bestM.deltPen.PtwOnly > 0
			|| bestM.deltPen.PcOnly > 0
			|| bestM.deltPen.deltCost >= 0
			) {
			break;
		}

		//++contricus[bestM.v];
		++contribution[bestM.kind];

		#if LYH_CHECKING
		Vector<Vector<int>> oldRoutes;
		Vector<int> oldrv;
		Vector<int> oldrw;

		DisType oldpenalty = penalty;
		DisType oldPtwOnly = PtwNoWei;
		DisType oldPcOnly = Pc;
		DisType oldRcost = RoutesCost;

		int rvid = customers[bestM.v].routeId;
		int rwid = customers[bestM.w].routeId;

		Route& rv1 = rts.getRouteByRouteId(rvid);
		Route& rw1 = rts.getRouteByRouteId(rwid);
		oldrv = rPutCustomersInVector(rv1);
		oldrw = rPutCustomersInVector(rw1);

		#endif // LYH_CHECKING

		int rvId = customers[bestM.v].routeId;
		int rwId = customers[bestM.w].routeId;
		Route& rv = rts.getRouteByRouteId(rvId);
		Route& rw = rts.getRouteByRouteId(rwId);

		yearTable->updateYearTable(this,bestM);

		doMoves(bestM);

		updatePenalty(bestM.deltPen);

		RoutesCost += bestM.deltPen.deltCost;

		rReCalculateRouteCost(rv);
		rReCalculateRouteCost(rw);

		bks->updateBKSAndPrint(*this);
		
		#if LYH_CHECKING
		for (int i = 0; i < rts.cnt; ++i) {
			auto rold = rts[i].routeCost;
			rReCalculateRouteCost(rts[i]);
			if (rold != rts[i].routeCost) {
				Logger::ERROR(rv.routeId);
				Logger::ERROR(rw.routeId);
				Logger::ERROR(rts[i].routeId);
				Logger::ERROR(rts[i].routeId);
			}
		}

		DisType penaltyafterupdatePen = penalty;
		DisType costafterplus = RoutesCost;

		sumRoutesPenalty();
		reCalRtsCostSumCost();

		if (!(costafterplus == RoutesCost)) {
			Logger::ERROR("costafterplus:", costafterplus);
			Logger::ERROR("rvid:", rvid);
			Logger::ERROR("rwid:", rwid);
			Logger::ERROR("bestM.kind:", bestM.kind);
			Logger::ERROR("RoutesCost:", RoutesCost);
			Logger::ERROR("bestM.v:", bestM.v);
			Logger::ERROR("bestM.w:", bestM.w);
			Logger::ERROR("RoutesCost:", RoutesCost);
			for (int i : oldrv) {
				std::cout << i << ", ";
			}
			std::cout << std::endl;

			Logger::ERROR("RoutesCost:", RoutesCost);
		}

		lyhCheckTrue(penaltyafterupdatePen == penalty);
		lyhCheckTrue(costafterplus == RoutesCost);
		lyhCheckTrue(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty);
		lyhCheckTrue(oldPcOnly + oldPtwOnly + bestM.deltPen.PcOnly + bestM.deltPen.PtwOnly == PtwNoWei + Pc);
		lyhCheckTrue(oldRcost + bestM.deltPen.deltCost == RoutesCost);
		lyhCheckTrue(verify() > 0)
		lyhCheckTrue(oldpenalty + bestM.deltPen.deltPc + bestM.deltPen.deltPtw == penalty);

		#endif // LYH_CHECKING

	}

	sumRoutesCost();

	return true;
}

bool Solver::printDimacs() {

	for (int i = 0; i < rts.cnt; ++i) {
		Route& r = rts[i];
		printf("Route #%d:", i + 1);
		int pt = customers[r.head].next;
		while (pt <= input->customerNumer) {
			printf(" %d", pt);
			pt = customers[pt].next;
		}
		printf("\n");
	}

	printf("Cost %.1lf\n", double(RoutesCost) / 10);
	fflush(stdout);
	return true;
}

Solver::~Solver() {};

BKS::BKS(Input*input):
	bestSolFound(input,nullptr,nullptr),
	timer(&input->timer) 
{
	bestSolFound.penalty = DisInf;
	bestSolFound.RoutesCost = DisInf;
	lastPrCost = DisInf;
}

void BKS::reset() {
	bestSolFound.penalty = DisInf;
	bestSolFound.RoutesCost = DisInf;
}

bool BKS::updateBKSAndPrint(Solver& newSol, String opt) {

	if (bksAtRn[newSol.rts.cnt] == 0) {
		bksAtRn[newSol.rts.cnt] = newSol.RoutesCost;
	}
	else {
		bksAtRn[newSol.rts.cnt] = 
		std::min<DisType>(bksAtRn[newSol.rts.cnt], newSol.RoutesCost);
	}

	bool ret = false;

	if (newSol.RoutesCost <= bestSolFound.RoutesCost && newSol.rts.cnt <= newSol.input->vehicleNumber) {

		if (bestSolFound.input->commandLine->dimacsPrint == 1) {
		}
		else {

			auto lastRec = bestSolFound.RoutesCost;
			if (newSol.RoutesCost < bestSolFound.RoutesCost) {
				Logger::INFO("new bks cost:", newSol.RoutesCost,
					"time:" + std::to_string(timer->getRunTime()), "rn:",
					newSol.rts.cnt, "up:",
					lastRec - newSol.RoutesCost, opt);
			}
		}
		bestSolFound = newSol;
		ret = true;

	}

	if (bestSolFound.input->commandLine->dimacsPrint == 1) {
		Timer::TimePoint pt = Timer::Clock::now();
		auto ms = Timer::durationInMillisecond(lastPrintTp, pt);

		if (ms.count() >= 1 && bestSolFound.RoutesCost < lastPrCost) {

			bestSolFound.printDimacs();
			lastPrCost = bestSolFound.RoutesCost;
			lastPrintTp = Timer::Clock::now();
		}
	}

	return ret;
}

void BKS::resetBksAtRn() {
	bksAtRn.clear();
}

}