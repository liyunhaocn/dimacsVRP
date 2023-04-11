
#ifndef CN_HUST_LYH_ALGORITHM_PARAMETERS_H
#define CN_HUST_LYH_ALGORITHM_PARAMETERS_H

#include "Common.h"
#include "Util.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct AlgorithmParameters {
	
	//static Vector<int> nieborMoves;
	int initSetSize = 50;
	int populationSizeMin = 4;
	int populationSizeMax = 50;

	int maxStagnationIterOfRepair = 200;

	int minKmax = 1;
	int maxKmax = 4;

	int eamaEaxCh = 20;
	double abCycleWinkRate = 0.99; // 100

	int perturbNeiborRange = 60;
	
	int perturbIrand = 400;

	int neiborSizeMin = 20;
	int neiborSizeMax = 50;

	double ruinSplitRate = 0.05; // %100 means ruinSplitRate%
	int ruinLmax = 20;
	//(ruinLmax+1)/2
	
	int ruinC_Min = 15;
	int ruinC_Max = 35;
	int ruinWinkRate = 0.90; // 100
	int initWinkRate = 0.80; // 100

	int customersWeight1 = 1;
	int customersWeight2 = 2;

	void displayInfo();

	void check(int cusCnt);
	
	int psizemulpsum = 1;
	double broaden = 1.2;
	int broadenWhenPos_0 = 20;
	Vector<int> neiborRange = { 10,20 };
	int neiMoveKind = 16;
	int outNeiborSize = 50;

	int ruinC_ = 15;
	int weightUpStep = 1;
	int yearTabuLen = 10;
	int yearTabuRand = 10;
	// 1000 step:C2_8_7 took 20s C1_6_6 took 6s
	int ejectLSMaxIter = 50;

    int rateOfDynamicInAndOut = 0.50;

	~AlgorithmParameters() {}
};

    void to_json(Json& j, const AlgorithmParameters& aps);
    void from_json(const Json& j, AlgorithmParameters& aps);

}

#endif // !CN_HUST_LYH_ALGORITHM_PARAMETERS_H
