
#ifndef CN_HUST_LYH_COMMAND_LINE_H
#define CN_HUST_LYH_COMMAND_LINE_H

#include <iostream>
#include <climits>
#include <fstream>
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

	int seed = -1;
	int runTimer = 1800; //s
	int dimacsPrint = 0;
	int infoFlag = 1;
	int debugFlag = 0;
	int errorFlag = 1;

	CommandLine(int argc, char* argv[]) {

		if (argc < 4) {
			displayHelpInfo();
			exit(0);
		}
		if (String(argv[1]) == "-h" || String(argv[1]) == "--help") {
			displayHelpInfo();
			exit(0);
		}

		this->instancePath = String(argv[1]);
		this->parametersPath = String(argv[2]);
		this->outputDir = String(argv[3]);

		if (this->outputDir.size() == 0) {
			throw std::invalid_argument("the output directory is required");
		}

		if (this->outputDir.back() == '\\' || this->outputDir.back() == '/') {
			;
		}
		else {
			this->outputDir += '/';
		}

//		String outputName = instancePath;
//		strtool::removeChar(outputName, '\\');
//		strtool::removeChar(outputName, '/');
//		strtool::removeChar(outputName, '.');
//		std::fstream fs1(this->outputDir + outputName + ".txt", std::ios::in | std::ios::out | std::ios::app);
//		if (!fs1)
//			throw std::invalid_argument("something wrong with output directory: " + this->outputDir);
//		this->outputDir += outputName + ".txt";
//		fs1.clear();
//		fs1.close();
	}

	void check() {

		// check the dir exist
		std::fstream fs1(this->instancePath);
		if (!fs1)
			throw std::invalid_argument("Impossible to instance file: " + this->instancePath);
		std::fstream fs2(this->parametersPath);
		if (!fs2)
			throw std::invalid_argument("Impossible to parameters file: " + this->parametersPath);
		
		fs1.clear();
		fs2.clear();
		fs1.close();
		fs2.close();
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
		
		ss << "Options:												  " << std::endl;
		ss << "-h,--help                 print this help message and exit" << std::endl;
		ss << "example use" << std::endl;
		ss << "../Instances/Homberger/C1_2_1.txt ../SolverParameters.json ../Results" << std::endl;

		std::cout << ss.str();
	};
};
    void to_json(Json& j, const CommandLine& cl);
    void from_json(const Json& j, CommandLine& cl);

}

#endif // CN_HUST_LYH_COMMAND_LINE_H