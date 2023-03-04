

#include <algorithm>
#include <type_traits>
#include "AlgorithmParameters.h"
#include "Util.h"
#include "Solver.h"

namespace hust {

void AlgorithmParameters::displayInfo() {

	Logger::INFO("customersWeight1:", customersWeight1);
	Logger::INFO("customersWeight2:", customersWeight2);
	Logger::INFO("minKmax:", minKmax);
	Logger::INFO("maxKmax:", maxKmax);
}

void AlgorithmParameters::check(int cusCnt) {

	if (outNeiborSize >= cusCnt) throw std::string("outNeiborSize >= cusCnt");
	if(broadenWhenPos_0 > cusCnt)throw std::string("broadenWhenPos_0 > cusCnt");
	if(perturbNeiborRange >= cusCnt)throw std::string("perturbNeiborRange >= cusCnt");
	if(neiborRange[0] >= cusCnt)throw std::string("neiborRange[0] >= cusCnt");
	if(neiborRange[1] >= cusCnt)throw std::string("neiborRange[1] >= cusCnt");
	if(neiborRange[0] > neiborRange[1])throw std::string("neiborRange[0] > neiborRange[1]");
	if(ruinC_ >= cusCnt)throw std::string("ruinC_ >= cusCnt");
}

}