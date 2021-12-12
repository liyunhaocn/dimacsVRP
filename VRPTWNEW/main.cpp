﻿// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <functional>
#include <numeric>
//#include "./VRPTW/Environment.h"
//#include "./VRPTW/Utility.h"
//#include "./VRPTW/Solver.h"
//#include "./VRPTW/Problem.h"
//#include "./VRPTW/Configuration.h"

#include "./Environment.h"
#include "./Utility.h"
#include "./Solver.h"
#include "./Problem.h"
#include "./Configuration.h"
#include "./Flag.h"
#include "./EAX.h"

//#include "./getBound.h"

namespace vrpSln = vrptwNew;
//namespace vrpSln = lyh;
using namespace std;

#include<cstdlib>
#include<cstring>
#include<cmath>
#include<iostream>
#include<algorithm>
#include<queue>
#include<vector>
#include<map>

void findbug() {

	vrpSln::Environment env("../Instances/Solomon/C101.txt");
	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	vrpSln::println("__DATE__: ", __DATE__);
	vrpSln::println("__TIME__: ", __TIME__);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}
	env.seed = 1611589828;
	auto env2 = env;

	env2.seed = env.seed + 1611589828;
	env.show();
	cfg.show();
	vrpSln::Input input(env, cfg);

	vrpSln::Vec< vrpSln::Vec<int> > vpa = {
{581,654,125,954,39,782,857,256,404,510},
{811,153,986,27,189,680,563,411},
{997,337,947,588,637,515,166,496,751},
{754,645,858,835,288,430,802,550,531,483,154,868},
{363,797,45,68,575,295,180,302,187,19,24,217},
{981,368,918,444,59,699,335,52,933,561,714},
{544,278,511,560,603,773,415,705,122},
{403,614,866,664,16,669,731,375,701,349,492},
{66,297,695,491,8,821,150,383,129,778,545},
{346,685,549,558,342,354,555,528,788,476,552,578,451},
{623,228,598,556,649,99,273,717,599},
{815,816,81,385,505,142,516,431,317,652},
{914,362,969,213,251,455,252,86,439,266},
{819,246,911,61,744,62,53,912,34,736,922,818,938},
{573,72,613,206,427,684,960,833,56,640,735},
{231,1,70,611,807,738,757,863,330,248,162,107,734},
{38,262,212,859,618,63,192,12,929},
{648,949,197,767,214,198,64,589,632,366},
{306,294,504,766,386,909,259,803,165,128,691,668},
{473,109,728,519,424,119,604,742,493,282,745},
{655,458,487,233,554,85,423,950,572,289,899,255,585},
{336,254,133,913,539,111,94,58,587,242,323,164},
{285,518,315,688,967,474,9,4,919,666},
{985,25,1000,478,477,398,5,410,774},
{380,18,89,393,472,356,910,862,567,161,227,828,433,999},
{921,479,437,205,696,26,163,173,931},
{115,486,743,440,397,526,223,962,939,123,494},
{779,595,468,296,377,995,449,546,759,602,299},
{849,861,339,457,748,325,673,854,783,638,924},
{338,33,209,927,509,465,222,502,820,406,786},
{785,267,837,104,132,925,568,67,7,443,605},
{6,118,897,202,574,210,980,268,621,485},
{167,890,11,721,253,768,881,650,770,873},
{283,152,112,157,29,314,508,651,309,565,3},
{116,813,840,906,71,850,943,920,723,880,703},
{535,303,226,671,978,851,789,553,103,124,537,656,817},
{982,707,551,622,450,827,932,832,270,35,677,343},
{76,631,467,333,639,229,730,319,901,792,874,983,136},
{600,274,352,196,871,674,662,870,869},
{190,842,50,675,822,459,830,872,903,127,351},
{460,682,475,235,596,429,749,454,328,344,683,501},
{740,825,370,384,727,432,853,361,839,608,994,447},
{780,529,514,719,204,37,746,582,300,965,989,917},
{293,542,928,73,15,733,396,446,619,952,741,469,87},
{722,718,326,334,414,100,955,139,184,379},
{175,84,867,908,847,916,31,279,276,365},
{957,787,806,710,318,831,961,609,877,843,891,507},
{488,800,211,804,715,926,923,808,824,355},
{764,879,426,846,624,799,594,503,399,98,82,156},
{382,784,708,852,907,517,445,201,634,200,92},
{461,678,316,172,805,795,114,876,74,481,313,616},
{286,277,988,964,888,732,979,898,359,442,667,615},
{287,772,506,409,739,208,944,422,974,401},
{95,834,557,381,271,215,630,750,310,532,641,895,234},
{238,626,471,260,391,281,592,464,658,612,725},
{54,607,889,14,700,724,135,177,769,195,791,340},
{322,627,698,462,47,389,977,390,593,131,57,291},
{498,438,643,374,513,418,953,159,148,562,65,186},
{301,777,360,763,236,80,264,642,709,559,569},
{44,17,105,97,269,571,948,713,185,606,258,823},
{661,844,143,371,218,968,956,915},
{856,758,456,402,207,670,646,263,726},
{224,312,199,120,521,996,590,155,576,676},
{958,341,91,96,378,75,865,681,752,350,845,970},
{348,686,151,998,193,801,987,466,963,41},
{108,182,762,693,203,716,706,875,88,659,110,42,145},
{482,625,990,584,60,809,993,864,790,280,43,512},
{937,181,407,102,934,720,126,2,265,541,522,441,40},
{364,237,882,400,570,178,170,219,579,538,160},
{704,798,51,591,141,239,976,220,55,883,848,495},
{28,775,973,188,527,245,793,636,147,747,547},
{941,32,520,357,814,697,838,261,369,421,324,633},
{610,543,679,298,902,781,886,174,586,463,46,249},
{305,30,884,644,900,499,353,405,548},
{760,272,321,388,930,241,230,169,540,660},
{712,191,308,395,90,417,77,23,524,394},
{896,497,48,935,484,653,975,434,765,244,984,428},
{327,243,776,435,101,392,453,860,221,992},
{307,940,534,620,959,841,304,480,580,810},
{753,290,179,419,942,885,892,144,373,692,194},
{756,292,146,171,525,93,470,601,232,665,137},
{536,257,121,966,635,247,113,452,448,367,347,83,500},
{106,158,412,331,972,240,216,408,887,690,275},
{564,320,372,991,894,672,416,284,878},
{176,79,49,489,490,436,387,812,729,413,711},
{36,130,628,945,20,689,138,905,946,225,329,533,577,530},
{737,69,21,566,420,951,796,694,836,583,702},
{617,687,755,10,117,829,523,657,629,250,826,647},
{597,425,936,332,345,134,149,358,761,771,168},
{904,971,794,855,13,78,376,893,311,140,22,663,183}
	};

	vrpSln::Vec< vrpSln::Vec<int> > vpb = {
{781,246,943,850,828,161,71,567,433,999,880,703},
{355,923,926,888,732,979,898,359,964,988,277,286},
{997,312,947,588,854,673,166,70,638},
{985,25,1000,478,477,5,410,774,856},
{488,800,211,442,804,715,808,667,824,615},
{656,168,358,789,553,103,974,422,401,944,124,537},
{224,199,737,120,521,996,590,155,576,676},
{971,904,725,368,654,918,335,699,444,59,140,22,561,663},
{66,297,920,227,491,8,821,150,129,778,968},
{758,398,456,565,670,646,263,207,402,3},
{704,10,55,807,314,863,657,523,734,873},
{9,89,393,472,356,680,189,153,586},
{914,362,86,969,213,251,455,260,439,266},
{819,911,61,744,62,53,912,34,736,922,910,811},
{836,69,21,566,420,495,951,796,694,583,702},
{641,33,209,927,509,222,465,196,871,674,406,786},
{38,633,63,618,212,324,421,369,261,838,814},
{648,949,197,767,214,198,64,589,632,366},
{306,294,766,343,930,241,230,169,540,660},
{678,316,705,462,698,415,593,131,57,291,186,143},
{655,976,220,239,141,591,51,798,458,637,848,647,273,717},
{307,940,534,620,810,304,480,684,833,783},
{285,518,315,688,967,474,18,105,97,919,4,666},
{514,825,932,832,270,232,601,108,525,791,696},
{380,44,17,505,142,516,958,317,258,823,652},
{379,724,184,14,139,700,754,334,727,994,35},
{115,962,486,743,512,440,939,123,494,67,443,605},
{136,32,520,357,697,859,262,192,12,929},
{849,267,837,104,132,925,526,223,217,568,7},
{194,84,867,779,908,595,847,916,619,952,365},
{785,56,738,233,554,85,423,950,881,650,487,755},
{6,268,980,210,574,118,897,202,547,621},
{780,529,719,582,193,801,987,466,726},
{573,72,613,206,427,580,841,959,960,339,735},
{937,181,407,102,934,720,126,2,265,818,938,411,46,933},
{130,628,794,855,13,78,376,893,311,714},
{982,707,450,419,622,942,885,609,843,877,290,753},
{76,631,467,333,639,229,730,88,110,42,145},
{751,1,640,861,457,748,325,496,617,231,337},
{190,842,50,675,822,459,830,872,903,127,351},
{805,564,320,372,894,672,74,313,481},
{931,173,151,998,803,319,901,792,686,941,884},
{327,243,776,107,162,757,651,309,31,41},
{167,890,11,721,253,453,860,101,112,157,29,221,87},
{581,577,249,945,20,689,138,905,946,225,329,533},
{175,468,296,377,995,892,891,507},
{957,787,806,710,318,831,961,323,164,242,179},
{896,497,48,935,484,653,454,445,201,634,846,328},
{764,879,344,200,624,799,594,503,399,98,82,156},
{36,562,148,159,418,953,560,513,374,661},
{712,191,308,395,90,417,545,383,695,723,862,116,394},
{687,883,611,770,768,255,572,289,899,829,117,585},
{252,287,772,506,626,471,238,391,281,592,464,817},
{921,479,437,205,746,37,204,300,965,740,989,917},
{530,563,986,27,463,906,541,886,174,522,840,813},
{665,182,470,93,762,693,203,716,706,875,659,137},
{536,257,121,966,635,247,113,452,448,367,347,83,500},
{336,283,392,152,629,250,330,248,826,435,992},
{23,543,679,902,298,119,604,742,493,282,441},
{544,278,511,122,461,47,389,603,977,390},
{889,645,955,100,414,430,802,550,531,483,756,504},
{106,158,412,331,972,240,216,408,812,269,865},
{473,109,524,40,610,77,424,519,728,65},
{485,425,149,332,936,208,761,771,658,612},
{292,171,839,608,447,384,607,135,177,769,54,26},
{305,30,348,386,909,259,165,128,691,668,874,983,548},
{388,760,272,321,353,644,900,499,405},
{600,274,352,662,502,820,870,869},
{535,303,226,671,978,851,134,345,409,739,597},
{623,228,598,556,649,99,599,515,924},
{28,775,973,188,527,245,793,636,147,747},
{403,614,690,79,16,669,490,49,387,349,492},
{981,510,857,256,782,39,125,954,404,52},
{95,834,557,381,271,215,630,750,310,532,172,559,569},
{460,682,475,235,596,429,749,517,92,426,683,501},
{363,797,45,68,575,295,180,302,187,19,397,558},
{570,350,664,489,436,701,375,731,866,887,845,752},
{144,373,692,133,913,539,111,94,759,546,602,449,299},
{382,784,708,852,428,907,984,975,434,765,244},
{293,542,928,73,15,446,396,733,741,469,276,279,963},
{722,718,326,288,835,858,361,853,432,195,163,340},
{482,625,990,584,60,809,993,864,790,280,43,24},
{431,400,882,237,170,178,219,364,176,75,571,606},
{498,438,218,371,745,643,844,956,915,183},
{681,91,96,711,413,729,970,275,378,948,341},
{773,322,627,878,234,795,114,895,416,284,876,616},
{301,777,360,763,236,80,264,642,709,991,338},
{508,254,551,58,587,868,154,370,827,146,677},
{346,685,549,342,555,354,528,788,476,552,578,451},
{815,160,81,385,816,538,579,713,185},
	};

	vrpSln::Solver pa(input, cfg, env);
	vrpSln::Solver pb(input, cfg, env2);


	pa.initByArr2(vpa);
	pb.initByArr2(vpb);
	//pa.saveOutAsSintefFile();
	//pb.saveOutAsSintefFile();

	vrpSln::EAX eax(pa, pb,0);

	vrpSln::Solver pc(pa);

	debug(pa.verify());
	debug(pb.verify());

	pa.printDimacs();

	auto newCus = eax.doNaEAX(pa, pb, pc);

}

