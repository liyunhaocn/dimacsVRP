
#ifndef CN_HUST_LYH_ALGORITHM_PARAMETERS_H
#define CN_HUST_LYH_ALGORITHM_PARAMETERS_H

#include "Common.h"
#include "Util.h"

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct AlgorithmParameters {
	
	//static Vector<int> nieborMoves;
	int initSetSize = 50;
	int populationSizeMin = 4;
	int populationSizeMax = 50;

	int maxStagnationIterOfRepair = 200;

	int minKmax = 1;
	int maxKmax = 4;

	int eamaEaxCh = 20;
	int abCycleWinkRate = 99; // 100

	int perturbNeiborRange = 60;
	
	int perturbIrand = 400;

	int neiborSizeMin = 20;
	int neiborSizeMax = 50;

	int ruinSplitRate = 0; // %100 means ruinSplitRate%
	int ruinLmax = 20;
	//(ruinLmax+1)/2
	
	int ruinC_Min = 15;
	int ruinC_Max = 35;
	int ruinWinkRate = 90; // 100
	int initWinkRate = 80; // 100

	int customersWeight1 = 1;
	int customersWeight2 = 2;

	void to_json(Json& j, const AlgorithmParameters& aps) {
		j = Json{
			{"initSetSize", aps.initSetSize },
			{"customersWeight1", aps.customersWeight1 },
			{"customersWeight2", aps.customersWeight2 },
			{"populationSizeMin", aps.populationSizeMin },
			{"populationSizeMax", aps.populationSizeMax },
			{"populationSizeMin", aps.populationSizeMin},
			{"populationSizeMax", aps.populationSizeMax},
			{"maxStagnationIterOfRepair", aps.maxStagnationIterOfRepair},
			{"minKmax", aps.minKmax},
			{"maxKmax", aps.maxKmax},
			{"eamaEaxCh", aps.eamaEaxCh},
			{"abCycleWinkRate", aps.abCycleWinkRate},
			{"perturbNeiborRange", aps.perturbNeiborRange},
			{"perturbIrand", aps.perturbIrand},
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
		j.at("initSetSize").get_to(aps.initSetSize);
		j.at("customersWeight1").get_to(aps.customersWeight1);
		j.at("customersWeight2").get_to(aps.customersWeight2);
		j.at("populationSizeMin").get_to(aps.populationSizeMin);
		j.at("populationSizeMax").get_to(aps.populationSizeMax);
		j.at("maxStagnationIterOfRepair").get_to(aps.maxStagnationIterOfRepair);
		j.at("minKmax").get_to(aps.minKmax);
		j.at("maxKmax").get_to(aps.maxKmax);
		j.at("eamaEaxCh").get_to(aps.eamaEaxCh);
		j.at("abCycleWinkRate").get_to(aps.abCycleWinkRate);
		j.at("perturbNeiborRange").get_to(aps.perturbNeiborRange);
		j.at("perturbIrand").get_to(aps.perturbIrand);
		j.at("neiborSizeMin").get_to(aps.neiborSizeMin);
		j.at("neiborSizeMax").get_to(aps.neiborSizeMax);
		j.at("ruinSplitRate").get_to(aps.ruinSplitRate);
		j.at("ruinLmax").get_to(aps.ruinLmax);
		j.at("ruinC_Min").get_to(aps.ruinC_Min);
		j.at("ruinC_Max").get_to(aps.ruinC_Max);
		j.at("ruinLmax").get_to(aps.ruinLmax);
		j.at("ruinWinkRate").get_to(aps.ruinWinkRate);

		aps.populationSizeMax = aps.initSetSize;
	}

	void displayInfo();

	void check(int cusCnt);
	
	int psizemulpsum = 1;
	double broaden = 1.2;
	int broadenWhenPos_0 = 20;
	Vector<int> neiborRange = { 10,20 };
	int neiMoveKind = 16;
	int outNeiborSize = 50;

	int ruinC_ = 15;
	int weightUpStep = 1;
	int yearTabuLen = 10;
	int yearTabuRand = 10;
	// 1000 step:C2_8_7 took 20s C1_6_6 took 6s
	int ejectLSMaxIter = 50;

	~AlgorithmParameters() {}
};

}

#endif // !CN_HUST_LYH_ALGORITHM_PARAMETERS_H
