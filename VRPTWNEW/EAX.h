#ifndef LYH_EAX_H
#define LYH_EAX_H

#include <unordered_map>
#include <numeric>
#include"./Flag.h"
#include "./Solver.h"

namespace hust {

class EAX
{
public:

	enum class Owner { UNKNOWN = -1, Pa, Pb, COMMON };

	struct Edge {
		int a = -1;
		int b = -1;
		Edge() :a(-1), b(-1) {}
		Edge(int aa, int bb) :a(aa), b(bb) {}
	};

	struct RichEdge
	{

		int index = -1; // 在存储空间的 ID
		Edge e;
		int code = -1; // 边的哈希码
		Owner owner = Owner::UNKNOWN; // 边的所有权属性 0 pa 1 pb 2 pc -1 未知
		//int baseRouteID, goalRouteID; // 在双亲中的路径 ID
		bool visited = false; // 是否已访问
		//int cycleID=-1; // 该边被划分到的 AB-Cycle 的 ID

		RichEdge() :
			index(-1)
			, e(-1, -1)
			, code(-1)
			, owner(Owner::UNKNOWN)
			//baseRouteID(-1), goalRouteID(-1),
			, visited(false)
			//,cycleID(-1)
		{ }

		/* 设置数据 */
		bool set(
			int index,
			Edge e,
			int code, // 边的哈希码
			Owner owner, // 边的所有权属性 0 pa 1 pb 2 pc -1 未知
			//int baseRouteID, goalRouteID; // 在双亲中的路径 ID
			int visited, // 是否已访问
			int cycleID)
		{

			this->index = index;
			this->e = e;
			this->code = code;
			this->owner = owner;
			//baseRouteID(-1), goalRouteID(-1),
			this->visited = visited;
			//this->cycleID = cycleID;
			return true;
		}
	};

	/* 可支持节点数 */
	int supportNumNodes = -1;

	int richEdgeCurSize = 0; // `richEdges` 下一次分配的 ID
	Vec<RichEdge> richEdges; // 存储双亲所有边信息
	Vec<Vec<int>> adjEdgeTable; // 当前 GAB 中每个节点的邻接rich edge列表

	// 映射 `code` 到 `richEdges` 的 ID
	UnorderedMap<int, int>	papbEMap; // 所有

	ConfSet paPriE; // pa 所有
	ConfSet pbPriE;// pb 独有 
	Vec<Vec<int>> abCycleSet; // 一次分解 GAB 得到的 AB-Cycle 集合
	int generSolNum = -1; //生成了多少解
	int repairSolNum = -1; //生成了多少解
	//Vec<bool> visited; // 标记一个节点是否已访问
	int GabEsize = 0;
	
	int eaxCusCnt = -1;
	int eaxRCnt = -1;
	int subCyNum = 0;
	int subCyCusNum = 0;
	//enum EaxStateEnum:int {
	//	noabcycle = 0,
	//	cantRepair,
	//};

	EAX(Solver& pa,Solver& pb):paPriE(2 * (pa.input.custCnt + pa.rts.cnt)),pbPriE(2 * (pb.input.custCnt + pb.rts.cnt)){
		
		this->eaxCusCnt = pa.input.custCnt;
		this->eaxRCnt = pa.rts.cnt;

		richEdges = Vec<RichEdge>(2* (this->eaxCusCnt + 1 + this->eaxRCnt - 1) );
		adjEdgeTable = Vec<Vec<int>>(this->eaxCusCnt + 1, Vec<int>());
		//visited = Vec<bool>(cusCnt+1,0);
		supportNumNodes = this->eaxCusCnt + 1;

		classifyEdges(pa, pb);
	};

	/* 从边到哈希码 */
	int toCode(int i, int j) {
		return i * supportNumNodes + j;
	}

	/* 从哈希码到边 */
	Edge toEdge(int code) {
		Edge e;
		e.a = code / supportNumNodes;
		e.b = code % supportNumNodes;
		return e;
	}

	/* 准备交叉算符所需的数据结构, 并使上一次交叉的结果无效 */
	void prepareDataStructures() {
	
	}

