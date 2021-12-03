#ifndef LYH_EAX_H
#define LYH_EAX_H

#include<unordered_map>
#include"./Flag.h"
#include "./Solver.h"


namespace vrptwNew {

class EAX
{
public:
	/* ��֧�ֽڵ��� */
	const int supportNumNodes = 1000;

	/* �洢�����������������ݽṹ */
	bool isFirstTimeEax = true;
	struct RichEdge;
	int maxNumRichEdges; // �������洢�ı���
	int curSize = 0; // `richEdges` ��һ�η���� ID
	Vec<RichEdge> richEdges; // �洢˫�����б���Ϣ
	Vec<Vec<int>> adjNodeTable; // ��ǰ GAB ��ÿ���ڵ���ڽӽڵ��б�

	UnorderedMap<int, int>
		papbEMap, // ����
		GabEMap, // Gab
		papbNotGabEMap; // �� Gab

	ConfSet paPriE; // pa ����
	ConfSet pbPriE;// pb ���� 
	// ӳ�� `code` �� `richEdges` �� ID

	Vec<Vec<int>> abCycleSet; // һ�ηֽ� GAB �õ��� AB-Cycle ����
	Vec<Vec<int>> 
		multiple, & cycles = multiple; 
	// ��ηֽ� GAB �õ����������� AB-Cycle ����

	//Vec<bool> visited; // ���һ���ڵ��Ƿ��ѷ���
	int GabEsize = 0;
	
	int cusCnt;
	int rCnt;
	Random mr;

	EAX(int cusCnt,int rCnt,LL seed):paPriE(2 * (cusCnt + rCnt)),pbPriE(2*(cusCnt + rCnt)), mr(seed){
		
		this->cusCnt = cusCnt;
		this->rCnt = rCnt;

		richEdges = Vec<RichEdge>(2* (cusCnt + 1 + rCnt - 1) );
		adjNodeTable = Vec<Vec<int>>(cusCnt + 1, Vec<int>());
		//visited = Vec<bool>(cusCnt+1,0);
	};

	enum class Owner { UNKNOWN=-1, Pa, Pb, COMMON };

	struct Edge {
		int a;
		int b;
	};

	struct RichEdge
	{
		
		int index=-1; // �ڴ洢�ռ�� ID
		Edge e;
		int code =-1; // �ߵĹ�ϣ��
		Owner owner = Owner::UNKNOWN; // �ߵ�����Ȩ���� 0 pa 1 pb 2 pc -1 δ֪
		//int baseRouteID, goalRouteID; // ��˫���е�·�� ID
		bool visited = false; // �Ƿ��ѷ���
		int cycleID=-1; // �ñ߱����ֵ��� AB-Cycle �� ID

		RichEdge() :
			index(-1), e({ -1 ,-1}), code(-1), owner(Owner::UNKNOWN),
			//baseRouteID(-1), goalRouteID(-1),
			visited(false), cycleID(-1)
		{ }

		RichEdge(int index,Edge e,
		int code, // �ߵĹ�ϣ��
		int owner, // �ߵ�����Ȩ���� 0 pa 1 pb 2 pc -1 δ֪
		//int baseRouteID, goalRouteID; // ��˫���е�·�� ID
		int visited, // �Ƿ��ѷ���
		int cycleID) :
			index(index),e(e), code(code), owner(Owner::UNKNOWN),
			//baseRouteID(-1), goalRouteID(-1),
			visited(visited), cycleID(cycleID)
		{ }

		/* �������� */
		bool set(
			int index,
			Edge e,
			int code, // �ߵĹ�ϣ��
			Owner owner, // �ߵ�����Ȩ���� 0 pa 1 pb 2 pc -1 δ֪
			//int baseRouteID, goalRouteID; // ��˫���е�·�� ID
			int visited, // �Ƿ��ѷ���
			int cycleID) 
			
		{

			this->index = index;
			this->e = e;
			this->code = code;
			this->owner = owner;
			//baseRouteID(-1), goalRouteID(-1),
			this->visited = visited;
			this->cycleID = cycleID;
			return true;
		}
	};

	/* ����ӳ�� */
	void resetMap(UnorderedMap<int, int>& map, int size) {
		
	}

	/* ���ü��� */
	void resetSet(UnorderedSet<int>& set, int size) {
		
	}

	/* �������нڵ��������Ĳ���ջ */
	void resetArrivalSteps() {
		
	}

	/* �ӱߵ���ϣ�� */
	int toCode(int i, int j) {
		return i * supportNumNodes + j;
	}

	/* �ӹ�ϣ�뵽�� */
	Edge toEdge(int code) {
		Edge e;
		e.a = code / supportNumNodes;
		e.b = code % supportNumNodes;
		return e;
	}

