// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

//#include "Utility.h"
//#include "Solver.h"
//#include "Problem.h"
//#include "Configuration.h"
//#include "Flag.h"
//#include "EAX.h"
#include "Goal.h"

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {

	 //1800 827.3 1
	globalCfg = new hust::Configuration();
	//globalCfg->inputPath = "../Instances/Homberger/C2_8_7.txt";
	//globalCfg->inputPath = "../Instances/Homberger/RC2_6_4.txt";
	globalCfg->inputPath = "../Instances/Solomon/RC201.txt";
	//globalCfg->inputPath = "../Instances/Homberger/R2_2_4.txt";

	//globalEnv = new Environment("../Instances/Solomon/C101.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_3.txt");
	//globalEnv = new Environment("../Instances/Homberger/RC1_8_1.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C1_4_2.txt");
	//globalEnv  = new Environment("../Instances/Homberger/C2_10_6.txt");
	//globalEnv  = new Environment("../Instances/Homberger/RC1_8_5.txt");
	//globalEnv = new Environment("../Instances/Homberger/C2_8_7.txt");

	globalCfg->solveCommandLine(argc, argv);

	if (globalCfg->seed == -1) {
		globalCfg->seed = std::time(nullptr) + std::clock();
	}

	//globalCfg->seed = 1611589828;
	//globalCfg->seed = 1611589111;
	//globalCfg->seed = 1640620823;
	//globalCfg->seed = 1640660545;//RC2_6_4
	//globalCfg->seed = 1640858824;//RC2_6_4
	//globalCfg->seed = 1640880535;
	//globalCfg->seed = 1641054042;

	globalCfg->show();
	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<LL> >
		(globalInput->custCnt + 1, Vec<LL>(globalInput->custCnt + 1, 0));

	bks = new BKS();
	//TODO[lyh][0]:
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
	//hust::println(sizeof(std::mt19937));
	//hust::println(sizeof(hust::Solver));
	//hust::println(sizeof(hust::Timer));
	//hust::println(sizeof(hust::Solver::alpha));
	//hust::println(sizeof(hust::Solver::input));
	//hust::println(sizeof(hust::Solver::globalCfg));
	//hust::println(sizeof(hust::Input));

	hust::allocGlobalMem(argc, argv);
	hust::Goal goal;

	//goal.justLocalSearch(); 
	goal.TwoAlgCombine();
	//goal.test();
	hust::deallocGlobalMem();

	return 0;
}