

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
	Logger::INFO("routeWeightUpStep:", routeWeightUpStep);
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
                {"ruinWinkRate", aps.ruinWinkRate},
                {"routeWeightUpStep", aps.routeWeightUpStep}
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
        j.at("initWinkRate").get_to(aps.initWinkRate);
        j.at("routeWeightUpStep").get_to(aps.routeWeightUpStep);

        aps.populationSizeMax = aps.initSetSize;
    }

}