	/* ���ñߵķ���״̬ */
	void setVisited(UnorderedMap<int, int>& mp, bool stat) {
	
	}

	/* ���ñߵĿ���״̬ */
	void setUsable(UnorderedMap<int, int>& mp, bool stat) {
	
	}

	/* �����µı� */
	void addRichEdge(int i, int j, Owner owner, bool unique, int routeID) {
	
	}

	/* ׼�����������������ݽṹ, ��ʹ��һ�ν���Ľ����Ч */
	void prepareDataStructures() {
	
	}

	/* ˫�ױ߼����� */
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
						re.index = curSize++;
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

		/*debug(pa.input.custCnt)
		debug(pa.rts.cnt)
		debug(curSize)*/

		auto dispDeoptE = [&](Solver & ps) {

			for (int i = 0; i < ps.rts.cnt; ++i) {
				Route& r = ps.rts[i];
				int a = 0;
				int b = ps.customers[r.head].next;
				deOut(a)debug(b)

				a = ps.customers[r.tail].pre;
				b = 0;
				deOut(a)debug(b)
			}
		};
			
		for (int i = 0; i < curSize; ++i) {
			RichEdge& re = richEdges[i];
			/*if (re.owner == Owner::Pa || re.owner == Owner::Pb) {
				++adjNodeSizes[re.e.a];
				++adjNodeSizes[re.e.b];
				adjNodeTable[re.e.a].push_back(re.index);
				adjNodeTable[re.e.b].push_back(re.index);
			}*/
			if (re.owner == Owner::Pa) {
				//++adjNodeSizes[re.e.a];
				++GabEsize;
				paPriE.ins(re.index);
				adjNodeTable[re.e.a].push_back(re.index);
			}
			else if(re.owner == Owner::Pb) {
				//++adjNodeSizes[re.e.b];
				++GabEsize;
				pbPriE.ins(re.index);
				adjNodeTable[re.e.b].push_back(re.index);
			}
		}

		return true;
	}

	/* �ֽ� GAB, ��� AB-Cycle */
	bool generateCycles(int repeatTimes = 5) {

		Vec<Vec<int>> cusVisitTime(cusCnt+1);
		Vec<int> xthCusVisitTime(cusCnt+1,0);

		//��¼һ��customer�ǵڼ������ʵ���
		//*(cusCnt + rCnt)

		Vec<int> genAbCy(2 * (cusCnt + rCnt),0);

		int genSize = 0;
		int curCus = -1;
		Owner lastEdge = Owner::UNKNOWN;

		while (paPriE.cnt > 0 || pbPriE.cnt > 0) {

			if (genSize == 0) {
				if (mr.pick(2) == 0) {

					lastEdge = Owner::Pb;
					int reIndex = paPriE.ve[mr.pick(paPriE.cnt)];
					RichEdge& globalre = richEdges[reIndex];
					curCus = globalre.e.a;
				}
				else {

					lastEdge = Owner::Pa;
					int reIndex = pbPriE.ve[mr.pick(pbPriE.cnt)];
					RichEdge& globalre = richEdges[reIndex];
					curCus = globalre.e.b;
				}
			}

#if CHECKING
			if (curCus == -1) {
				debug(curCus == -1)
					debug(curCus == -1)
			}
#endif // CHECKING

			int reIndex = -1;
			if (lastEdge == Owner::Pb) {
				// pa
				int cnt = 0;
				for (int i : adjNodeTable[curCus]) {
					RichEdge& re1 = richEdges[i];
					if (re1.visited == false) {
						if (re1.owner == Owner::Pa && re1.e.a == curCus) {
							++cnt;
							if (mr.pick(cnt) == 0) {
								reIndex = re1.index;
							}
						}
					}
				}
				paPriE.removeVal(reIndex);
			}
			else {

				int cnt = 0;
				for (int i : adjNodeTable[curCus]) {
					RichEdge& re1 = richEdges[i];
					if (re1.visited == false) {
						if (re1.owner == Owner::Pb && re1.e.b == curCus) {
							++cnt;
							if (mr.pick(cnt) == 0) {
								reIndex = re1.index;
							}
						}
					}
				}
				pbPriE.removeVal(reIndex);
			}

#if CHECKING
			if (reIndex == -1) {
				debug(reIndex == -1)
					debug(reIndex == -1)
			}
#endif // CHECKING

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
				for (int i = 0; i < cusVisitTime[curCus].size(); ++i) {
					if (genSize - cusVisitTime[curCus][i] > 0
						&& (genSize - cusVisitTime[curCus][i]) % 2 == 0) {
						abcStart = cusVisitTime[curCus][i];
					}
				}

				if (abcStart != -1) {

					Vec<int> oneCycle(genAbCy.begin() + abcStart,
						genAbCy.begin() + genSize);

					for (int i = genSize - 1; i >= abcStart; i--) {
						int ei = genAbCy[i];
						int cus = 0;
						if (richEdges[ei].owner == Owner::Pa) {
							cus = richEdges[ei].e.a;
						}
						else {
							cus = richEdges[ei].e.b;
						}
						cusVisitTime[cus].pop_back();
					}

					genSize = abcStart;
					abCycleSet.push_back(oneCycle);

				}

			}
		}

