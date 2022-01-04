// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"

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

	//globalCfg->seed = 1641240893;

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
	//hust::INFO(sizeof(std::mt19937));
	//hust::INFO(sizeof(hust::Solver));
	//hust::INFO(sizeof(hust::Timer));
	//hust::INFO(sizeof(hust::Solver::alpha));
	//hust::INFO(sizeof(hust::Solver::input));
	//hust::INFO(sizeof(hust::Solver::globalCfg));
	//hust::INFO(sizeof(hust::Input));

	#if 0
	hust::Vec<std::string> cmds = {
		"DLLSMA.exe ../Instances/Solomon/C101.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C102.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C103.txt 1800 826.3 1",
		"DLLSMA.exe ../Instances/Solomon/C104.txt 1800 822.9 1",
		"DLLSMA.exe ../Instances/Solomon/C105.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C106.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C107.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C108.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C109.txt 1800 827.3 1",
		"DLLSMA.exe ../Instances/Solomon/C201.txt 1800 589.1 1",
		"DLLSMA.exe ../Instances/Solomon/C202.txt 1800 589.1 1",
		"DLLSMA.exe ../Instances/Solomon/C203.txt 1800 588.7 1",
		"DLLSMA.exe ../Instances/Solomon/C204.txt 1800 588.1 1",
		"DLLSMA.exe ../Instances/Solomon/C205.txt 1800 586.4 1",
		"DLLSMA.exe ../Instances/Solomon/C206.txt 1800 586.0 1",
		"DLLSMA.exe ../Instances/Solomon/C207.txt 1800 585.8 1",
		"DLLSMA.exe ../Instances/Solomon/C208.txt 1800 585.8 1",
		"DLLSMA.exe ../Instances/Solomon/R101.txt 1800 1637.7 1",
		"DLLSMA.exe ../Instances/Solomon/R102.txt 1800 1466.6 1",
		"DLLSMA.exe ../Instances/Solomon/R103.txt 1800 1208.7 1",
		"DLLSMA.exe ../Instances/Solomon/R104.txt 1800 971.5 1",
		"DLLSMA.exe ../Instances/Solomon/R105.txt 1800 1355.3 1",
		"DLLSMA.exe ../Instances/Solomon/R106.txt 1800 1234.6 1",
		"DLLSMA.exe ../Instances/Solomon/R107.txt 1800 1064.6 1",
		"DLLSMA.exe ../Instances/Solomon/R108.txt 1800 932.1 1",
		"DLLSMA.exe ../Instances/Solomon/R109.txt 1800 1146.9 1",
		"DLLSMA.exe ../Instances/Solomon/R110.txt 1800 1068.0 1",
		"DLLSMA.exe ../Instances/Solomon/R111.txt 1800 1048.7 1",
		"DLLSMA.exe ../Instances/Solomon/R112.txt 1800 948.9 1",
		"DLLSMA.exe ../Instances/Solomon/R201.txt 1800 1143.2 1",
		"DLLSMA.exe ../Instances/Solomon/R202.txt 1800 1029.6 1",
		"DLLSMA.exe ../Instances/Solomon/R203.txt 1800 870.8 1",
		"DLLSMA.exe ../Instances/Solomon/R204.txt 1800 731.3 1",
		"DLLSMA.exe ../Instances/Solomon/R205.txt 1800 949.8 1",
		"DLLSMA.exe ../Instances/Solomon/R206.txt 1800 875.9 1",
		"DLLSMA.exe ../Instances/Solomon/R207.txt 1800 794.0 1",
		"DLLSMA.exe ../Instances/Solomon/R208.txt 1800 701.0 1",
		"DLLSMA.exe ../Instances/Solomon/R209.txt 1800 854.8 1",
		"DLLSMA.exe ../Instances/Solomon/R210.txt 1800 900.5 1",
		"DLLSMA.exe ../Instances/Solomon/R211.txt 1800 746.7 1",
		"DLLSMA.exe ../Instances/Solomon/RC101.txt 1800 1619.8 1",
		"DLLSMA.exe ../Instances/Solomon/RC102.txt 1800 1457.4 1",
		"DLLSMA.exe ../Instances/Solomon/RC103.txt 1800 1258.0 1",
		"DLLSMA.exe ../Instances/Solomon/RC104.txt 1800 1132.3 1",
		"DLLSMA.exe ../Instances/Solomon/RC105.txt 1800 1513.7 1",
		"DLLSMA.exe ../Instances/Solomon/RC106.txt 1800 1372.7 1",
		"DLLSMA.exe ../Instances/Solomon/RC107.txt 1800 1207.8 1",
		"DLLSMA.exe ../Instances/Solomon/RC108.txt 1800 1114.2 1",
		"DLLSMA.exe ../Instances/Solomon/RC201.txt 1800 1261.8 1",
		"DLLSMA.exe ../Instances/Solomon/RC202.txt 1800 1092.3 1",
		"DLLSMA.exe ../Instances/Solomon/RC203.txt 1800 923.7 1",
		"DLLSMA.exe ../Instances/Solomon/RC204.txt 1800 783.5 1",
		"DLLSMA.exe ../Instances/Solomon/RC205.txt 1800 1154.0 1",
		"DLLSMA.exe ../Instances/Solomon/RC206.txt 1800 1051.1 1",
		"DLLSMA.exe ../Instances/Solomon/RC207.txt 1800 962.9 1",
		"DLLSMA.exe ../Instances/Solomon/RC208.txt 1800 776.1 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_10.txt 1800 2624.7 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_1.txt 1800 2698.6 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_2.txt 1800 2694.3 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_3.txt 1800 2675.8 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_4.txt 1800 2625.6 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_5.txt 1800 2694.9 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_6.txt 1800 2694.9 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_7.txt 1800 2694.9 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_8.txt 1800 2684.0 1",
		"DLLSMA.exe ../Instances/Homberger/C1_2_9.txt 1800 2639.6 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_10.txt 1800 1791.2 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_1.txt 1800 1922.1 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_2.txt 1800 1851.4 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_3.txt 1800 1763.4 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_4.txt 1800 1695.0 0",
		"DLLSMA.exe ../Instances/Homberger/C2_2_5.txt 1800 1869.6 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_6.txt 1800 1844.8 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_7.txt 1800 1842.2 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_8.txt 1800 1813.7 1",
		"DLLSMA.exe ../Instances/Homberger/C2_2_9.txt 1800 1815.0 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_10.txt 1800 3293.1 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_1.txt 1800 4667.2 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_2.txt 1800 3919.9 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_3.txt 1800 3373.9 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_4.txt 1800 3047.6 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_5.txt 1800 4053.2 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_6.txt 1800 3559.1 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_7.txt 1800 3141.9 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_8.txt 1800 2938.4 1",
		"DLLSMA.exe ../Instances/Homberger/R1_2_9.txt 1800 3734.7 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_10.txt 1800 2549.4 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_1.txt 1800 3468.0 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_2.txt 1800 3008.2 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_3.txt 1800 2537.5 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_4.txt 1800 1928.5 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_5.txt 1800 3061.1 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_6.txt 1800 2675.4 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_7.txt 1800 2304.7 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_8.txt 1800 1842.4 1",
		"DLLSMA.exe ../Instances/Homberger/R2_2_9.txt 1800 2843.3 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_10.txt 1800 2990.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_1.txt 1800 3516.9 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_2.txt 1800 3221.6 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_3.txt 1800 3001.4 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_4.txt 1800 2845.2 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_5.txt 1800 3325.6 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_6.txt 1800 3300.7 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_7.txt 1800 3177.8 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_8.txt 1800 3060.0 1",
		"DLLSMA.exe ../Instances/Homberger/RC1_2_9.txt 1800 3073.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_10.txt 1800 1989.2 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_1.txt 1800 2797.4 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_2.txt 1800 2481.6 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_3.txt 1800 2227.7 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_4.txt 1800 1854.8 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_5.txt 1800 2491.4 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_6.txt 1800 2495.1 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_7.txt 1800 2287.7 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_8.txt 1800 2151.2 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_2_9.txt 1800 2086.6 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_10.txt 3600 6825.4 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_1.txt 3600 7138.8 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_2.txt 3600 7113.3 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_3.txt 3600 6929.9 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_4.txt 3600 6777.7 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_5.txt 3600 7138.8 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_6.txt 3600 7140.1 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_7.txt 3600 7136.2 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_8.txt 3600 7083.0 1",
		"DLLSMA.exe ../Instances/Homberger/C1_4_9.txt 3600 6927.8 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_10.txt 3600 3665.1 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_1.txt 3600 4100.3 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_2.txt 3600 3914.1 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_3.txt 3600 3755.2 0",
		"DLLSMA.exe ../Instances/Homberger/C2_4_4.txt 3600 3523.7 0",
		"DLLSMA.exe ../Instances/Homberger/C2_4_5.txt 3600 3923.2 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_6.txt 3600 3860.1 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_7.txt 3600 3870.9 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_8.txt 3600 3773.7 1",
		"DLLSMA.exe ../Instances/Homberger/C2_4_9.txt 3600 3842.1 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_10.txt 3600 8086.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_1.txt 3600 10305.8 1",
		"DLLSMA.exe ../Instances/Homberger/R1_4_2.txt 3600 8876.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_3.txt 3600 7802.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_4.txt 3600 7269.0 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_5.txt 3600 9188.6 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_6.txt 3600 8340.4 1",
		"DLLSMA.exe ../Instances/Homberger/R1_4_7.txt 3600 7602.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_8.txt 3600 7255.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_4_9.txt 3600 8680.6 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_10.txt 3600 5645.9 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_1.txt 3600 7520.7 1",
		"DLLSMA.exe ../Instances/Homberger/R2_4_2.txt 3600 6482.8 1",
		"DLLSMA.exe ../Instances/Homberger/R2_4_3.txt 3600 5372.9 1",
		"DLLSMA.exe ../Instances/Homberger/R2_4_4.txt 3600 4211.2 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_5.txt 3600 6567.9 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_6.txt 3600 5818.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_7.txt 3600 4893.5 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_8.txt 3600 4001.0 0",
		"DLLSMA.exe ../Instances/Homberger/R2_4_9.txt 3600 6067.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_10.txt 3600 7585.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_1.txt 3600 8524.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_2.txt 3600 7884.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_3.txt 3600 7516.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_4.txt 3600 7295.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_5.txt 3600 8172.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_6.txt 3600 8148.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_7.txt 3600 7935.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_8.txt 3600 7765.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_4_9.txt 3600 7722.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_10.txt 3600 4252.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_1.txt 3600 6147.3 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_2.txt 3600 5407.5 1",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_3.txt 3600 4573.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_4.txt 3600 3597.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_5.txt 3600 5396.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_6.txt 3600 5332.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_7.txt 3600 4987.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_8.txt 3600 4706.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_4_9.txt 3600 4510.4 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_10.txt 3600 13636.5 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_1.txt 3600 14076.6 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_2.txt 3600 13948.3 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_3.txt 3600 13757.0 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_4.txt 3600 13572.2 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_5.txt 3600 14066.8 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_6.txt 3600 14070.9 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_7.txt 3600 14066.8 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_8.txt 3600 13991.2 0",
		"DLLSMA.exe ../Instances/Homberger/C1_6_9.txt 3600 13701.5 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_10.txt 3600 7123.9 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_1.txt 3600 7752.2 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_2.txt 3600 7471.5 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_3.txt 3600 7215.0 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_4.txt 3600 6877.8 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_5.txt 3600 7553.8 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_6.txt 3600 7449.8 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_7.txt 3600 7491.4 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_8.txt 3600 7303.7 0",
		"DLLSMA.exe ../Instances/Homberger/C2_6_9.txt 3600 7303.2 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_10.txt 3600 17648.1 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_1.txt 3600 21274.2 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_2.txt 3600 18571.7 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_3.txt 3600 16940.7 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_4.txt 3600 15773.6 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_5.txt 3600 19305.7 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_6.txt 3600 17790.2 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_7.txt 3600 16537.2 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_8.txt 3600 15617.3 0",
		"DLLSMA.exe ../Instances/Homberger/R1_6_9.txt 3600 18510.0 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_10.txt 3600 11851.6 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_1.txt 3600 15145.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_2.txt 3600 12986.9 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_3.txt 3600 10455.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_4.txt 3600 7940.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_5.txt 3600 13809.7 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_6.txt 3600 11849.1 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_7.txt 3600 9811.0 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_8.txt 3600 7512.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_6_9.txt 3600 12741.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_10.txt 3600 15672.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_1.txt 3600 16981.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_2.txt 3600 15982.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_3.txt 3600 15232.1 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_4.txt 3600 14778.2 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_5.txt 3600 16580.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_6.txt 3600 16534.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_7.txt 3600 16091.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_8.txt 3600 15955.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_6_9.txt 3600 15837.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_10.txt 3600 9014.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_1.txt 3600 11979.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_2.txt 3600 10336.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_3.txt 3600 8894.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_4.txt 3600 6989.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_5.txt 3600 11082.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_6.txt 3600 10843.1 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_7.txt 3600 10310.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_8.txt 3600 9820.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_6_9.txt 3600 9449.4 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_10.txt 3600 24030.4 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_1.txt 3600 25156.9 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_2.txt 3600 24974.1 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_3.txt 3600 24198.9 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_4.txt 3600 23812.7 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_5.txt 3600 25138.6 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_6.txt 3600 25133.3 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_7.txt 3600 25127.3 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_8.txt 3600 24882.5 0",
		"DLLSMA.exe ../Instances/Homberger/C1_8_9.txt 3600 24200.4 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_10.txt 3600 10946.0 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_1.txt 3600 11631.9 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_2.txt 3600 11394.5 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_3.txt 3600 11138.1 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_4.txt 3600 10682.8 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_5.txt 3600 11395.6 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_6.txt 3600 11316.3 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_7.txt 3600 11332.9 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_8.txt 3600 11133.9 0",
		"DLLSMA.exe ../Instances/Homberger/C2_8_9.txt 3600 11140.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_10.txt 3600 31059.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_1.txt 3600 36393.3 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_2.txt 3600 32352.3 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_3.txt 3600 29429.1 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_4.txt 3600 27844.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_5.txt 3600 33640.0 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_6.txt 3600 30989.5 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_7.txt 3600 28954.7 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_8.txt 3600 27715.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_8_9.txt 3600 32381.1 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_10.txt 3600 20016.5 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_1.txt 3600 24986.8 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_2.txt 3600 21330.2 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_3.txt 3600 17238.1 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_4.txt 3600 13198.4 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_5.txt 3600 22802.0 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_6.txt 3600 19754.5 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_7.txt 3600 16400.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_8.txt 3600 12708.5 0",
		"DLLSMA.exe ../Instances/Homberger/R2_8_9.txt 3600 21298.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_10.txt 3600 28210.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_1.txt 3600 30039.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_2.txt 3600 28363.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_3.txt 3600 27504.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_4.txt 3600 26595.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_5.txt 3600 29306.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_6.txt 3600 29247.1 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_7.txt 3600 28858.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_8.txt 3600 28505.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_8_9.txt 3600 28400.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_10.txt 3600 14445.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_1.txt 3600 19213.2 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_2.txt 3600 16712.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_3.txt 3600 14046.1 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_4.txt 3600 11031.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_5.txt 3600 17493.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_6.txt 3600 17250.1 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_7.txt 3600 16445.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_8.txt 3600 15601.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_8_9.txt 3600 15251.7 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_10.txt 7200 39874.5 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_1.txt 7200 42444.8 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_2.txt 7200 41392.7 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_3.txt 7200 40146.2 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_4.txt 7200 39490.9 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_5.txt 7200 42434.8 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_6.txt 7200 42437.0 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_7.txt 7200 42420.4 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_8.txt 7200 41837.8 0",
		"DLLSMA.exe ../Instances/Homberger/C1_10_9.txt 7200 40366.7 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_10.txt 7200 15728.6 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_1.txt 7200 16841.1 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_2.txt 7200 16462.6 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_3.txt 7200 16036.5 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_4.txt 7200 15482.9 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_5.txt 7200 16521.3 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_6.txt 7200 16290.7 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_7.txt 7200 16378.4 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_8.txt 7200 16029.1 0",
		"DLLSMA.exe ../Instances/Homberger/C2_10_9.txt 7200 16077.0 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_10.txt 7200 47570.6 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_1.txt 7200 53233.9 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_2.txt 7200 48369.1 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_3.txt 7200 44862.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_4.txt 7200 42577.9 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_5.txt 7200 50487.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_6.txt 7200 47145.8 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_7.txt 7200 44231.4 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_8.txt 7200 42435.9 0",
		"DLLSMA.exe ../Instances/Homberger/R1_10_9.txt 7200 49334.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_10.txt 7200 29956.9 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_1.txt 7200 36899.4 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_2.txt 7200 31296.2 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_3.txt 7200 24422.1 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_4.txt 7200 17968.1 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_5.txt 7200 34233.0 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_6.txt 7200 29215.3 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_7.txt 7200 23243.8 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_8.txt 7200 17512.7 0",
		"DLLSMA.exe ../Instances/Homberger/R2_10_9.txt 7200 32104.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_10.txt 7200 43745.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_1.txt 7200 45948.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_2.txt 7200 43870.6 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_3.txt 7200 42338.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_4.txt 7200 41469.2 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_5.txt 7200 45235.5 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_6.txt 7200 45144.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_7.txt 7200 44686.2 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_8.txt 7200 44130.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC1_10_9.txt 7200 44072.4 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_10.txt 7200 21848.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_1.txt 7200 28190.8 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_2.txt 7200 24271.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_3.txt 7200 19747.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_4.txt 7200 15804.9 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_5.txt 7200 25892.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_6.txt 7200 25871.0 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_7.txt 7200 24451.7 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_8.txt 7200 23332.3 0",
		"DLLSMA.exe ../Instances/Homberger/RC2_10_9.txt 7200 22859.3 0",
	};

	for (auto cmd : cmds) {
		
		hust::MyString ms;
		auto arr = ms.split(cmd," ");
		//hust::printve(arr);
		//hust::allocGlobalMem(argc, argv);
		argv = new char* [argc];

		for (int i = 0; i < argc; ++i) {
			argv[i] = const_cast<char*>(arr[i].data());
		}
	#endif // 0

	hust::allocGlobalMem(argc,argv);
	hust::Goal goal;

	//goal.justLocalSearch(); 
	//goal.justLocalSearch(); 
	goal.CVB2LocalSearch(); 
	//goal.TwoAlgCombine();
	//goal.test();
	hust::deallocGlobalMem();
	//}
	return 0;
}