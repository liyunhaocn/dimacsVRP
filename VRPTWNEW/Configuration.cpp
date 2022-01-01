

#include <algorithm>
#include <type_traits>
#include "Configuration.h"
#include "Utility.h"

namespace hust{

void Configuration::show() {

	println("inputPath:", inputPath);
	println("runTimer:",runTimer);
	println("Pwei0:",Pwei0);
	println("Pwei1:",Pwei1);
	println("minKmax:",minKmax);
	println("maxKmax:",maxKmax);
	println("seed:",seed);
	println("cmdDimacsRL:", cmdDimacsRL);
	println("cmdIsopt", cmdIsopt);
}

void Configuration::repairByCusCnt(int cusCnt) {

		outNeiSize = std::min<int>(cusCnt, outNeiSize);
		broadenWhenPos_0 = std::min<int>(cusCnt, broadenWhenPos_0);

		//TODO[lyh][2]: patternAdjustment²ÎÊý
		patternAdjustmentNnei = std::min<int>(cusCnt, patternAdjustmentNnei);
		patternAdjustmentGetM = std::min<int>(cusCnt, patternAdjustmentGetM);

		//TODO[lyh][2]:naRepairGetMoves
		naRepairGetMovesNei = std::min<int>(cusCnt, naRepairGetMovesNei);

		// TODO[lyh][2] mRLLocalSearchRange
		//mRLLocalSearchRange = { 10,50 };
		mRLLocalSearchRange[0] = std::min<int>(cusCnt, mRLLocalSearchRange[0]);
		mRLLocalSearchRange[1] = std::min<int>(cusCnt, mRLLocalSearchRange[1]);
		//mRLLSgetAllRange = std::min<int>(cusCnt, mRLLSgetAllRange);

		//ruinLocalSearch
		ruinLocalSearchNextNeiBroad = std::min<int>(cusCnt, ruinLocalSearchNextNeiBroad);

	}

void Configuration::solveCommandLine(int argc, char* argv[]) {

	if (argc >= 2) {
		std::string inpath = argv[1];
		globalCfg->inputPath = (inpath);
	}

	if (argc >= 3) {
		globalCfg->runTimer = std::stoi(argv[2], nullptr, 0);
	}

	if (argc >= 4) {
		if (std::is_same<DisType, LL>::value == true) {
			globalCfg->cmdDimacsRL = std::atof(argv[3])*disMul;
		}
		else {
			globalCfg->cmdDimacsRL = std::atof(argv[3]) * disMul;
		}
	}
	
	if (argc >= 5) {
		globalCfg->cmdIsopt = std::stoi(argv[4], nullptr, 0);
	}


	//if (argc >= 6) {
	//	globalCfg->seed = std::stoi(argv[5], nullptr, 0);
	//}
}

}