bool run(int argc, char* argv[]) {

	vrpSln::Environment env("../Instances/Homberger/C1_8_2.txt");

	vrpSln::Configuration cfg;
	solveCommandLine(argc, argv, cfg, env);

	//env.seed = 1611589828;

	debug(argc);
	env.show();
	cfg.show();
	
	vrpSln::DateTime d(time(0));
	cout << d << endl;
	debug("compile tag:");
	debug(__DATE__);
	debug(__TIME__);
	
	vrpSln::Vec<string> e1 = {"C1_", "C2_" ,"R1_","R2_","RC1_","RC2_"};
	vrpSln::Vec<string> e2 = { "2_","4_","6_","8_","10_" };
	vrpSln::Vec<string> e3 = { "11","12","13","14","15","16","17","18","19","20" };

	/*Vec<string> allCase;
	for (string a : e1) {
	for (string b : e2) {
	for (string c : e3) {
		allCase.push_back(a + b + c);
		cfg.breakRecord = 1;
	}
	}
	}*/

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);
	
	solver.minimizeRN();

	auto& P = solver.P;
	string path = env.outputPath;
	path += "PVal.";
	path += input.example;
	path += ".csv";

	auto f = freopen(path.c_str(), "a", stdout);

	for (std::size_t i = 0; i < P.size(); ++i) {
		cout << P[i] << ", ";
	}
	cout << endl;

	auto f2 = freopen("CON", "a", stdout);

	vrpSln::saveSln(input, solver.output, cfg, env);
	//solver.saveOutAsSintefFile();

	return true;
}

