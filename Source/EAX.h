
#ifndef CN_HUST_LYH_EAX_H
#define CN_HUST_LYH_EAX_H

#include "Solver.h"
#include "Util.h"

namespace hust {

struct EAX
{
public:

	enum class Owner { UNKNOWN = -1, Pa, Pb, COMMON };

	struct Edge {
		int a = -1;
		int b = -1;
		Edge() :a(-1), b(-1) {}
		Edge(int aa, int bb) :a(aa), b(bb) {}
		~Edge() {}
	};

	struct RichEdge {

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
	Vector<RichEdge> richEdges = {}; // 存储双亲所有边信息
	Vector<Vector<int>> adjEdgeTable; // 当前 GAB 中每个节点的邻接rich edge列表

	// 映射 `code` 到 `richEdges` 的 ID
	UnorderedMap<int, int>	papbEMap; // 所有

	ConfSet paPriE; // pa: all from pa
	ConfSet pbPriE;// pb 独有 
	Vector<Vector<int>> abCycleSet; // 一次分解 GAB 得到的 AB-Cycle 集合
	int generSolNum = -1; //生成了多少解
	int repairSolNum = -1; //生成了多少解
	//Vector<bool> visited; // 标记一个节点是否已访问
	int GabEsize = 0;
	
	int eaxCusCnt = -1;
	int eaxRCnt = -1;
	int subCyNum = 0;
	int subCyCusNum = 0;
	
	Vector< Vector<int> > unionArr;
	Vector< Vector<int> > abCyAdj;
	UnorderedSet<int> tabuCyIds;

	int unionIndex = -1;
	int choosecyIndex = -1;

	Random* random = nullptr;
	RandomX* randomx = nullptr;

	AlgorithmParameters* aps;
	BKS* bks;

	Solver* pa = nullptr;
	Solver* pb = nullptr;

	EAX(Solver& pa, Solver& pb);

	/* 从边到哈希码 */
	int toCode(int i, int j);

	/* 从哈希码到边 */
	Edge toEdge(int code);

	/* 双亲边集分类 */
	bool classifyEdges();

	/* 分解 GAB, 获得 AB-Cycle */
	bool generateCycles();

	/* 仅复制个体的客户节点连接信息 */
	//对个体应用给定 AB-Cycle; 目标路径数为 `params.preprocess.numRoutes`
	bool applyOneCycle(int& cycleIndex, Solver& pc);

	/* 对个体应用给定 eSet 集合; */
	bool applyCycles(const Vector<int>& cyclesIndexes, Solver& pc);

	//int sameCnt = 0;

	Solver::Position findBestPosRemoveSubtour(Solver& pc, int w, int wj, DisType deInSub);

	int removeSubring(Solver& pc);

	UnorderedSet<int> getCusInOneCycle(int cyIndex);

	bool whetherTwoSetIntersect(const UnorderedSet<int>& a, const UnorderedSet<int>& b);

	void getUnionArr();

	int doNaEAX(Solver& pc);

	int doPrEAX(Solver& pc);

	static int getabCyNum(Solver& pa, Solver& pb);
	
	static Vector<int> getDiffCusofPb(Solver& pa, Solver& pb);

};

}
#endif // !CN_HUST_LYH_EAX_H
