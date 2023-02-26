#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	int populationSize = 0;
	int routeNumberLowerBound = 0;
	int routeNumberUpperBound = 0;
	UnorderedMap<int, Vector<Solver>> mapOfPopulation;
	
	YearTable yearTable;
	
	int curSearchRouteNumber = -1;

	AlgorithmParameters* aps = nullptr;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
	Input* input = nullptr;
	Timer* timer = nullptr;
	
	BKS bks;
	std::queue<int> alternativeRouteNumbers;

	Goal(Input*input);

	void updateMapOfPopulation(Solver& sol, int index);

	DisType getMinRoutesCostInPool(int rn);

	DisType doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	bool perturbOneSolution(Solver& sol);

	int EAMA(int rn);

	int gotoPopulationAtRouteNumber(int rn);

	bool fillPopulation(int rn);

	int callSimulatedannealing();

	bool test();

	bool callRouteNumberMinimization();

	void fillAlternativeRouteNumbers();
	
	int getNextRouteNumerToRun();

	int run();

	void initialMapOfPopulation();
};


}

#endif // !CN_HUST_LYH_GOAL_H
