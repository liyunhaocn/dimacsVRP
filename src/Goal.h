#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	Vec<Vec<bool>> eaxTabuTable;

	//Vec<Solver> pool;
	//Vec<Vec<Solver>> ppool;
	UnorderedMap<int, Vec<Solver>> ppool;
	//Vec<Vec<Solver>> ppool;

	int curSearchRN = -1;

	Goal();

	DisType getMinRtCostInPool();

	DisType doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbOnePop(int i);

	int naMA();

	//Vec<int> getNotTabuPaPb();
	//Vec<int> getpairOfPaPb();

	int gotoRNPop(int rn);

	bool fillPopulation(int rn);

	bool saveSlnFile();

	int callSimulatedannealing();

	bool test();

	int TwoAlgCombine();

};


}

#endif // !CN_HUST_LYH_GOAL_H
