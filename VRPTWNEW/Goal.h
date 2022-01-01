#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	int sintefRecRN = 0;
	DisType dimacsRecRL = 0;
	DisType dimacsRecRN = 0;
	DisType sintefRecRL = 0;
	//  TODO[lyh][0]:提交之前要去掉
	DisType naRecRL = 0;

	Vec<Vec<LL>> eaxYearTable;

	Vec<Solver> pool;

	Timer gloalTimer;

	Goal();

	int doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbThePop();

	int naMA(Solver& pa, Solver& pb);

	bool initPopulation();

	bool TwoAlgCombine();

	bool justLocalSearch();

	bool saveSlnFile();

	bool test();
};


}

#endif // !CN_HUST_LYH_GOAL_H
