
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

	enum class SolverGoal {
		StaticRoutesDistance = 0,
		StaticRoutesNumber,
		DynamicRoutesDistance,
		StaticRoutesNumberBound,
	};

	std::vector<std::string> allKindOfGoal = {
		"StaticRoutesDistance",
		"StaticRoutesNumber",
		"DynamicRoutesDistance",
		"StaticRoutesNumberBound",
	};

	AlgorithmParameters aps;

	int seed = 0;
	int runTimer = 1800; //s
	int dimacsPrint = 0;

	SolverGoal solverGoal = SolverGoal(0);
	int readInstanceFromStdin = 0;

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
			else if (argvstr == "-readInstanceFromStdin") {
				this->readInstanceFromStdin = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-bksDataFileBasePath") {
				this->bksDataFileBasePath = std::string(argv[i + 1]);
			}
			else if (argvstr == "-solverGoal") {
				std::string goalString = std::string(argv[i + 1]); 
				int index = -1;
				if ( (index = vectool::indexOfElement(allKindOfGoal,goalString)) >= 0) {
					this->solverGoal = SolverGoal(index);
				}
				else {
					Logger::ERROR("-solverGoal Parameter Error");
					displayHelpInfo();
					exit(0);
				}
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
			else if (argvstr == "-popSizeMin") {
				aps.popSizeMin = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-popSizeMax") {
				aps.popSizeMax = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-customersWeight1") {
				aps.customersWeight1 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-customersWeight2") {
				aps.customersWeight2 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-psizemulpsum") {
				aps.psizemulpsum = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-ejectLSMaxIter") {
				aps.ejectLSMaxIter = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-dimacsPrint") {
				this->dimacsPrint = std::stoi(argv[i + 1], nullptr, 0);
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
				Logger::ERROR("--------------unknow argv------------:", argvstr);
				displayHelpInfo();
				exit(0);
			}
		}

		if (dimacsPrint == true) {
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
		ss << "[-seed:int] the seed of random" << std::endl;

		// solverGoal 
		ss << "[-solverGoal:string] values:";
		for (auto& str : allKindOfGoal) {
			ss << str<< ", ";
		}
		ss<<"default value: "<< allKindOfGoal[0] << std::endl;

		ss << "[-readInstanceFromStdin:int] ranges {0,1},default 0, get instance from stdin"<< std::endl;
		ss << "[-rateOfDynamicInAndOut:int] ranges [0,100],default 5, the rate to limit dymaic search customer insertion and ejection" << std::endl;
		ss << "[-bksDataFileBasePath:string] the bks file path" << std::endl;
		ss << "[-popSizeMin:int] the minimal size of population" << std::endl;
		ss << "[-popSizeMax:int] the maximal size of population" << std::endl;
		ss << "[-customersWeight1:int] the weight of when eject customers from solution" << std::endl;
		ss << "[-customersWeight2:int] the weight of when insert customers to solution" << std::endl;
		ss << "[-tag:string] the tag of one run to distinguish each run" << std::endl;
		ss << "[-ejectLSMaxIter:int] the maximal iterations to limit ejectLocalSearch" << std::endl;
		ss << "[-dimacsPrint:int] ranges {0,1} set value to 1 means close the info log and print the bks format dimacs style" << std::endl;
		ss << "[-infoFlag:bool] switch to control the std output of info" << std::endl;
		ss << "[-debugFlag:bool] switch to control the std output of debug" << std::endl;
		ss << "[-errorFlag:bool] switch to control the stderr output of error info" << std::endl;

		std::cout << ss.str();
	};
};
}

#endif // CN_HUST_LYH_COMMAND_LINE_H