#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	Vec<Vec<bool>> eaxTabuTable;

	int poprnLowBound = 0;
	int poprnUpBound = 0;

	UnorderedMap<int, Vec<Solver>> ppool;
	//Vec<Vec<Solver>> ppool;

	int curSearchRN = -1;

	AlgorithmParameters* aps = nullptr;
	BKS* bks = nullptr;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
	Input* input = nullptr;
	Timer* timer = nullptr;
	YearTable* yearTable = nullptr;

	Goal(
		Input*input,
		AlgorithmParameters* aps,
		BKS* bks,
		YearTable* yearTable);

	void updateppol(Solver& sol, int index);

	DisType getMinRtCostInPool(int rn);

	DisType doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbOneSol(Solver& sol);

	int naMA(int rn);

	//Vec<int> getNotTabuPaPb();
	//Vec<int> getpairOfPaPb();

	int gotoRNPop(int rn);

	bool fillPopulation(int rn);

	int callSimulatedannealing();

	bool test();

	bool experOnMinRN();

	int TwoAlgCombine();

	void getTheRangeMostHope();
};


}

#endif // !CN_HUST_LYH_GOAL_H