bool solverByEAX(int argc, char* argv[]) {

	//vrpSln::Environment env("../Instances/Solomon/C101.txt");
	vrpSln::Environment env("../Instances/Homberger/RC1_8_3.txt");

	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}
	env.seed = 1611589828;
	env.show();
	cfg.show();

	vrpSln::myRand = new vrpSln::Random(env.seed);
	vrpSln::myRandX = new vrpSln::RandomX(env.seed);

	vrpSln::Input input(env, cfg);
	vrpSln::Solver pBest(input, cfg, env);
	//TODO[lyh][0]:
	pBest.RoutesCost = vrpSln::DisInf;

	int contiNoDown = 1;

	vrpSln::Timer t1(cfg.runTimer);
	t1.setLenUseSecond(cfg.runTimer);
	t1.reStart();

	auto updateBestSol = [&](vrpSln::Solver& pTemp) {
		if (pTemp.RoutesCost < pBest.RoutesCost) {
			pBest = pTemp;
			contiNoDown = 1;
			pBest.saveToOutPut();
			vrpSln::println("cost:", pBest.RoutesCost);
			return true;
		}
		else {
			return false;
		}
		return false;
	};

	int popSize = 3;

	vrpSln::Vec<vrpSln::Solver> pool;
	pool.reserve(popSize);
	for (int i = 0; i < popSize; ++i) {
		vrpSln::Environment envt = env;
		envt.seed = env.seed + ( (i+1) * (vrpSln::myRand->pick(10000007))) % vrpSln::Mod;
		vrpSln::Solver st(input, cfg, envt);
		st.minimizeRN();
		st.mRLLocalSearch({});
		if (st.RoutesCost < pBest.RoutesCost) {
			pBest = st;
		}
		pool.push_back(st);
	}

	while (pBest.RoutesCost > input.sintefRecRL && !t1.isTimeOut()){
		
		int paIndex = vrpSln::myRand->pick( popSize );
		int pbIndex = (paIndex + vrpSln::myRand->pick(popSize-1) + 1) % popSize;
		/*debug(paIndex);
		debug(pbIndex);*/
		vrpSln::Solver& pa  = pool[paIndex];
		vrpSln::Solver& pb  = pool[pbIndex];
		vrpSln::Solver pc = pa;
		
		unsigned eaxSeed = env.seed + (vrpSln::myRand->pick(10000007)) % vrpSln::Mod;
		vrpSln::EAX eax(pa, pb, eaxSeed);
		auto newCus = eax.doNaEAX(pa, pb, pc);

		if (eax.repairSolNum == 0) {

			// TODO[lyh][0]: 如果是没有ab环 那么就是说这两个解一模一样,把b的函数值升高以跳出局部最优
			if (eax.abCycleSet.size() == 0) {
				vrpSln::println("no abcycle");
				bool isRuin = pb.ruinLocalSearch();
				if (isRuin) {
					vrpSln::println("ruin success", "333pb.RoutesCost: ", pb.RoutesCost);
				}
				else {
					vrpSln::println("ruin fail patternAdjustment");
					pb.patternAdjustment(input.custCnt * 0.1);
					pb.mRLLocalSearch({});
				}
				updateBestSol(pb);
				continue;
			}
			else {
				// TODO[lyh][1]: 如果是有ab环 重新生成ab环，采用path relinking
				vrpSln::println("has abcycle repeat gen abcy");
				
				int succRepairNum = 0;
				auto pcColone = pa;

				for (int i = 0; i < 3; ++i) {

					auto newCusTemp = eax.doNaEAX(pa, pb, pcColone);
					if (eax.repairSolNum > 0) {
						++succRepairNum;
						if (succRepairNum == 1) {
							vrpSln::println("na get first");
							pc = pcColone;
							newCus = newCusTemp;
						}
						else if (succRepairNum > 1 && pcColone.RoutesCost < pc.RoutesCost) {
							vrpSln::println("na is better");
							pc = pcColone;
							newCus = newCusTemp;
						}
					}

					newCusTemp = eax.doPrEAX(pa, pb, pcColone);
					if (eax.repairSolNum > 0) {
						++succRepairNum;
						if (succRepairNum == 1) {
							vrpSln::println("path get first");
							pc = pcColone;
							newCus = newCusTemp;
						}
						else if (succRepairNum > 1 && pcColone.RoutesCost < pc.RoutesCost) {
							vrpSln::println("path is better");
							pc = pcColone;
							newCus = newCusTemp;
						}
					}
				}

				if (succRepairNum == 0) {
					vrpSln::println("repeat fail");
					continue;
				}
				else {
					vrpSln::println("repeat gen succeess");
				}
			}
		}

		if (pc.penalty > 0) {
			vrpSln::println("error pc penalty > 0");
		}
		pc.mRLLocalSearch(newCus);
		//pc.mRLLocalSearch({});
		updateBestSol(pc);
		if (pc.RoutesCost < pa.RoutesCost) {
			pa = pc;
		}
		++contiNoDown;

		if (contiNoDown % popSize == 0) {


			//pa = pBest;
			pa.patternAdjustment(input.custCnt * 0.1);
			pa.mRLLocalSearch({});
			updateBestSol(pa);

			pool[0] = pBest;
			/*bool isRuin = pb.ruinLocalSearch();
			if (isRuin) {
				vrpSln::println("333pb.RoutesCost: ", pb.RoutesCost);
			}*/
			/*else {
				pb.patternAdjustment(30);
				pb.mRLLocalSearch({});
			}*/
			updateBestSol(pb);
		}
	}

	pBest.output.runTime = t1.getRunTime();
	vrpSln::saveSln(input, pBest.output, cfg, env);
	t1.disp();

	delete vrpSln::myRand;
	delete vrpSln::myRandX;

	return true;
}

