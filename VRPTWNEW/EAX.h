#ifndef LYH_EAX_H
#define LYH_EAX_H

#include <unordered_map>
#include <numeric>
#include"./Flag.h"
#include "./Solver.h"

namespace vrptwNew {

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
	unsigned eaxSeed = 0;
	
	//enum EaxStateEnum:int {
	//	noabcycle = 0,
	//	cantRepair,
	//};

	EAX(Solver& pa,Solver& pb,unsigned seedarg):paPriE(2 * (pa.input.custCnt + pa.rts.cnt)),pbPriE(2 * (pb.input.custCnt + pb.rts.cnt)), eaxSeed(seedarg){
		
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
				Log(Log::Level::Error) << "re.owner: " << enum_int(re.owner) << endl;
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
	Vec<int> applyCycles(Vec<int>& cyclesIndexes, Solver& pc) {
		for (int index : cyclesIndexes) {
			applyOneCycle(index, pc);
		}

		Vec<int> ret;
		ret.reserve(eaxCusCnt);
		for (auto& index : cyclesIndexes) {
			auto& cy = abCycleSet[index];
			for (auto& reIndex : cy) {
				auto& re = richEdges[reIndex];
				if (re.owner == Owner::Pa) {
					ret.push_back(re.e.a);
				}
				else {
					ret.push_back(re.e.b);
				}
			}
		}
		return ret;
	}

	bool removeSubring(Solver& pc) {

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

		while (subCyCus.cnt > 0) {

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
			int subcy = 0;
			do {
				subCyCus.removeVal(pt);
				onesubcyle.push_back(pt);
				pt = pc.customers[pt].next;
				++subcy;
			} while (pt!=w);

			/*Route& rv = pc.rts.getRouteByRid(pc.customers[v].routeID);
			for (auto pt: onesubcyle) {
				pc.rInsAtPos(rv, v, pt);
				v = pt;
			}*/

			for (auto pt : onesubcyle) {
				auto bestFitPos = pc.findBestPosInSol(pt);
				pc.rInsAtPos(pc.rts[bestFitPos.rIndex], bestFitPos.pos, pt);
			}

		}
		
		return true;
	}

	template <typename Enumeration>
	auto enum_int(Enumeration const value)
		-> typename std::underlying_type<Enumeration>::type
	{
		return static_cast<typename std::underlying_type<Enumeration>::type>(value);
	}

	Vec<int> doNaEAX(Solver& pa, Solver& pb,Solver & pc) {
		
		struct eRchNode
		{
			int deleABCost = 0;
		};

		Solver sbest(pc);

		generateCycles();

		Vec<int> ret;
		if (abCycleSet.size() == 0) {
			println("abCycleSet.size() == 0");
			return {};
		}

		int chNum = std::min<int>(abCycleSet.size(),10);

		auto& choseAbces = myRandX->getMN(chNum, chNum);

		unsigned shuseed = (eaxSeed + (myRand->pick(10000007))) % Mod;
		std::shuffle(choseAbces.begin(), choseAbces.end(), default_random_engine(shuseed));

		generSolNum = 0;
		repairSolNum = 0;

		for (int cyId: choseAbces) {

			pc = pa;

			Vec<int> eset = { cyId };
			Vec<int> retTemp = applyCycles(eset, pc);

			removeSubring(pc);

			pc.reCalRtsCostAndPen();
			++generSolNum;

			bool isRepair = pc.repair();
			if (isRepair) {
				++repairSolNum;
				if (repairSolNum == 1) {
					sbest = pc;
					ret = retTemp;
				}else if (pc.RoutesCost< sbest.RoutesCost) {
					sbest = pc;
					ret = retTemp;
				}
				/*if (mr.pick(repairSolNum) == 0) {
					sbest = pc;
					ret = retTemp;
				}*/
			}
		}

		//printve(applyabc);
		if (ret.size() > 0) {
			pc = sbest;
		}
		else {
			//println("naEax cant repair");
			pc = pa;
		}
		
		return ret;
	}

	Vec<int> doPrEAX(Solver& pa, Solver& pb, Solver& pc) {

		struct eRchNode
		{
			int deleABCost = 0;
		};

		Solver sbest(pc);

		generateCycles();

		Vec<int> ret;
		if (abCycleSet.size() == 0) {
			println("abCycleSet.size() == 0");
			return {};
		}

		generSolNum = 0;
		repairSolNum = 0;

		for (int children = 0; children < 10; ++children) {

			pc = pa;

			Vec<int> Alleset(abCycleSet.size(),0);
			std::iota(Alleset.begin(), Alleset.end(), 0);
			Vec<int> eset;
			eset.reserve(abCycleSet.size());
			
			int maxCycleCnt = -1;

			/*maxCycleCnt = myRand->pick(Alleset.size()) + 1;
			Vec<eRchNode> arrE;
			arrE.reserve(abCycleSet.size());
			for (int i = 0; i < abCycleSet.size(); ++i) {
				eRchNode ern;
				ern.deleABCost = 0;
				for (int eIndex : abCycleSet[i]) {
					RichEdge& re = richEdges[eIndex];
					if (re.owner == Owner::Pa) {
						ern.deleABCost -= pc.input.disOf[re.e.a][re.e.b];
					}
					else {
						ern.deleABCost += pc.input.disOf[re.e.a][re.e.b];
					}
				}
				arrE.push_back(ern);
			}
			auto cmp = [&](int a, int b) {
				return arrE[a].deleABCost < arrE[b].deleABCost;
			};
			sort(Alleset.begin(), Alleset.end(), cmp);
			unsigned shuseed = (eaxSeed + (myRand->pick(10000007))) % Mod;
			std::shuffle(Alleset.begin() + 1, Alleset.end(), default_random_engine(shuseed));*/

			maxCycleCnt = myRand->pick(Alleset.size()) + 1;
			unsigned shuseed = (eaxSeed + (myRand->pick(10000007))) % Mod;
			std::shuffle(Alleset.begin(), Alleset.end(), default_random_engine(shuseed));
			//maxCycleCnt = 2;
			maxCycleCnt = std::min<int>(Alleset.size(), maxCycleCnt);
			for (int i = 0; i < maxCycleCnt; ++i) {
				eset.push_back(Alleset[i]);
			}

			Vec<int> retTemp = applyCycles(eset, pc);
			removeSubring(pc);
			++generSolNum;
			pc.reCalRtsCostAndPen();
			//pc.rtsCheck();
			//pc.reCalRtsCost();
			bool isRepair = pc.repair();
			if (isRepair) {
				++repairSolNum;
				if (repairSolNum == 1) {
					sbest = pc;
					ret = retTemp;
				}
				else if (pc.RoutesCost < sbest.RoutesCost) {
					sbest = pc;
					ret = retTemp;
				}
				/*if (mr.pick(repairSolNum) == 0) {
					sbest = pc;
					ret = retTemp;
				}*/
			}
		}
		if (ret.size() > 0) {
			pc = sbest;
		}
		else {
			//println("naEax cant repair");
			pc = pa;
		}

		return ret;
	}

private:

};

}
#endif // !LYH_EAX_H