		return true;
	}

	/* �����Ƹ���Ŀͻ��ڵ�������Ϣ */
	//�Ը���Ӧ�ø��� AB-Cycle; Ŀ��·����Ϊ `params.preprocess.numRoutes`
	bool applyOneCycle(int& cycleIndex, Solver& pc) {

		Vec<int> &cycle = abCycleSet[cycleIndex];
		
		Vec<int> deopt0;
		Vec<int> deoptN;

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

	/* �Ը���Ӧ�ø��� eSet ����; */
	bool applyCycles(Vec<int>& cyclesIndex, Solver& pc) {
		for (int index : cyclesIndex) {
			applyOneCycle(index, pc);
		}
		return true;
	}

	bool removeSubring(Solver& pc) {

		ConfSet subCyCus(cusCnt+1);
		ConfSet cusSet(cusCnt+1);

		for (int i = 1; i <= cusCnt; ++i) {
			subCyCus.ins(i);
		}

		for (int i = 0; i < pc.rts.cnt; ++i) {
			vector<int> arr = pc.rPutCusInve(pc.rts[i]);
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

			Route& rv = pc.rts.getRouteByRid(pc.customers[v].routeID);

			int pt = w;
			vector<int>onesubcyle;

			while (pt != -1) {
				subCyCus.removeVal(pt);
				onesubcyle.push_back(pt);
				pt = pc.customers[pt].next;
				if (pt == w) {
					break;
				}
			}

			for (auto pt: onesubcyle) {
				pc.rInsAtPos(rv, v, pt);
				v = pt;
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

	bool doEAX(Solver& pa, Solver& pb,Solver & pc) {
			
		classifyEdges(pa, pb);
		
#if CHECKING

		if (GabEsize == 0) {
			debug(pa.updateRtsCost())
				debug(pb.updateRtsCost())
				debug(GabEsize)
				debug(GabEsize)
				return false;
		}

		if (GabEsize == 0) {
			debug(pa.updateRtsCost())
				debug(pb.updateRtsCost())
				debug(GabEsize)
				debug(GabEsize)
				return false;
		}
#endif // CHECKING

		generateCycles();
		
#if CHECKING
		if (abCycleSet.size() == 0 && GabEsize > 0) {
			debug(richEdges.size())
				debug(GabEsize)
				debug(GabEsize)
				return false;
		}
#endif // CHECKING

		struct eRchNode
		{
			int deleABCost = 0;
		};

		Solver sbest(pc);

		bool isSucceed = false;

		for (int children = 0; children < 10; ++children) {

			pc = pa;
			Vec<int> Alleset;
			Vec<int> eset;
			Alleset.reserve(abCycleSet.size());
			for (int i = 0; i < abCycleSet.size(); ++i) {
				Alleset.push_back(i);
			}

			Vec<eRchNode> arrE;
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

			//sort(Alleset.begin(), Alleset.end(),cmp);

			ShuffleCards sc;
			std::random_shuffle(Alleset.begin(),Alleset.end());

			int maxCycleCnt = mr.pick(Alleset.size()) + 1;
			//maxCycleCnt = min(Alleset.size() / 2+1, maxCycleCnt);
			
			maxCycleCnt = (children&1)==0? 1: Alleset.size()/2;
			//maxCycleCnt = 2;
			maxCycleCnt = min(Alleset.size(), maxCycleCnt);
			for (int i = 0; i < maxCycleCnt; ++i) {
				eset.push_back(Alleset[i]);
			}

			//pc.RTSDisPlay();
			applyCycles(eset, pc);
			/*debug(pc.getCusCnt())
			debug(11111111111)*/
			removeSubring(pc);

			pc.updateRtsValsAndPen();
			pc.rtsCheck();
			//println("pc.verify(): ",pc.verify());
			
			pc.updateRtsCost();
			if (pc.repair()) {

				isSucceed = true;
				if (isSucceed == false) {
					
					sbest = pc;
				}
				else {
					if (pc.RoutesCost < sbest.RoutesCost) {
						sbest = pc;
					}
				}
				
			}
			
		}
		if (isSucceed) {
			pc = sbest;
		}
		
		println("abCycleSet.size():", abCycleSet.size());
		int ecnt = 0;
		for (Vec<int> c : abCycleSet) {
			ecnt += c.size();
		}
		//debug(ecnt)
		return isSucceed;
	}

private:

};

}
#endif // !LYH_EAX_H
