
#ifndef CN_HUST_LYH_CFG_H
#define CN_HUST_LYH_CFG_H

#include "Flag.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct Configuration {

	unsigned seed = -1;
	std::string inputPath = "";
	std::string outputPath = "../Results/";
	
	int lkhRN = -1;
	DisType lkhRL = -1;
	int cmdIsopt = -1;

	int d15RecRN = 0;
	DisType d15RecRL = 0;

	int sintefRecRN = 0;
	DisType sintefRecRL = 0;
	//  TODO[lyh][0]:�ύ֮ǰҪȥ��
	int naRecRN = 0;
	DisType naRecRL = 0;

	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	//int outNeiSize = 100;

	int outNeiSize = 50;
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

	//bool breakRecord = 0;

	int yearTabuLen = 5;
	int yearTabuRand = 5;

	//int yearTabuLen = 10;
	//int yearTabuRand = 10;

	// TODO[0]:���費��ҪƵ������ 1000 stepC2_8_7 ��Ҫ20s C1_6_6 ��Ҫ6s
	int ejectLSMaxIter = 500;

	//int yearTabuLen = 20;
	//int yearTabuRand = 10;

	int popSize = 5;
	//int repairExitStep = 50;
	int repairExitStep = 20;
	int initFindPosPqSize = 20;
	int findBestPosForRuinPqSize = 10;
	int findBestPosInSolPqSize = 64;//64

	int naEaxCh = 20;
	//int naEaxCh = 20;

	//patternAdjustment����
	int patternAdjustmentNnei = 60;
	int patternAdjustmentGetM = 10;

	//naRepairGetMoves
	int naRepairGetMovesNei = 50;

	// mRLLocalSearchRange
	Vec<int> mRLLocalSearchRange = { 10,50 };
	//int mRLLSgetAllRange = 50;

	// ruinLocalSearch
	int ruinLocalSearchNextNeiBroad = 5;

	int ruinSplitRate = 0; // %100 means ruinSplitRate%
	
	int ruinLmax = 20;
	//(ruinLmax+1)/2
	int ruinC_ = 15;

	int ruinWinkacRate = 80; // 100
	//TODO[-1]:Ϊ��ʼ��������գ��
	int initWinkacRate = 99; // 100
	int abcyWinkacRate = 100; // 100

	void show();

	void repairByCusCnt(int cusCnt);

	void solveCommandLine(int argc, char* argv[]);

	void addHelpInfo();
};

}

#endif // !CN_HUST_LYH_CFG_H