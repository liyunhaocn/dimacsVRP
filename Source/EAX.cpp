
#include <unordered_map>
#include <numeric>

#include "EAX.h"

namespace hust {

EAX::EAX(Solver& pa, Solver& pb) :
	pa(&pa),pb(&pb),
	paPriE(2 * (pa.input->customerNumer + pa.rts.cnt+1)), 
	pbPriE(2 * (pb.input->customerNumer + pb.rts.cnt+1)),
	random(&pa.input->random),
	randomx(&pa.input->randomx),
	aps(pa.aps),
	bks(pa.bks)
{

	this->eaxCusCnt = pa.input->customerNumer;
	this->eaxRCnt = pa.rts.cnt;

	this->richEdges = Vector<RichEdge>(2 * (this->eaxCusCnt + 1 + this->eaxRCnt));
	this->adjEdgeTable = Vector<Vector<int>>(this->eaxCusCnt + 1, Vector<int>());
	//visited = Vector<bool>(cusCnt+1,0);
	this->supportNumNodes = this->eaxCusCnt + 1;

#if LYH_CHECKING
	if (pa.rts.cnt != pb.rts.cnt) {
		Logger::ERROR(pa.rts.cnt);
		Logger::ERROR(pb.rts.cnt);
		Logger::ERROR(pa.rts.cnt != pb.rts.cnt);
	}
#endif // LYH_CHECKING

	generateGab();
};

/* 从边到哈希码 */
int EAX::toCode(int i, int j) {
	return i * supportNumNodes + j;
}

/* 从哈希码到边 */
EAX::Edge EAX::toEdge(int code) {
	Edge e;
	e.a = code / supportNumNodes;
	e.b = code % supportNumNodes;
	return e;
}

/* 双亲边集分类 */
void EAX::generateGab() {

	auto getAllEdgeInSol = [&](Solver& ps, Owner owner) {

		for (int i = 0; i < ps.rts.cnt; ++i) {
			Route& r = ps.rts[i];
			int pt = r.head;
			int ptn = ps.customers[r.head].next;

			while (ptn != -1) {

				Edge e;
				e.a = pt <= ps.input->customerNumer ? pt : 0;
				e.b = ptn <= ps.input->customerNumer ? ptn : 0;

				int code = toCode(e.a, e.b);

				if (papbEMap.count(code) > 0) {
					int index = papbEMap[code];
					richEdges[index].owner = Owner::COMMON;
				}
				else {

					RichEdge re;
					re.owner = owner;
					re.e = e;
					re.code = code;
					re.index = richEdgeCurSize++;
					papbEMap[re.code] = re.index;
					richEdges[re.index] = re;
				}

				pt = ptn;
				ptn = ps.customers[ptn].next;
			}
		}
	};

	getAllEdgeInSol(*pa, Owner::Pa);
	getAllEdgeInSol(*pb, Owner::Pb);

	#if 0
	auto dispDeoptE = [&](Solver& ps) {

		for (int i = 0; i < ps.rts.cnt; ++i) {
			Route& r = ps.rts[i];
			int a = 0;
			int b = ps.customers[r.head].next;
			deOut(a);
			debug(b);

			a = ps.customers[r.tail].prev;
			b = 0;
			deOut(a);
			debug(b);
		}
	};
	#endif // 0

	for (int i = 0; i < richEdgeCurSize; ++i) {
		RichEdge& re = richEdges[i];
		if (re.owner == Owner::Pa) {
			++GabEsize;
			paPriE.insert(re.index);
			adjEdgeTable[re.e.a].push_back(re.index);
		}
		else if (re.owner == Owner::Pb) {
			++GabEsize;
			pbPriE.insert(re.index);
			adjEdgeTable[re.e.b].push_back(re.index);
		}
	}

	#if LYH_CHECKING
	if (paPriE.cnt != pbPriE.cnt) {
		Logger::ERROR("paPriE.cnt != pbPriE.cnt:", paPriE.cnt != pbPriE.cnt);
		Logger::ERROR("pa->rts.cnt:", pa->rts.cnt);
		Logger::ERROR("pb->rts.cnt:", pb->rts.cnt);
	}
	#endif // LYH_CHECKING
}

/* 分解 GAB, 获得 AB-Cycle */
void EAX::generateCycles() {

	tabuCyIds.clear();
	abCycleSet.clear();

	for (auto i = 0; i < static_cast<int>(richEdges.size()); ++i) {
		richEdges[i].visited = false;
	}

	Vector<Vector<int>> cusVisitTime(eaxCusCnt + 1);
	//TODO[-1]bug!!!!!
	//cusVisitTime.reserve(eaxRCnt * 2);
	for (int i = 1; i <= eaxCusCnt; ++i) {
		cusVisitTime[i].reserve(4);
	}
	//记录一个customer是第几步访问到的
	//*(cusCnt + rCnt)

	Vector<int> genAbCy(2 * (eaxCusCnt + eaxRCnt), 0);

	int genSize = 0;
	int curCus = -1;
	Owner lastEdge = Owner::UNKNOWN;

	auto paPriEClone = paPriE;
	auto pbPriEClone = pbPriE;

	while (paPriEClone.cnt > 0 || pbPriEClone.cnt > 0) {

		if (genSize == 0) {
			if (random->pick(2) == 0) {

				lastEdge = Owner::Pb;
				int reIndex = paPriEClone.ve[random->pick(paPriEClone.cnt)];
				RichEdge& globalre = richEdges[reIndex];
				curCus = globalre.e.a;
			}
			else {

				lastEdge = Owner::Pa;
				int reIndex = pbPriEClone.ve[random->pick(pbPriEClone.cnt)];
				RichEdge& globalre = richEdges[reIndex];
				curCus = globalre.e.b;
			}
		}

		int reIndex = -1;

		if (lastEdge == Owner::Pb) {
			// pa
			int cnt = 0;
			for (int i : adjEdgeTable[curCus]) {
				RichEdge& re1 = richEdges[i];

				if (re1.visited == false) {
					if (re1.owner == Owner::Pa && re1.e.a == curCus) {
						++cnt;
						if (random->pick(cnt) == 0) {
							reIndex = re1.index;
						}
					}
				}
			}
			paPriEClone.removeVal(reIndex);
		}
		else if (lastEdge == Owner::Pa) {

			int cnt = 0;
			for (int i : adjEdgeTable[curCus]) {

				RichEdge& re2 = richEdges[i];
				if (re2.visited == false) {
					if (re2.owner == Owner::Pb && re2.e.b == curCus) {
						++cnt;
						if (random->pick(cnt) == 0) {
							reIndex = re2.index;
						}
					}
				}
			}
			pbPriEClone.removeVal(reIndex);
		}

		#if LYH_CHECKING
		else {
			Logger::INFO("lastEdge: is not pa pb");
		}

		if (reIndex == -1) {
			Logger::INFO("reIndex == -1");
		}
		#endif // LYH_CHECKING

			

		RichEdge& re = richEdges[reIndex];
		re.visited = true;

		cusVisitTime[curCus].push_back(genSize);
		genAbCy[genSize++] = reIndex;

		if (re.owner == Owner::Pa) {
			lastEdge = Owner::Pa;
			curCus = re.e.b;
		}
		else if (re.owner == Owner::Pb) {
			lastEdge = Owner::Pb;
			curCus = re.e.a;
		}

		#if LYH_CHECKING
		else {
			Logger::INFO("re.owner lastEdge: is not pa pb");
		}
		#endif // LYH_CHECKING

		if (cusVisitTime[curCus].size() > 0) {

			int abcStart = -1;
			int cnt = 0;
			for (int i = 0; i < static_cast<int>(cusVisitTime[curCus].size()); ++i) {
				if (genSize - cusVisitTime[curCus][i] > 0
					&& (genSize - cusVisitTime[curCus][i]) % 2 == 0) {
					abcStart = cusVisitTime[curCus][i];
					++cnt;
				}
			}

			if (abcStart != -1) {

				Vector<int> oneCycle(genAbCy.begin() + abcStart,
					genAbCy.begin() + genSize);

				for (int i = genSize - 1; i >= abcStart; i--) {
					int ei = genAbCy[i];
					int cus = -1;
					if (richEdges[ei].owner == Owner::Pa) {
						cus = richEdges[ei].e.a;
					}
					else if (richEdges[ei].owner == Owner::Pb) {
						cus = richEdges[ei].e.b;
					}
					cusVisitTime[cus].pop_back();
				}

				if (abcStart > 0)lastEdge = richEdges[genAbCy[abcStart - 1]].owner;
				genSize = abcStart;

				abCycleSet.push_back(oneCycle);
			}
		}
	}

	//TODO: 这玩意只在生成abcy换之后才发生变化 所以放在这里
	getUnionArr();
}

/* 仅复制个体的客户节点连接信息 */
//对个体应用给定 AB-Cycle; 目标路径数为 `params.preprocess.numRoutes`
bool EAX::applyOneCycle(int& cycleIndex, Solver& pc) {

	Vector<int>& cycle = abCycleSet[cycleIndex];

	Vector<int> deopt0;
	Vector<int> deoptN;

	deopt0.reserve(pc.rts.cnt);
	deoptN.reserve(pc.rts.cnt);

	int curD0 = 0;
	int curDN = 0;

	auto linkab = [&](int a, int b) {
		if (a == 0) {
			a = deopt0[curD0++];
		}
		else if (b == 0) {
			b = deoptN[curDN++];
		}

		pc.customers[a].next = b;
		pc.customers[b].prev = a;

	};

	auto breakab = [&](int a, int b) {

		if (a == 0) {
			a = pc.customers[b].prev;
			deopt0.push_back(a);
			pc.customers[a].next = -1;
			pc.customers[b].prev = -1;

		}
		else if (b == 0) {

			b = pc.customers[a].next;
			deoptN.push_back(b);
			pc.customers[a].next = -1;
			pc.customers[b].prev = -1;
		}
	};

	for (int i : cycle) {
		RichEdge& riche = richEdges[i];
		if (riche.owner == Owner::Pa) {
			breakab(riche.e.a, riche.e.b);
		}
	}

	for (int i : cycle) {
		RichEdge& riche = richEdges[i];
		if (riche.owner == Owner::Pb) {
			linkab(riche.e.a, riche.e.b);
		}
	}

	return true;
}

/* 对个体应用给定 eSet 集合; */
bool EAX::applyCycles(const Vector<int>& cyclesIndexes, Solver& pc) {
	for (int index : cyclesIndexes) {
		applyOneCycle(index, pc);
	}
	return true;
}

bool updateBestPos(Solver::Position& ret,Solver::Position& temp) {

	//if (temp.cost + 0*temp.pen < ret.cost + 0*ret.pen) {
	//	ret = temp;
	//	return true;
	//}
	//return false;

	if (temp.pen < ret.pen) {
		ret = temp;
		return true;
	}
	else if(temp.pen == ret.pen){
		if (temp.cost < ret.cost) {
			ret = temp;
			return true;
		}
		else {
			return false;
		}
	} 
	return false;
};

Solver::Position EAX::findBestPosRemoveSubtour(Solver& pc, int w, int wj, DisType deInSub) {

	auto& rtsIndexOrder = randomx->getMN(pc.rts.cnt, pc.rts.cnt);

	std::sort(rtsIndexOrder.begin(), rtsIndexOrder.end(), [&](int x,int y) {
		return 	pc.rts[x].rQ < pc.rts[y].rQ;
	});

	Solver::Position ret;

	for (int i : rtsIndexOrder) {

		Route& rt = pc.rts[i];

		int v = rt.head;
		int vj = pc.customers[v].next;

		DisType oldrPc = pc.rts[i].rPc;
		DisType rPc = std::max<DisType>(0, rt.rQ + deInSub - pc.input->vehicleCapacity);
		rPc = rPc - oldrPc;

		//if (rPc > ret.pen) {
		//	continue;
		//}

		while (v != -1 && vj != -1) {

			DisType oldrPtw = pc.rts[i].rPtw;

			pc.customers[v].next = wj;
			pc.customers[wj].prev = v;

			pc.customers[w].next = vj;
			pc.customers[vj].prev = w;

			DisType rPtw = pc.getaRangeOffPtw(v, vj);

			rPtw = rPtw - oldrPtw;

			pc.customers[v].next = vj;
			pc.customers[vj].prev = v;

			pc.customers[w].next = wj;
			pc.customers[wj].prev = w;

			DisType cost = 
				pc.input->getDisof2(v,wj) + pc.input->getDisof2(w,vj)
				- pc.input->getDisof2(v,vj) -pc.input->getDisof2(w,wj);
			//int year = (*yearTable)[reCusNo(w)][reCusNo(v)] + (*yearTable)[reCusNo(w)][reCusNo(vj)];
			//year >>= 1;

			Solver::Position posTemp;
			posTemp.rIndex = i;
			posTemp.cost = cost;
			posTemp.pen = rPtw + rPc;
			posTemp.pos = v;
			//posTemp.year = year;
			//posTemp.secDis = abs(input->datas[w].polarAngle - input->datas[v].polarAngle);
			// TODO[-1]:移除子环的方式，目标函数

			updateBestPos(ret,posTemp);

			v = vj;
			vj = pc.customers[vj].next;
		}
	}

	return ret;
}

int EAX::removeSubring(Solver& pc) {

	ConfSet subCyCus(eaxCusCnt + 1);
	ConfSet cusSet(eaxCusCnt + 1);

	for (int i = 1; i <= eaxCusCnt; ++i) {
		subCyCus.insert(i);
	}

	for (int i = 0; i < pc.rts.cnt; ++i) {
		Vector<int> arr = pc.rPutCustomersInVector(pc.rts[i]);
		for (int c : arr) {
			subCyCus.removeVal(c);
			cusSet.insert(c);
		}
	}

	subCyNum = 0;
	subCyCusNum = 0;
	if (subCyCus.cnt == 0) {
		return subCyNum;
	}

	while (subCyCus.cnt > 0) {
		++subCyNum;
		int subbegin = subCyCus.ve[0];

		int w = subbegin;
		DisType demandInSub = 0;

		Solver::Position ret;
		do {
			subCyCus.removeVal(w);
			demandInSub += pc.input->datas[w].DEMAND;

			int wj = pc.customers[w].next;
			w = wj;
			++subCyCusNum;
		} while (w != subbegin);

		w = subbegin;
		int retW = -1;

		//sameCnt = 0;
		do {

			int wj = pc.customers[w].next;
			auto posTemp = findBestPosRemoveSubtour(pc, w, wj, demandInSub);

			if (updateBestPos(ret, posTemp)) {
				retW = w;
			}

			w = wj;
		} while (w != subbegin);

		int v = ret.pos;
		int vj = pc.customers[v].next;
		w = retW;
		int wj = pc.customers[w].next;

		Route& r = pc.rts.getRouteByRouteId(pc.customers[v].routeId);
		pc.customers[v].next = wj;
		pc.customers[wj].prev = v;

		pc.customers[w].next = vj;
		pc.customers[vj].prev = w;

		pc.rReCalCusNumAndSetCusrIdWithHeadrId(r);
		pc.reCalRtsCostAndPen();

	}

	return subCyNum;
}

UnorderedSet<int> EAX::getCusInOneCycle(int cyIndex) {

	auto& reSet = abCycleSet[cyIndex];
	UnorderedSet<int> cusInCycle;
	for (int reIndex : reSet) {
		auto& re = richEdges[reIndex];
		if (re.owner == Owner::Pa) {
			cusInCycle.insert(re.e.a);
		}
		else {
			cusInCycle.insert(re.e.b);
		}
	}
	return cusInCycle;
}

bool EAX::whetherTwoSetIntersect(const UnorderedSet<int>& a, const UnorderedSet<int>& b) {
	const auto& smallSet = a.size() < b.size() ? a : b;
	const auto& bigSet = a.size() >= b.size() ? a : b;
	for (auto& i : smallSet) {
		if (bigSet.count(i) > 0) {
			return true;
		}
	}
	return false;
}

void EAX::getUnionArr() {

	int n = static_cast<int>(abCycleSet.size());
	Union u(n);
	Vector<UnorderedSet<int>> setVe(n);

	for (int cyIndex = 0; cyIndex < n; ++cyIndex) {
		setVe[cyIndex] = getCusInOneCycle(cyIndex);
	}
	abCyAdj.clear();
	abCyAdj.resize(n);

	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			if (whetherTwoSetIntersect(setVe[i], setVe[j])) {
				u.merge(i, j);
				abCyAdj[i].push_back(j);
				abCyAdj[j].push_back(i);
			}
		}
	}

	UnorderedMap<int, Vector<int>>mp;

	for (int i = 0; i < n; ++i) {
		int a = u.find(i);
		mp[a].push_back(i);
	}

	unionArr.clear();
	for (auto it : mp) {
		unionArr.push_back(it.second);
	}
}

