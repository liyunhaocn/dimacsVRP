
#ifndef CN_HUST_LYH_CFG_H
#define CN_HUST_LYH_CFG_H

#include "Flag.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct Configuration {

	LL seed = -1;
	std::string inputPath = "";
	std::string outputPath = "../Results/";
	
	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	int weightUpStep = 1;

	int squGrea0AC = 1;
	int squGrea0Range = 1;
	int squRandomNeiSize = 5;

	int Irand = 200;
	int runTimer = 1800; //s
	//int runTimer = 0; //s
	int neiMoveKind = 16;

	int Pwei0 = 1;
	int Pwei1 = 2;

	int minKmax = 2;
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

	int initFindPosPqSize = 20;
	int findBestPosForRuinPqSize = 10;
	int findBestPosInSolPqSize = 64;//64

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

	int ruinWinkacRate = 97; // 100
	//TODO[-1]:为初始化设置了眨眼
	int initWinkacRate = 97; // 100
	int abcyWinkacRate = 99; // 100

	void show();

	void repairByCusCnt(int cusCnt);

	void solveCommandLine(int argc, char* argv[]);

	void addHelpInfo();

};

}

#endif // !CN_HUST_LYH_CFG_H
