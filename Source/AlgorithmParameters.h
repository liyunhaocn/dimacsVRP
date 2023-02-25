
#ifndef CN_HUST_LYH_ALGORITHM_PARAMETERS_H
#define CN_HUST_LYH_ALGORITHM_PARAMETERS_H

#include "Common.h"
#include "Util.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct AlgorithmParameters {
	
	//static Vector<int> nieborMoves;

	int populationSizeMin = 4;
	int populationSizeMax = 50;

	int maxStagnationIterOfRepair = 200;

	int minKmax = 1;
	int maxKmax = 4;

	int namaEaxCh = 20;
	int abCycleWinkRate = 99; // 100

	int patternAdjustmentNeiborRange = 60;
	
	int patternAdjustmentIrand = 400;

	int neiborSizeMin = 20;
	int neiborSizeMax = 50;

	int ruinSplitRate = 0; // %100 means ruinSplitRate%
	int ruinLmax = 20;
	//(ruinLmax+1)/2
	
	int ruinC_Min = 15;
	int ruinC_Max = 35;
	int ruinWinkRate = 90; // 100
	int initWinkRate = 80; // 100

	void to_json(Json& j, const AlgorithmParameters& aps) {
		j = Json{
			{"populationSizeMin", aps.populationSizeMin },
			{"populationSizeMax", aps.populationSizeMax },
			{"populationSizeMin", aps.populationSizeMin},
			{"populationSizeMax", aps.populationSizeMax},
			{"maxStagnationIterOfRepair", aps.maxStagnationIterOfRepair},
			{"minKmax", aps.minKmax},
			{"maxKmax", aps.maxKmax},
			{"namaEaxCh", aps.namaEaxCh},
			{"abCycleWinkRate", aps.abCycleWinkRate},
			{"patternAdjustmentNeiborRange", aps.patternAdjustmentNeiborRange},
			{"patternAdjustmentIrand", aps.patternAdjustmentIrand},
			{"neiborSizeMin", aps.neiborSizeMin},
			{"neiborSizeMax", aps.neiborSizeMax},
			{"ruinSplitRate", aps.ruinSplitRate},
			{"ruinLmax", aps.ruinLmax},
			{"ruinC_Min", aps.ruinC_Min},
			{"ruinC_Max", aps.ruinC_Max},
			{"ruinLmax", aps.ruinLmax},
			{"ruinWinkRate", aps.ruinWinkRate}
		};
	}

	void from_json(const Json& j, AlgorithmParameters& aps){
		j.at("populationSizeMin").get_to(aps.populationSizeMin);
		j.at("populationSizeMax").get_to(aps.populationSizeMax);
		j.at("maxStagnationIterOfRepair").get_to(aps.maxStagnationIterOfRepair);
		j.at("minKmax").get_to(aps.minKmax);
		j.at("maxKmax").get_to(aps.maxKmax);
		j.at("namaEaxCh").get_to(aps.namaEaxCh);
		j.at("abCycleWinkRate").get_to(aps.abCycleWinkRate);
		j.at("patternAdjustmentNeiborRange").get_to(aps.patternAdjustmentNeiborRange);
		j.at("patternAdjustmentIrand").get_to(aps.patternAdjustmentIrand);
		j.at("neiborSizeMin").get_to(aps.neiborSizeMin);
		j.at("neiborSizeMax").get_to(aps.neiborSizeMax);
		j.at("ruinSplitRate").get_to(aps.ruinSplitRate);
		j.at("ruinLmax").get_to(aps.ruinLmax);
		j.at("ruinC_Min").get_to(aps.ruinC_Min);
		j.at("ruinC_Max").get_to(aps.ruinC_Max);
		j.at("ruinLmax").get_to(aps.ruinLmax);
		j.at("ruinWinkRate").get_to(aps.ruinWinkRate);
	}

	void displayInfo();

	void check(int cusCnt);
	
	int psizemulpsum = 1;
	double broaden = 1.2;
	int broadenWhenPos_0 = 20;
	Vector<int> neiborRange = { 10,20 };
	int neiMoveKind = 16;
	int outNeiborSize = 50;
	int customersWeight1 = 1;
	int customersWeight2 = 2;
	int ruinC_ = 15;
	int weightUpStep = 1;
	int yearTabuLen = 10;
	int yearTabuRand = 10;
	// TODO[0]:看需不需要频繁构造 1000 stepC2_8_7 需要20s C1_6_6 需要6s
	int ejectLSMaxIter = 200;

	~AlgorithmParameters() {}
};

}

#endif // !CN_HUST_LYH_ALGORITHM_PARAMETERS_H