//TODO[!]eax 返回值的含义
/*
	返回值的含义：-1 全部被禁忌了没有能选的abcy，但是可以再次重新生成abcy
				1 正常修复
				0 没能正常修复
*/

int EAX::doNaEAX(Solver& pc) {

	repairSolNum = 0;
	generSolNum = 1;

	static int cnt = 0;
	static int all = 0;
	//printve(tabuCyIds);
	//debug(chooseIndex);
	++all;

	choosecyIndex = -1;
	auto& order = randomx->getMN(static_cast<int>(abCycleSet.size()), static_cast<int>(abCycleSet.size()));
	if (tabuCyIds.count(order[0]) > 0) {
		++cnt;
	}
	for (int i : order) {
		if (tabuCyIds.count(i) == 0) {
			choosecyIndex = i;
		}
	}
	//Logger::INFO("all:", all, "cnt:", cnt);

	if (choosecyIndex == -1) {
		return -1;
	}

	applyCycles({ choosecyIndex }, pc);

	pc.reCalRtsCostAndPen();
	removeSubring(pc);
	pc.reCalRtsCostAndPen();
		
	//TODO[0]:这里考虑是否可以在没有子换的情况下再禁忌
	if (aps->abCycleWinkRate == 100) {
		tabuCyIds.insert(choosecyIndex);
	}
	else {
		if (subCyCusNum == 0) {
			tabuCyIds.insert(choosecyIndex);
		}
	}
	
	if (pc.repair()) {

		bks->updateBKSAndPrint(pc,"doNaEAX after repair");
		if (pc.RoutesCost == pa->RoutesCost) {
				
			/*for (auto id : tabuCyIds) {
				Logger::INFO("id:", id);
			}*/
			return 0;
		}
		++repairSolNum;
		return 1;
	}
	else {
		return 0;
	}
	return 0;
}

