
#ifndef CN_HUST_LYH_COMMAND_LINE_H
#define CN_HUST_LYH_COMMAND_LINE_H

#include <iostream>
#include <string>
#include <climits>
#include "Algorithm_Parameters.h"

namespace hust {

class CommandLine
{
public:

	AlgorithmParameters aps;

	LL seed = 0;
	int runTimer = 1800; //s
	std::string inputPath = "";
	std::string outputPath = "../Results/";

	CommandLine(int argc, char* argv[]) {

		if (argc % 2 == 0 || argc < 3) {
			std::cerr << "argc num is wrong" << std::endl;
			exit(-1);
		}

		for (int i = 1; i < argc; i += 2) {
			std::string argvstr = std::string(argv[i]);
			if (argvstr == "-time") {
				this->runTimer = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-ins") {
				this->inputPath = std::string(argv[i + 1]);
			}
			else if (argvstr == "-seed") {
				this->seed = std::stol(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-tag") {
				aps.tag = std::string(argv[i + 1]);
			}
			else if (argvstr == "-pwei0") {
				aps.Pwei0 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-pwei1") {
				aps.Pwei1 = std::stoi(argv[i + 1], nullptr, 0);
			}
			else if (argvstr == "-psizemulpsum") {
				aps.psizemulpsum = std::stoi(argv[i + 1], nullptr, 0);
			}
			else {
				std::cerr << "--------------unknow argv------------:" << argvstr << std::endl;
				displayHelpInfo();
			}
		}

	}
	void displayInfo() {

		INFO("seed:", seed);
		INFO("runTimer:", runTimer);
		INFO("inputPath:", inputPath);
		INFO("outputPath:", outputPath);
	}

	void displayHelpInfo()
	{

		std::stringstream ss;
		ss << "------------------- solver DLLSMA ------------------------------" << std::endl;
		ss << "call solver by:" << std::endl;
		ss << "./DLLSMA -ins instancePath " << std::endl;
		ss << "[-time inputPath:string]" << std::endl;
		ss << "[-seed seed:unsigned]" << std::endl;
		ss << "[-pwei0 Pwei0:int]" << std::endl;
		ss << "[-pwei1 Pwei1:int]" << std::endl;
		ss << "[-tag tag:string]" << std::endl;
		ss << "[-isbreak isBreak:int]" << std::endl;
		ss << "[-psizemulpsum psizemulpsum:int]" << std::endl;
		std::cout << ss.str();
	};
};
}

#endif // CN_HUST_LYH_COMMAND_LINE_H