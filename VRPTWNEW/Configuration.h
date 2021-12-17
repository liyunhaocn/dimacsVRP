#ifndef vrptwNew_CONFIGURATION_H
#define vrptwNew_CONFIGURATION_H

#include "./Flag.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct Configuration {

	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	int outNeiSize = 100;
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
	int repairExitStep = 40;
	int initFindPosPqSize = 5;
	int findBestPosForRuinPqSize = 10;
	int naEaxCh = 10;

	//TODO[lyh][2]: patternAdjustment参数
	int patternAdjustmentNnei = 60;
	int patternAdjustmentGetM = 10;

	//TODO[lyh][2]:naRepairGetMoves
	int naRepairGetMovesNei = 20;

	// TODO[lyh][2] mRLLocalSearchRange
	Vec<int> mRLLocalSearchRange = { 10,50 };
	int mRLLSgetAllRange = 50;

	//ruinLocalSearch
	int ruinLocalSearchNextNeiBroad = 50;

	//applyCycles
	int applyCyclesNextNeiBroad = 50;

	void show() {

		debug(breakRecord);
		debug(Pwei0);
		debug(Pwei1);
		debug(minKmax);
		debug(maxKmax);
	}

	void repairByCusCnt(int cusCnt) {

		outNeiSize = std::min<int>(cusCnt, outNeiSize);
		broadenWhenPos_0 = std::min<int>(cusCnt, broadenWhenPos_0);

		//TODO[lyh][2]: patternAdjustment参数
		patternAdjustmentNnei = std::min<int>(cusCnt, patternAdjustmentNnei);
		patternAdjustmentGetM = std::min<int>(cusCnt, patternAdjustmentGetM);

		//TODO[lyh][2]:naRepairGetMoves
		naRepairGetMovesNei = std::min<int>(cusCnt, naRepairGetMovesNei);

		// TODO[lyh][2] mRLLocalSearchRange
		//mRLLocalSearchRange = { 10,50 };
		mRLLocalSearchRange[0] = std::min<int>(cusCnt, mRLLocalSearchRange[0]);
		mRLLocalSearchRange[1] = std::min<int>(cusCnt, mRLLocalSearchRange[1]);
		mRLLSgetAllRange = std::min<int>(cusCnt, mRLLSgetAllRange);

		//ruinLocalSearch
		ruinLocalSearchNextNeiBroad = std::min<int>(cusCnt, ruinLocalSearchNextNeiBroad);

		//applyCycles
		applyCyclesNextNeiBroad = std::min<int>(cusCnt, applyCyclesNextNeiBroad);
	}
};

}

#endif // !vrptwNew_CONFIGURATION_H
