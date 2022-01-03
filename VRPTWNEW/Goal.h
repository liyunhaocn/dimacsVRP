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

	int naMA();

	bool initPopulation();

	bool TwoAlgCombine();

	bool justLocalSearch();

	bool saveSlnFile();

	bool test();
};


}

#endif // !CN_HUST_LYH_GOAL_H
