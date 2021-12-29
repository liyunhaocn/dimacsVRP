#ifndef vrptwNew_CONFIGURATION_H
#define vrptwNew_CONFIGURATION_H

#include "Flag.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct Configuration {

	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	int outNeiSize = 100;
	//int outNeiSize = 50;
	double broaden = 1.2;
	int broadenWhenPos_0 = 50;

	int weightUpStep = 1;

	int squGrea0AC = 1;
	int squGrea0Range = 1;
	int squRandomNeiSize = 5;

	int Irand = 200;
	int runTimer = 1800; //s
	int neiMoveKind = 16;

	int Pwei0 = 1;
	int Pwei1 = 2;

	int minKmax = 2;
	int maxKmax = 4;

	bool breakRecord = 0;

	int yearTabuLen = 5;
	int yearTabuRand = 5;
	
	//int yearTabuLen = 20;
	//int yearTabuRand = 10;

	int popSize = 5;
	//int repairExitStep = 50;
	int repairExitStep = 50;
	int initFindPosPqSize = 20;
	int findBestPosForRuinPqSize = 10;
	int findBestPosInSolPqSize = 64;//64
	int naEaxCh = 10;

	//TODO[lyh][2]: patternAdjustment²ÎÊý
	int patternAdjustmentNnei = 60;
	int patternAdjustmentGetM = 10;

	//TODO[lyh][2]:naRepairGetMoves
	int naRepairGetMovesNei = 50;

	// TODO[lyh][2] mRLLocalSearchRange
	Vec<int> mRLLocalSearchRange = { 10,50 };
	//int mRLLSgetAllRange = 50;

	//ruinLocalSearch
	int ruinLocalSearchNextNeiBroad = 5;

	int ruinSplitRate = 25; // %100 means ruinSplitRate%
	
	int ruinLmax = 20;
	
	//(ruinLmax+1)/2
	int ruinC_ = 15;

	int ruinWinkacRate = 95; // 100
	int abcyWinkacRate = 100; // 100

	void show();

	void repairByCusCnt(int cusCnt);
};

}

#endif // !vrptwNew_CONFIGURATION_H
