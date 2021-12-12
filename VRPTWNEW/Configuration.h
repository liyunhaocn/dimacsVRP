#ifndef vrptwNew_CONFIGURATION_H
#define vrptwNew_CONFIGURATION_H

#include "./Flag.h"

using namespace std;

// $(SolutionDir)$(Platform)\$(Configuration)
// $(SolutionDir)\dimacsVRP

namespace hust {

struct Configuration {

	int squContiIter = 100;
	int squMinContiIter = 100;
	int squMaxContiIter = 199;
	int squIterStepUp = 10;

	/*vector<int> outNeiSize = { 199 };
	vector<double> broaden = { 1.5 };*/

	vector<int> outNeiSize = {100};
	vector<double> broaden = {1.2};
	
	int broadIndex = 0;

	int weightUpStep = 1;

	int squGrea0AC = 1;
	int squGrea0Range = 1;
	int squRandomNeiSize = 5;

	//const LL squRandAC = 1;//随机挑选的概率
	//const LL squRandRange = 2000 ;//随机挑选的概率

	int Irand = 200;
	int runTimer = 1800; //s
	int neiMoveKind = 16;
	int popSize = 10;

	int Pwei0 = 1;
	int Pwei1 = 2;

	int minKmax = 2;
	int maxKmax = 4;

	bool breakRecord = 0;

	//LL yearTabuLen = 5;
	//LL yearTabuRand = 5;
	
	int yearTabuLen = 20;
	int yearTabuRand = 10;

	void show() {

		debug(breakRecord);
		debug(Pwei0);
		debug(Pwei1);
		debug(minKmax);
		debug(maxKmax);
	}
};

}

#endif // !vrptwNew_CONFIGURATION_H