	/* 双亲边集分类 */
	bool classifyEdges(Solver& pa, Solver& pb) {

		auto getAllEdgeInSol = [&](Solver& ps, Owner owner) {

			for (int i = 0; i < ps.rts.cnt; ++i) {
				Route& r = ps.rts[i];
				int pt = r.head;
				int ptn = ps.customers[r.head].next;

				while (ptn != -1) {
					
					Edge e;
					e.a = pt <= ps.input.custCnt ? pt : 0;
					e.b = ptn <= ps.input.custCnt ? ptn : 0;
					
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

		getAllEdgeInSol(pa,Owner::Pa);
		getAllEdgeInSol(pb,Owner::Pb);

		auto dispDeoptE = [&](Solver & ps) {

			for (int i = 0; i < ps.rts.cnt; ++i) {
				Route& r = ps.rts[i];
				int a = 0;
				int b = ps.customers[r.head].next;
				deOut(a);
				debug(b);

				a = ps.customers[r.tail].pre;
				b = 0;
				deOut(a);
				debug(b);
			}
		};
			
		for (int i = 0; i < richEdgeCurSize; ++i) {
			RichEdge& re = richEdges[i];
			if (re.owner == Owner::Pa) {
				++GabEsize;
				paPriE.ins(re.index);
				adjEdgeTable[re.e.a].push_back(re.index);
			}
			else if(re.owner == Owner::Pb) {
				++GabEsize;
				pbPriE.ins(re.index);
				adjEdgeTable[re.e.b].push_back(re.index);
			}
		}
		return true;
	}

	/* 分解 GAB, 获得 AB-Cycle */
	bool generateCycles() {

		abCycleSet.clear();
		for (auto i = 0; i < richEdges.size(); ++i) {
			richEdges[i].visited = false;
		}

		Vec<Vec<int>> cusVisitTime(eaxCusCnt + 1);
		cusVisitTime.reserve(eaxRCnt*2);
		for (int i = 1; i <= eaxCusCnt; ++i) {
			cusVisitTime[i].reserve(4);
		}
		//记录一个customer是第几步访问到的
		//*(cusCnt + rCnt)

		Vec<int> genAbCy(2 * (eaxCusCnt + eaxRCnt),0);

		int genSize = 0;
		int curCus = -1;
		Owner lastEdge = Owner::UNKNOWN;

		auto paPriEClone = paPriE;
		auto pbPriEClone = pbPriE;

		while (paPriEClone.cnt > 0 || pbPriEClone.cnt > 0) {

			if (genSize == 0) {
				if (myRand->pick(2) == 0) {

					lastEdge = Owner::Pb;
					int reIndex = paPriEClone.ve[myRand->pick(paPriEClone.cnt)];
					RichEdge& globalre = richEdges[reIndex];
					curCus = globalre.e.a;
				}
				else {

					lastEdge = Owner::Pa;
					int reIndex = pbPriEClone.ve[myRand->pick(pbPriEClone.cnt)];
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
					//println("i:", i);
					if (re1.visited == false) {
						if (re1.owner == Owner::Pa && re1.e.a == curCus) {
							++cnt;
							if (myRand->pick(cnt) == 0) {
								reIndex = re1.index;
							}
						}
					}
				}
				//println("cnt:", cnt, "curCus", curCus);
				paPriEClone.removeVal(reIndex);
			}
			else if (lastEdge == Owner::Pa) {

				int cnt = 0;
				for (int i : adjEdgeTable[curCus]) {
					//println("i:",i);
					RichEdge& re2 = richEdges[i];
					if (re2.visited == false) {
						
						if (re2.owner == Owner::Pb && re2.e.b == curCus) {
							++cnt;
							if (myRand->pick(cnt) == 0) {
								reIndex = re2.index;
							}
						}
					}
				}
				pbPriEClone.removeVal(reIndex);
			}
			else {
				println("lastEdge:", enum_int(lastEdge));
			}

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
			else {
				Log(Log::Level::Error) << "re.owner: " << enum_int(re.owner) << std::endl;
			}

			if (cusVisitTime[curCus].size() > 0) {

				int abcStart = -1;
				int cnt = 0;
				for (int i = 0; i < cusVisitTime[curCus].size(); ++i) {
					if (genSize - cusVisitTime[curCus][i] > 0
						&& (genSize - cusVisitTime[curCus][i]) % 2 == 0) {
						abcStart = cusVisitTime[curCus][i];
						++cnt;
					}
				}

				if (abcStart != -1) {

					Vec<int> oneCycle(genAbCy.begin() + abcStart,
						genAbCy.begin() + genSize);
					
					for (int i = genSize - 1; i >= abcStart; i--) {
						int ei = genAbCy[i];
						int cus = -1;
						if (richEdges[ei].owner == Owner::Pa) {
							cus = richEdges[ei].e.a;
						}
						else if(richEdges[ei].owner == Owner::Pb){
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
		return true;
	}

	/* 仅复制个体的客户节点连接信息 */
	//对个体应用给定 AB-Cycle; 目标路径数为 `params.preprocess.numRoutes`
	bool applyOneCycle(int& cycleIndex, Solver& pc) {

		Vec<int> &cycle = abCycleSet[cycleIndex];
		
		Vec<int> deopt0;
		Vec<int> deoptN;
		
		deopt0.reserve(pc.rts.cnt);
		deoptN.reserve(pc.rts.cnt);

		int curD0 = 0;
		int curDN = 0;

		auto linkab = [&](int a,int b) {
			if (a == 0) {
				a = deopt0[curD0++];
			}
			else if (b == 0) {
				b = deoptN[curDN++];
			}

			pc.customers[a].next = b;
			pc.customers[b].pre = a;
			
		};

		auto breakab = [&](int a,int b) {

			if (a == 0) {
				a = pc.customers[b].pre;
				deopt0.push_back(a);
				pc.customers[a].next = -1;
				pc.customers[b].pre = -1;

			}else if (b == 0) {

				b = pc.customers[a].next;
				deoptN.push_back(b);
				pc.customers[a].next = -1;
				pc.customers[b].pre = -1;
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
	Vec<int> applyCycles(const Vec<int>& cyclesIndexes, Solver& pc) {
		for (int index : cyclesIndexes) {
			applyOneCycle(index, pc);
		}

		ConfSet ret(eaxCusCnt);
		for (auto& index : cyclesIndexes) {
			auto& cy = abCycleSet[index];
			for (auto& reIndex : cy) {
				auto& re = richEdges[reIndex];
				if (re.owner == Owner::Pa) {
					ret.ins(re.e.a);
				}
				else {
					ret.ins(re.e.b);
				}
			}
		}
		int n = ret.cnt;
		//debug(n);
		for (int i = 0; i < n ; ++i) {
			int v = ret.ve[i];
			for (int wpos = 0; wpos < cfg->applyCyclesNextNeiBroad; ++wpos) {
				int w = pc.input.allCloseOf[v][wpos];
				ret.ins(w);
			}
		}
		//debug(ret.cnt);
		return Vec<int>(ret.ve.begin(), ret.ve.begin() + ret.cnt);
	}

	int removeSubring(Solver& pc) {

		ConfSet subCyCus(eaxCusCnt +1);
		ConfSet cusSet(eaxCusCnt +1);

		for (int i = 1; i <= eaxCusCnt; ++i) {
			subCyCus.ins(i);
		}

		for (int i = 0; i < pc.rts.cnt; ++i) {
			Vec<int> arr = pc.rPutCusInve(pc.rts[i]);
			for (int c : arr) {
				subCyCus.removeVal(c);
				cusSet.ins(c);
			}
		}

		for (int i = 0; i < subCyCus.cnt; ++i) {
			pc.customers[subCyCus.ve[i]].routeID = -1;
		}

		subCyNum = 0;
		subCyCusNum = 0;
		if (subCyCus.cnt == 0) {
			return subCyNum;
		}

		while (subCyCus.cnt > 0) {
			++subCyNum;
			int w = subCyCus.ve[0];
			
			auto& close =  pc.input.allCloseOf[w];
			int v = -1;
			
			for (int vpos = 0; vpos < pc.input.custCnt; ++vpos) {
				v = close[vpos];
				if (pc.customers[v].routeID != -1) {
					break;
				}
			}

			int pt = w;
			Vec<int> onesubcyle;
			onesubcyle.reserve(eaxCusCnt);
			
			do {
				subCyCus.removeVal(pt);
				onesubcyle.push_back(pt);
				pt = pc.customers[pt].next;
				++subCyCusNum;
			} while (pt!=w);

			/*Route& rv = pc.rts.getRouteByRid(pc.customers[v].routeID);
			for (auto pt: onesubcyle) {
				pc.rInsAtPos(rv, v, pt);
				v = pt;
			}*/

			for (auto pt : onesubcyle) {
				//auto bestFitPos = pc.findBestPosInSol(pt);
				auto bestFitPos = pc.findBestPosForRuin(pt);
				pc.rInsAtPos(pc.rts[bestFitPos.rIndex], bestFitPos.pos, pt);
			}

		}
		
		return subCyNum;
	}

	template <typename Enumeration>
	auto enum_int(Enumeration const value)
		-> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}


	struct SolScore
	{
		int deleABCost = 0;
		Vec<int> newCus;
		int subcyNum = 0;
		DisType pen = 0;
		DisType cost = 0;
	};

	Vec<int> getRepartOrder(Vec<SolScore>& solScores) {

		Vec<int> repairOrder(generSolNum, 0);
		auto cmp = [&](int a, int b) ->bool {
			if (solScores[a].subcyNum == solScores[b].subcyNum) {
				return solScores[a].cost + solScores[a].pen <
					solScores[b].cost + solScores[b].pen;
			}
			else {
				return solScores[a].subcyNum < solScores[b].subcyNum;
			}
			return true;
		};
		std::iota(repairOrder.begin(), repairOrder.end(), 0);
		std::sort(repairOrder.begin(), repairOrder.end(), cmp);
		return repairOrder;
	}

	Vec<int> doNaEAX(Solver& pa, Solver& pb,Solver & pc) {
		
		generateCycles();
		repairSolNum = 0;

		if (abCycleSet.size() == 0) {
			//println("abCycleSet.size() == 0");
			return {};
		}

		int abcyNum = abCycleSet.size();
		// 
		//TODO[lyh][001]:最多放置多少个abcycle[2,(abcyNum+1)/2]
		generSolNum = std::min<int>(abcyNum, 2);
		auto& choseCyOrder = myRandX->getMN(abCycleSet.size(), generSolNum);

		Vec<hust::Solver> pool;
		pool.reserve(generSolNum);

		Vec<Vec<int> > newCuses;
		newCuses.reserve(generSolNum);

		Vec<SolScore> solScores(generSolNum);
		for (int i = 0; i < generSolNum; ++i) {
			pc = pa;
			Vec<int> eset = { choseCyOrder [i]};
			solScores[i].newCus = applyCycles(eset, pc);
			solScores[i].subcyNum = removeSubring(pc);
			pc.reCalRtsCostAndPen();
			pool.push_back(pc);
			solScores[i].pen = pc.penalty;
			solScores[i].cost = pc.RoutesCost;
		}

		Vec<int> repairOrder = getRepartOrder(solScores);

		int retIndex = -1;

		int index = repairOrder.front();
		auto& soltemp = pool[index];
		auto repRet = soltemp.repair();
		if (repRet.isRepair) {
			++repairSolNum;
			retIndex = index;
			auto& arr = solScores[index].newCus;
			arr.insert(arr.end(), repRet.cuses.begin(), repRet.cuses.end());
		}

		if (retIndex >= 0) {
			pc = pool[retIndex];
			
			return solScores[retIndex].newCus;
		}
		else {
			//
			pc = pa;
			return {};
		}
		return {};
	}

	Vec<int> doPrEAX(Solver& pa, Solver& pb, Solver& pc) {

		generateCycles();
		repairSolNum = 0;

		Vec<int> ret;
		if (abCycleSet.size() <=2 ) {
			//println("abCycleSet.size() <=2");
			return {};
		}

		pc = pa;
		generSolNum = 0;

		int abcyNum = abCycleSet.size();
		
		Vec<std::set<int>> cusInAbcy;

		//TODO[lyh][001]:最多放置多少个abcycle[2,(abcyNum+1)/2]
		generSolNum = (abcyNum + 1) / 2 - 2 + 1;
		//generSolNum = std::min<int>(generSolNum,3);
		auto& choseCyOrder = myRandX->getMN(abCycleSet.size(), generSolNum);

		Vec<hust::Solver> pool;
		pool.reserve(generSolNum);
		
		Vec<Vec<int> > newCuses;
		newCuses.reserve(generSolNum);

		Vec<SolScore> solScores(generSolNum);
		for (int i = 0; i < generSolNum; ++i) {
			pc = pa;
			Vec<int> eset = Vec<int>(choseCyOrder.begin(), choseCyOrder.begin()+i+2);
			solScores[i].newCus = applyCycles(eset , pc);
			solScores[i].subcyNum = removeSubring(pc);
			pc.reCalRtsCostAndPen();
			pool.push_back(pc);
			solScores[i].pen = pc.penalty;
			solScores[i].cost = pc.RoutesCost;
		}

		Vec<int> repairOrder = getRepartOrder(solScores);

		int retIndex = -1;

		int callRepairNum = std::min<int>(4, repairOrder.size());

		for (int i = 0;i< callRepairNum; ++i) {
			int index = repairOrder[i];
			auto& soltemp = pool[index];
			auto repRet = soltemp.repair();
			if (repRet.isRepair) {
				++repairSolNum;
				auto& arr = solScores[index].newCus;
				arr.insert(arr.end(), repRet.cuses.begin(), repRet.cuses.end());
				if (repairSolNum == 1) {
					retIndex = index;
					break;
				}
				else {
					if (soltemp.RoutesCost < pool[retIndex].RoutesCost) {
						retIndex = index;
					}
				}
			}
			/*Log(Log::Info) 
				<< " index: "<< index  
				<< " subcy: "<< solScores[index].subcyNum
				<< " pc+cost: "<< solScores[index].pen + solScores[index].cost 
				<< " isrepair: " << isRepair << std::endl;*/
			
		}
		//println("repairSolNum:",repairSolNum);

		if (retIndex >= 0) {
			pc = pool[retIndex];
			return solScores[retIndex].newCus;
		}
		else {
			//
			pc = pa;
			return {};
		}
		std::cout << "choseCyNum: " << generSolNum << " choseI: " << retIndex << std::endl;

		return {};
	}

private:

};




}
#endif // !LYH_EAX_H
