#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	Vec<Vec<bool>> eaxTabuTable;

	Vec<Solver> pool;

	Goal();

	DisType getMinRtCostInPool();

	DisType doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbOnePop(int i);

	int naMA();

	Vec<int> getNotTabuPaPb();

	Vec<int> getpairOfPaPb();

	bool initPopulation();

	bool saveSlnFile();

	int callSimulatedannealing();

	bool test();

	int TwoAlgCombine();

};


}

#endif // !CN_HUST_LYH_GOAL_H
