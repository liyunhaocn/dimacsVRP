
#ifndef CN_HUST_LYH_ALGORITHM_PARAMETERS_H
#define CN_HUST_LYH_ALGORITHM_PARAMETERS_H

#include "Flag.h"
#include "Util_Common.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct AlgorithmParameters {

	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	int weightUpStep = 1;

	int Irand = 400;
	int neiMoveKind = 16;

	int customersWeight0 = 1;
	int customersWeight1 = 2;

	int minKmax = 1;
	int maxKmax = 4;

	int yearTabuLen = 10;
	int yearTabuRand = 10;

	// TODO[0]:看需不需要频繁构造 1000 stepC2_8_7 需要20s C1_6_6 需要6s
	int ejectLSMaxIter = 200;

	//int yearTabuLen = 20;
	//int yearTabuRand = 10;

	int popSize = 4;
	int popSizeMin = 4;
	int popSizeMax = 50;
	//int repairExitStep = 50;
	int repairExitStep = 5;

	//int initFindPosPqSize = 20;
	//int findBestPosForRuinPqSize = 10;
	//int findBestPosInSolPqSize = 3;//64

	int naEaxCh = 20;
	//int naEaxCh = 20;

	//patternAdjustment参数
	int patternAdjustmentNnei = 60;
	int patternAdjustmentGetM = 10;

	// mRLLocalSearchRange
	//TODO[-1]:这里改成了40

	double broaden = 1.2;
	//int broadenWhenPos_0 = 50;
	int broadenWhenPos_0 = 20;
	int outNeiSize = 50;

	int neiSizeMin = 20;
	int neiSizeMax = 50;
	Vec<int> mRLLocalSearchRange = { 10,20 };
	//int mRLLSgetAllRange = 50;

	// ruinLocalSearch
	int ruinLocalSearchNextNeiBroad = 5;

	int ruinSplitRate = 0; // %100 means ruinSplitRate%
	
	int ruinLmax = 20;
	//(ruinLmax+1)/2
	int ruinC_ = 15;
	int ruinC_Min = 15;
	int ruinC_Max = 35;

	int ruinWinkacRate = 90; // 100
	//TODO[-1]:为初始化设置了眨眼
	int initWinkacRate = 80; // 100
	int abcyWinkacRate = 99; // 100

	int sintefRecRN = -1;
	DisType sintefRecRL = -1;
	int naRecRN = -1;
	DisType naRecRL = -1;

	int d15RecRN = -1;
	DisType d15RecRL = -1;

	int lkhRN = -1;
	DisType lkhRL = -1;
	int cmdIsopt = -1;

	std::string tag = "";

	int psizemulpsum = 0;

	void displayInfo();

	void repairByCusCnt(int cusCnt);

	//Configuration(): random(seed) {

	//}

};

}

#endif // !CN_HUST_LYH_ALGORITHM_PARAMETERS_H
