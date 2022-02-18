

#include <algorithm>
#include <type_traits>
#include "Configuration.h"
#include "Utility.h"
#include "Solver.h"

namespace hust {

void Configuration::show() {

	INFO("inputPath:", inputPath);
	INFO("runTimer:",runTimer);
	
	INFO("Pwei0:", Pwei0);
	INFO("Pwei1:", Pwei1);
	INFO("minKmax:", minKmax);
	INFO("maxKmax:", maxKmax);

	INFO("seed:", seed);
}

void Configuration::repairByCusCnt(int cusCnt) {

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


void Configuration::solveCommandLine(int argc, char* argv[]) {

	if (argc >= 2) {
		std::string inpath = argv[1];
		globalCfg->inputPath = (inpath);
	}

	if (argc >= 3) {
		globalCfg->runTimer = std::stoi(argv[2], nullptr, 0);
		//globalCfg->runTimer = 100;
	}
	
	if (argc >= 4) {
		globalCfg->seed = std::stol(argv[3], nullptr, 0);
	}

}

#if 0
static bool writeOneLineToFile(std::string path,std::string data) {
	
	std::ofstream rgbData;

	rgbData.open(path, std::ios::app | std::ios::out);

	if (!rgbData) {
		INFO("output file open errno");
		return false;
	}
	rgbData << data << std::endl;
	rgbData.close();
	return true;
}
#endif // 0

void Configuration::addHelpInfo() {

	std::string example = globalCfg->inputPath;
	int txtPos = example.find(".txt");
	example = example.substr(0, txtPos);
	int slich = example.find("Homberger/");
	if (slich != -1) {
		slich += 9;
	}
	else {
		slich = example.find("Solomon/");
		slich += 7;
	}

	example = example.substr(slich + 1);
	//DEBUG(example);

	//char lkh[100];
	//snprintf(lkh, 100, "{ \"%s\", {%d, %lld, %d}},", example, lkhRN, lkhRL, cmdIsopt);
	//writeOneLineToFile("./lkh.txt", lkh);
	//char d15[100];
	//snprintf(d15, 100, "{ \"%s\", {%d, %lld, %d}},", example, d15RecRN, d15RecRL, cmdIsopt);
	//writeOneLineToFile("./d15.txt", d15);
}


}