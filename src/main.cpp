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

	// ../Instances/Homberger/R2_8_9.txt
	// ../Instances/Homberger/RC1_10_1.txt
	// ../Instances/Homberger/RC1_10_5.txt
	// ../Instances/Solomon/R103.txt
	// ../Instances/Solomon/R210.txt
	// ../Instances/Homberger/RC2_6_6.txt
	// ../Instances/Homberger/R1_6_8.txt
	// ../Instances/Homberger/R1_2_2.txt
	// ../Instances/Homberger/RC2_6_4.txt
	// ../Instances/Homberger/RC2_10_5.txt
	//../Instances/Solomon/R204.txt
	//../Instances/Solomon/R210.txt
	//../Instances/Homberger/R2_2_4.txt 
	//../Instances/Homberger/RC1_8_1.txt 
	//../Instances/Homberger/C1_10_6.txt 
	//../Instances/Homberger/C2_10_6.txt 
	//../Instances/Homberger/RC1_8_5.txt
	//../Instances/Homberger/RC2_10_1.txt

	globalCfg = new hust::Configuration();

	globalCfg->solveCommandLine(argc, argv);

	if (globalCfg->seed == -1) {
		globalCfg->seed = std::time(0) + std::clock();
	}
	
	//globalCfg->seed = 1645192521;
	//globalCfg->seed = 1645199481;

	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	ERROR("globalCfg->seed:", globalCfg->seed, " ins:", globalInput->example);
	INFO("argc:", argc);

	globalCfg->addHelpInfo();

	globalCfg->show();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<int>>
		(globalInput->custCnt + 1, Vec<int>(globalInput->custCnt + 1, 0));

	bks = new BKS();
	gloalTimer = new Timer(globalCfg->runTimer);

#if DIMACSGO
#else
	globalInput->printHelpInfo();
#endif // DIMACSGO

	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalInput;
	delete bks;
	delete gloalTimer;

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
	//goal.TwoAlgCombine();
	
	//goal.test();
	goal.experOnMinRN();

	hust::deallocGlobalMem();

	return 0;
}