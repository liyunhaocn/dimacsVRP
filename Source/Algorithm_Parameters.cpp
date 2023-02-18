

#include <algorithm>
#include <type_traits>
#include "Algorithm_Parameters.h"
#include "Util_Common.h"
#include "Solver.h"

namespace hust {

void AlgorithmParameters::displayInfo() {

	//squContiIter = 100;
	//squMinContiIter = 100;
	//squMaxContiIter = 199;
	//squIterStepUp = 10;
	//weightUpStep = 1;
	//Irand = 400;
	//runTimer = 1800; //s
	//neiMoveKind = 16;
	//Pwei0 = 1;
	//Pwei1 = 2;
	//minKmax = 1;
	//maxKmax = 4;
	//yearTabuLen = 10;
	//yearTabuRand = 10;
	//ejectLSMaxIter = 200;
	//popSize = 4;
	//popSizeMin = 4;
	//popSizeMax = 50;
	//repairExitStep = 5;
	//naEaxCh = 20;
	//patternAdjustmentNnei = 60;
	//patternAdjustmentGetM = 10;
	//broaden = 1.2;
	//broadenWhenPos_0 = 20;
	//outNeiSize = 50;
	//neiSizeMin = 20;
	//neiSizeMax = 50;
	//ruinLocalSearchNextNeiBroad = 5;
	//ruinSplitRate = 0; // %100 means ruinSplitRate%
	//ruinLmax = 20;
	//ruinC_ = 15;
	//ruinC_Min = 15;
	//ruinC_Max = 35;
	//ruinWinkacRate = 90; // 100
	//initWinkacRate = 80; // 100
	//abcyWinkacRate = 99; // 100
	//sintefRecRN = -1;
	//sintefRecRL = -1;
	//naRecRN = -1;
	//naRecRL = -1;
	//d15RecRN = -1;
	//d15RecRL = -1;
	//lkhRN = -1;
	//lkhRL = -1;
	//cmdIsopt = -1;
	Logger::INFO("customersWeight1:", customersWeight1);
	Logger::INFO("customersWeight2:", customersWeight2);
	Logger::INFO("minKmax:", minKmax);
	Logger::INFO("maxKmax:", maxKmax);
}

void AlgorithmParameters::repairByCusCnt(int cusCnt) {

	outNeiSize = std::min<int>(cusCnt-1, outNeiSize);
	broadenWhenPos_0 = std::min<int>(cusCnt, broadenWhenPos_0);

	patternAdjustmentNnei = std::min<int>(cusCnt-1, patternAdjustmentNnei);
	patternAdjustmentGetM = std::min<int>(cusCnt-1, patternAdjustmentGetM);

	//mRLLocalSearchRange = { 10,50 };
	mRLLocalSearchRange[0] = std::min<int>(cusCnt-1, mRLLocalSearchRange[0]);
	mRLLocalSearchRange[1] = std::min<int>(cusCnt-1, mRLLocalSearchRange[1]);
	//mRLLSgetAllRange = std::min<int>(cusCnt, mRLLSgetAllRange);

	ruinLocalSearchNextNeiBroad = std::min<int>(cusCnt-1, ruinLocalSearchNextNeiBroad);

	ruinC_ = std::min<int>(cusCnt - 1, ruinC_);
}



#if 0
static bool writeOneLineToFile(std::string path,std::string data) {
	
	std::ofstream rgbData;

	rgbData.open(path, std::ios::app | std::ios::out);

	if (!rgbData) {
		Logger::INFO("output file open errno");
		return false;
	}
	rgbData << data << std::endl;
	rgbData.close();
	return true;
}
#endif // 0


}