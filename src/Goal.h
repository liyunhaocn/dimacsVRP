#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	Vec<Vec<LL>> eaxYearTable;

	Vec<Solver> pool;

	Timer gloalTimer;

	Goal();

	int doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbThePop();

	int naMA(Solver& pa, Solver& pb);

	Vec<int> getpairOfPaPb();

	bool initPopulation();

	bool saveSlnFile();

	int callSimulatedannealing();

	bool test();

	int TwoAlgCombine();

};


}

#endif // !CN_HUST_LYH_GOAL_H
