// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Utility.h"

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {
//bool allocGlobalMem(std::string inpath) {

	// ../Instances/Solomon/RC201.txt 1800 1261.8 1
	// ../Instances/Homberger/R1_8_3.txt 3600 29429.1 0
	// ../Instances/Homberger/RC2_6_4.txt 3600 6989.4 0

	globalCfg = new hust::Configuration();

	//globalCfg->inputPath = inpath;
	//globalCfg->inputPath = "../Instances/Homberger/C2_8_7.txt";
	//globalCfg->inputPath = "../Instances/Homberger/RC2_6_4.txt";
	//globalCfg->inputPath = "../Instances/Homberger/R1_8_3.txt";
	//globalCfg->inputPath = "../Instances/Solomon/RC201.txt";
	//globalCfg->inputPath = "../Instances/Homberger/R2_2_4.txt";

	//globalCfg->inputPath = "Instances/Solomon/C101.txt";
	//globalCfg->inputPath = "../Instances/Homberger/RC1_8_3.txt";
	//globalCfg->inputPath = "../Instances/Homberger/RC1_8_1.txt";
	//globalCfg->inputPath = "../Instances/Homberger/C1_4_2.txt";
	//globalCfg->inputPath = "../Instances/Homberger/C2_10_6.txt";
	//globalCfg->inputPath = "../Instances/Homberger/RC1_8_5.txt";
	//globalCfg->inputPath = "../Instances/Homberger/C2_8_7.txt";

	globalCfg->solveCommandLine(argc, argv);

	if (globalCfg->seed == -1) {
		globalCfg->seed = std::time(nullptr) + std::clock();
	}
	

	globalCfg->seed = 1641485895;

	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	globalCfg->addHelpInfo();

	globalCfg->show();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<int>>
		(globalInput->custCnt + 1, Vec<int>(globalInput->custCnt + 1, 0));

	bks = new BKS();

	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalInput;
	delete bks;
	return true;
}

}//namespace hust

int main(int argc, char* argv[])
{
	//hust::INFO(sizeof(std::mt19937));
	//hust::INFO(sizeof(hust::Solver));
	//hust::INFO(sizeof(hust::Timer));
	//hust::INFO(sizeof(hust::Solver::alpha));
	//hust::INFO(sizeof(hust::Solver::input));
	//hust::INFO(sizeof(hust::Solver::globalCfg));
	//hust::INFO(sizeof(hust::Input));

	hust::allocGlobalMem(argc,argv);
	hust::Goal goal;

	//goal.callSimulatedannealing();
	goal.TwoAlgCombine();
	//goal.test();
	hust::deallocGlobalMem();
	//}
	return 0;
}