int EAX::doPrEAX(Solver& pc) {

	//generateCycles();
	repairSolNum = 0;
	generSolNum = 1;

	//TODO[lyh][001]:最多放置多少个abcycle[2,(abcyNum)/2],pick 是开区间
	int abcyNum = static_cast<int>(abCycleSet.size());

	//static ProbControl probc(input->customerNumer/2);
	////int numABCyUsed = 2;
	//int numABCyUsed = probc.getIndexBasedData(3) + 2;
	////int numABCyUsed = probc.getIndexBasedData(std::min<int>(2,abcyNum / 2 + 1) ) + 2;
	////int numABCyUsed = random->pick(2, abcyNum/2+1);
	////int numABCyUsed = random->pick(2, 10);
	//numABCyUsed = std::min<int>(numABCyUsed, abcyNum-1);

	int putMax = abcyNum/2;
	//int putMax = random->pick(2, abcyNum / 2 + 1);
	int numABCyUsed = 2;
	for (int i = 3; i <= putMax; ++i) {
		// TODO[-1]:这里可以调整 放置多少个abcy
		if (random->pick(100) < 80) {
			numABCyUsed = i;
		}
		else {
			break;
		}
	}

	int uarrNum = static_cast<int>(unionArr.size());
	Vector<int> unionIndexOrder;
	for (int i = 0; i < uarrNum; ++i) {
		if (unionArr[i].size() >= 2) {
			unionIndexOrder.push_back(i);
		}
	}
	if (unionIndexOrder.size() == 0) {
		return -1;
	}
	random->shuffleVec(unionIndexOrder);


	ConfSet cyInUnion(abcyNum);

	Vector<int> eset;

	for (int uId : unionIndexOrder) {

		//将一个并查集的所有abcyindex保存起来
		for (int cy : unionArr[uId]) {
			cyInUnion.insert(cy);
		}

		int firstCyIndex = cyInUnion.ve[random->pick(cyInUnion.cnt)];
		std::queue<int> qu;
		qu.push(firstCyIndex);
		cyInUnion.removeVal(firstCyIndex);

		while (static_cast<int>(eset.size()) < numABCyUsed && qu.size() > 0) {
			auto tp = qu.front();
			eset.push_back(tp);
			qu.pop();

			auto adjs = abCyAdj[tp];
			random->shuffleVec(adjs);
			for (int ad : adjs) {
				if (cyInUnion.pos[ad] >= 0) {
					qu.push(ad);
					cyInUnion.removeVal(ad);
				}
			}
		}

		if (static_cast<int>(eset.size()) == numABCyUsed) {
			break;
		}
	}

	//printve(eset);
	//Logger::INFO("eset.size():", eset.size(),"numABCyUsed:", numABCyUsed);

	applyCycles(eset, pc);
	pc.reCalRtsCostAndPen();
	removeSubring(pc);
	pc.reCalRtsCostAndPen();

	if (pc.repair()) {

		bks->updateBKSAndPrint(pc, "doPrEAX after repair");

		//++probc.data[numABCyUsed-2];

		if (pc.RoutesCost == pa->RoutesCost) {
			//debug("same after repair");
			return 0;
		}
		++repairSolNum;
		return 1;
	}
	else {
		return 0;
	}
	return 0;
}

int EAX::getabCyNum(Solver& pa, Solver& pb) {
	UnorderedSet<int> s;
	EAX et(pa, pb);
	et.generateCycles();
	return static_cast<int>(et.abCycleSet.size());
}

Vector<int> EAX::getDiffCusofPb(Solver&pa, Solver&pb) {

	Vector<int> ret;
	
	int custNum = pa.input->customerNumer;

	for (int c = 1; c <= custNum; ++c) {

		int pacnext = pa.customers[c].next > custNum ? 0 : pa.customers[c].next;
		int pbcnext = pb.customers[c].next > custNum ? 0 : pb.customers[c].next;

		int pacpre = pa.customers[c].prev > custNum ? 0 : pa.customers[c].prev;
		int pbcpre = pb.customers[c].prev > custNum ? 0 : pb.customers[c].prev;

		if (pacnext == pbcnext && pacpre == pbcpre) {}
		else {
			ret.push_back(c);
		}
	}

	pa.random->shuffleVec(ret);
	
	return ret;

}

}

