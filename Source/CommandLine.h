
#ifndef CN_HUST_LYH_COMMAND_LINE_H
#define CN_HUST_LYH_COMMAND_LINE_H

#include <iostream>
#include <string>
#include <climits>
#include "Algorithm_Parameters.h"
#include "Util_Logger.h"

namespace hust {

class CommandLine
{
public:

	AlgorithmParameters aps;

	int seed = 0;
	int runTimer = 1800; //s
	int isDynamicRun = 0;
	int isDimacsRun = 0;
	int isMinimizeRouteNumberRun = 0;
	int isRouteNumberBoundRun = 0;

	std::string instancePath = "";
	std::string outputPath = "../Results/";
	std::string bksDataFileBasePath = "../Instances/DataFromLiterature/";

	CommandLine(int argc, char* argv[]) {

		if (argc % 2 == 0 || argc < 3) {
			std::cerr << "argc num is wrong" << std::endl;
			displayHelpInfo();
			exit(-1);
		}

		for (int i = 1; i < argc; i += 2) {

			std::string argvstr = std::string(argv[i]);
			if (argvstr == "-time") {
				this->runTimer = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-instancePath") {
				this->instancePath = std::string(argv[i + 1]);
			}
			else if (argvstr == "-bksDataFileBasePath") {
				this->bksDataFileBasePath = std::string(argv[i + 1]);
			}
			else if (argvstr == "-isDynamicRun") {
				this->isDynamicRun = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-isDimacsRun") {
				this->isDimacsRun = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-isMinimizeRouteNumberRun") {
				this->isMinimizeRouteNumberRun = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-isRouteNumberBoundRun") {
				this->isRouteNumberBoundRun = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-rateOfDynamicInAndOut") {
				aps.rateOfDynamicInAndOut = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-seed") {
				this->seed = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-tag") {
				aps.tag = std::string(argv[i + 1]);
			}
			else if (argvstr == "-customersWeight0") {
				aps.customersWeight0 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-customersWeight1") {
				aps.customersWeight1 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-psizemulpsum") {
				aps.psizemulpsum = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-ejectLSMaxIter") {
				aps.ejectLSMaxIter = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-dimacsGo") {
				dimacsGo = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-infoFlag") {
				Logger::infoFlag = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-debugFlag") {
				Logger::debugFlag = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-errorFlag") {
				Logger::errorFlag = std::stoi(argv[i + 1], nullptr, 0);
			}
			else {
				std::cerr << "--------------unknow argv------------:" << argvstr << std::endl;
				displayHelpInfo();
			}
		}

		if (dimacsGo == true) {
			Logger::infoFlag = false;
		}

	}
	void displayInfo() {

		Logger::INFO("instancePath:", instancePath);
		Logger::INFO("outputPath:", outputPath);
		Logger::INFO("bksDataFileBasePath:", bksDataFileBasePath);
		Logger::INFO("seed:", seed);
		Logger::INFO("runTimer:", runTimer);
	}

	void displayHelpInfo()
	{

		std::stringstream ss;
		ss << "------------------- solver DLLSMA ------------------------------" << std::endl;
		ss << "call solver by:" << std::endl;
		ss << "./DLLSMA -instancePath:str" << std::endl;
		ss << "[-time:int] the time limit" << std::endl;
		ss << "[-isDynamicRun:int] ranges 0 or 1, default value 0, if the dynamic run" << std::endl;
		ss << "[-isDimacsRun:int] ranges 0 or 1, default value 0, if the dimacsRun run" << std::endl;
		ss << "[-isMinimizeRouteNumberRun:int] ranges 0 or 1, default value 0, if the minimize route number run" << std::endl;
		ss << "[-isRouteNumberBoundRun:int] ranges 0 or 1, default value 0, if the getting route number bound run" << std::endl;
		ss << "[-rateOfDynamicInAndOut:int] ranges [0,100],default 5, the rate to limit dymaic search customer insertion and ejection" << std::endl;
		ss << "[-bksDataFileBasePath:string] the bks file path" << std::endl;
		ss << "[-seed:int] the seed of random" << std::endl;
		ss << "[-customersWeight0:int] the weight of when eject customers from solution" << std::endl;
		ss << "[-customersWeight0:int] the weight of when insert customers to solution" << std::endl;
		ss << "[-tag:string] the tag of one run to distinguish each run" << std::endl;
		//ss << "[-isbreak:int]" << std::endl;
		ss << "[-psizemulpsum:int]" << std::endl;
		ss << "[-ejectLSMaxIter:int] the max iter to limit ejectLocalSearch" << std::endl;
		ss << "[-dimacsGo:bool] to close the info log and print the bks format dimacs style" << std::endl;
		ss << "[-infoFlag:bool] switch to control the std output of info" << std::endl;
		ss << "[-debugFlag:bool] switch to control the std output of debug" << std::endl;
		ss << "[-errorFlag:bool] switch to control the stderr output of error info" << std::endl;

		std::cout << ss.str();
	};
};
}

#endif // CN_HUST_LYH_COMMAND_LINE_H