#ifndef CN_HUST_LYH_GOAL_H
#define CN_HUST_LYH_GOAL_H

#include "Solver.h"

namespace hust {

struct Goal {

	int populationSize = 0;
	int routeNumberLowerBound = 0;
	int routeNumberUpperBound = 0;
	UnorderedMap<int, Vector<Solver>> mapOfPopulation;
	AlgorithmParameters* aps = nullptr;
	Input* input = nullptr;
	
	YearTable yearTable;
	BKS bks;

	int curSearchRouteNumber = -1;
	Random* random = nullptr;
	RandomX* randomx = nullptr;
	Timer* timer = nullptr;
	
	std::queue<int> alternativeRouteNumbers;

	Goal(Input*input);

	void addSolverToMapOfPopulation(Solver& sol);

	DisType getMinRoutesCostInPool(int rn);

	DisType doTwoKindEAX(Solver& pa, Solver& pb, int kind);

	void EAMA(int rn);

	int gotoPopulationAtRouteNumber(int rn);

	int callSimulatedannealing();

	void test();

	void callRouteNumberMinimization();

	void fillAlternativeRouteNumbers();
	
	int getNextRouteNumerToRun();

	int run();

    void penaltiesAndDroppingVisits();

	void initialMapOfPopulation();

	void saveBestFoundSolution();

    void exportCSVFile(String path,const Solver& solver);

    String toCsvFileLine(Solver& solver);

};


}

#endif // !CN_HUST_LYH_GOAL_H
