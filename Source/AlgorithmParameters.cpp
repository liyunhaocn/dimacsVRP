

#include <algorithm>
#include <type_traits>
#include "AlgorithmParameters.h"
#include "Util.h"
#include "Solver.h"

namespace hust {

void AlgorithmParameters::displayInfo() {

	//squContiIter = 100;
	//squMinContiIter = 100;
	//maxStagnationIterOfRepair = 199;
	//squIterStepUp = 10;
	//weightUpStep = 1;
	//perturbIrand = 400;
	//runTimer = 1800; //s
	//neiMoveKind = 16;
	//Pwei0 = 1;
	//Pwei1 = 2;
	//minKmax = 1;
	//maxKmax = 4;
	//yearTabuLen = 10;
	//yearTabuRand = 10;
	//ejectLSMaxIter = 200;
	//populationSize = 4;
	//populationSizeMin = 4;
	//populationSizeMax = 50;
	//repairExitStep = 5;
	//namaEaxCh = 20;
	//perturbNeiborRange = 60;
	//patternAdjustmentGetM = 10;
	//broaden = 1.2;
	//broadenWhenPos_0 = 20;
	//outNeiborSize = 50;
	//neiborSizeMin = 20;
	//neiborSizeMax = 50;
	//ruinLocalSearchNextNeiBroad = 5;
	//ruinSplitRate = 0; // %100 means ruinSplitRate%
	//ruinLmax = 20;
	//ruinC_ = 15;
	//ruinC_Min = 15;
	//ruinC_Max = 35;
	//ruinWinkRate = 90; // 100
	//initWinkRate = 80; // 100
	//abCycleWinkRate = 99; // 100
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

void AlgorithmParameters::check(int cusCnt) {

	lyhCheckTrue(outNeiborSize < cusCnt);
	lyhCheckTrue(broadenWhenPos_0<=cusCnt);
	lyhCheckTrue(perturbNeiborRange<cusCnt);
	lyhCheckTrue(neiborRange[0] < cusCnt);
	lyhCheckTrue(neiborRange[1] < cusCnt);
	lyhCheckTrue(ruinC_ < cusCnt);
}

}