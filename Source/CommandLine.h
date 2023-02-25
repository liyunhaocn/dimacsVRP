
#ifndef CN_HUST_LYH_COMMAND_LINE_H
#define CN_HUST_LYH_COMMAND_LINE_H

#include <iostream>
#include <climits>
#include "AlgorithmParameters.h"
#include "Util.h"

namespace hust {

class CommandLine
{
public:

	AlgorithmParameters aps;

	String instancePath = "";
	String outputDir = "";
	String parametersPath = "";

	int seed = 0;
	int runTimer = 1800; //s
	int dimacsPrint = 0;
	int infoFlag = 1;
	int debugFlag = 0;
	int errorFlag = 1;

	void to_json(Json& j, const CommandLine& cl) {
		j = Json{
			{"seed", cl.seed},
			{"runTimer", cl.runTimer},
			{"dimacsPrint", cl.dimacsPrint},
			{"infoFlag", cl.infoFlag},
			{"debugFlag", cl.debugFlag},
			{"errorFlag", cl.errorFlag}
		};
		Json t;
		aps.to_json(t, cl.aps);
		j["aps"] = t;
	}

	void from_json(const Json& j, CommandLine& cl) {

		j.at("seed").get_to(cl.seed);
		j.at("runTimer").get_to(cl.runTimer);
		j.at("dimacsPrint").get_to(cl.dimacsPrint);
		j.at("infoFlag").get_to(cl.infoFlag);
		j.at("debugFlag").get_to(cl.debugFlag);
		j.at("errorFlag").get_to(cl.errorFlag);

		Logger::infoFlag = cl.infoFlag;
		Logger::debugFlag = cl.debugFlag;
		Logger::errorFlag = cl.errorFlag;

		auto& apsJson = j.at("AlgorithmParameters");
		aps.from_json(apsJson, this->aps);
	}

	CommandLine(int argc, char* argv[]) {

		if (argc < 4) {
			displayHelpInfo();
			exit(0);
		}
		if (argv[1] == "-h" || argv[1] == "--help") {
			displayHelpInfo();
			exit(0);
		}

		this->instancePath = String(argv[1]);
		this->parametersPath = String(argv[2]);
		this->outputDir = String(argv[3]);
	}

	void displayInfo() {

		Logger::INFO("instancePath:", instancePath);
		Logger::INFO("seed:", seed);
		Logger::INFO("runTimer:", runTimer);
	}

	void displayHelpInfo(){

		std::stringstream ss;
		ss << "----------------- DLLSMA Solver (2023) ----------------" << std::endl;
		ss << "call solver by:" << std::endl;
		ss << "./DLLSMA instance parameters output					  " << std::endl;
		ss << "instance  	string  required  vrptw instance file path" << std::endl;
		ss << "parameters	string  required  the parameters file path" << std::endl;
		ss << "output    	string	required  solution file saved path" << std::endl;
		std::cout << ss.str();
	};
};
}

#endif // CN_HUST_LYH_COMMAND_LINE_H