int main(int argc, char* argv[])
{

	//std::thread t2(run,argc, argv); // pass by value
	//std::thread t4(run, argc, argv); // t4 is now running f2(). t3 is no longer a thread
	//t2.join();
	//t4.join();
	//solverByEAX(argc, argv);
	//makeCases(argc, argv);
	//testGetbound(argc, argv);
	//return 0;
	//for(;;)
	vrpSln::Vec<string> all63 = { "C1_10_1", "C1_10_5", "C1_10_6", "C1_10_7", "C1_10_8", "C1_8_1", "C1_8_5", "C1_8_6", "C1_8_7",
								"C1_8_8", "C1_6_1", "C1_6_5", "C1_6_6", "C1_6_7", "C1_4_1", "C1_4_5", "C1_4_6", "C1_4_7",
								"C1_4_8", "C1_2_1", "C1_2_5", "C1_2_6", "C1_2_7", "C1_2_8",

								"C2_10_1", "C2_10_2", "C2_10_5", "C2_10_6", "C2_10_7", "C2_10_9", "C2_8_1", "C2_8_2", "C2_8_3",
								"C2_8_5", "C2_8_6", "C2_8_7", "C2_8_8", "C2_8_9", "C2_8_10", "C2_6_1", "C2_6_5", "C2_6_6",
								"C2_6_7", "C2_4_1", "C2_4_2", "C2_4_5", "C2_4_6", "C2_4_7", "C2_4_9",

										"R1_10_1", "R1_8_1", "R1_6_1", "R1_4_1", "R1_2_1",

										"RC2_10_1", "RC2_8_1", "RC2_8_2", "RC2_6_1", "RC2_6_2", "RC2_4_1", "RC2_4_2", "RC2_2_1", "RC2_2_2" 
	};

	solverByEAX(argc, argv);
	//run(argc, argv);

	return 0;
}


