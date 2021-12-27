

#include <algorithm>
#include "Configuration.h"

namespace hust{

void Configuration::show() {

	debug(runTimer);
	debug(breakRecord);
	debug(Pwei0);
	debug(Pwei1);
	debug(minKmax);
	debug(maxKmax);
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

}