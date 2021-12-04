﻿// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <functional>
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


bool run(int argc, char* argv[],string ex="C1_6_6") {

	vrpSln::Environment env(ex);

	vrpSln::Configuration cfg;
	solveCommandLine(argc, argv, cfg, env);

	/*cfg.breakRecord = 0;
	env.seed = 1634129452;
	*/

	debug(argc);
	env.show();
	cfg.show();
	
	vrpSln::DateTime d(time(0));
	cout << d << endl;
	debug("compile tag:");
	debug(__DATE__);
	debug(__TIME__);
	
	vector<string> e1 = {"C1_", "C2_" ,"R1_","R2_","RC1_","RC2_"};
	vector<string> e2 = { "2_","4_","6_","8_","10_" };
	vector<string> e3 = { "11","12","13","14","15","16","17","18","19","20" };

	/*vector<string> allCase;
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

	for (int i = 0; i < P.size(); ++i) {
		cout << P[i] << ", ";
	}
	cout << endl;

	auto f2 = freopen("CON", "a", stdout);

	vrpSln::saveSln(input, solver.output, cfg, env);
	//solver.saveOutAsSintefFile();

	return true;
}

bool solverByEAX(int argc, char* argv[]) {

	vrpSln::Environment env("C1_10_4");
	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	vrpSln::println("__DATE__: ",__DATE__);
	vrpSln::println("__TIME__: ",__TIME__);

	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}
	env.seed = 1611589828;
	auto env2 = env;

	env2.seed = env.seed + 1611589828;
	env.show();
	cfg.show();
	vrpSln::Input input(env, cfg);

	vrpSln::Solver pa(input, cfg, env);
	vrpSln::Solver pb(input, cfg, env2);
	
	pa.minimizeRN();
	//pb.patternAdjustment(-1, 1000);
	pa.minimizeRL();

	pb.minimizeRN();
	//pb.patternAdjustment(-1, 1000);
	pb.minimizeRL();

	vrpSln::Solver pc(pa);
	vrpSln::Solver pBest(pa);

	int contiNoDown = 1;

	vrpSln::Timer t1(3600);

	t1.setLenUseSecond(3600);
	t1.reStart();

	while (pa.RoutesCost > input.sintefRecRL && !t1.isTimeOut()){

		pc = pa;
		pb.patternAdjustment(10);
		pb.minimizeRL();

		vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
		bool eaSucceed = eax.doEAX(pa, pb, pc);

		if (!eaSucceed) {
			//debug(eaSucceed)
			pb.patternAdjustment(30);
			pb.minimizeRL();
		}

		if (pc.RoutesCost < pa.RoutesCost) {
			pa = pc;
			contiNoDown = 0;
		}
		
		if (pc.RoutesCost < pBest.RoutesCost) {
			pBest = pc;
			contiNoDown = 0;
			pBest.saveToOutPut();
			//vrpSln::saveSln(input, pBest.output, cfg, env);
			vrpSln::println("pc.RoutesCost: ",pc.RoutesCost);
			vrpSln::println("pBest.RoutesCost: ", pBest.RoutesCost);
			vrpSln::println("pBest.ver: ", pBest.verify());

		}
		else {
			++contiNoDown;
		}

		if (contiNoDown % 5 ==0 ) {
			pa = pBest;
			pb.patternAdjustment(50);
			pb.minimizeRL();
		}

		if (contiNoDown % 20 ==0 ) {
			pa = pBest;
			pb.patternAdjustment(pc.input.custCnt*0.2);
			pb.minimizeRL();
		}
	}
	
	vrpSln::saveSln(input, pBest.output, cfg, env);
	t1.disp();
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
	vector<string> all63 = { "C1_10_1", "C1_10_5", "C1_10_6", "C1_10_7", "C1_10_8", "C1_8_1", "C1_8_5", "C1_8_6", "C1_8_7",
								"C1_8_8", "C1_6_1", "C1_6_5", "C1_6_6", "C1_6_7", "C1_4_1", "C1_4_5", "C1_4_6", "C1_4_7",
								"C1_4_8", "C1_2_1", "C1_2_5", "C1_2_6", "C1_2_7", "C1_2_8",

								"C2_10_1", "C2_10_2", "C2_10_5", "C2_10_6", "C2_10_7", "C2_10_9", "C2_8_1", "C2_8_2", "C2_8_3",
								"C2_8_5", "C2_8_6", "C2_8_7", "C2_8_8", "C2_8_9", "C2_8_10", "C2_6_1", "C2_6_5", "C2_6_6",
								"C2_6_7", "C2_4_1", "C2_4_2", "C2_4_5", "C2_4_6", "C2_4_7", "C2_4_9",

										"R1_10_1", "R1_8_1", "R1_6_1", "R1_4_1", "R1_2_1",

										"RC2_10_1", "RC2_8_1", "RC2_8_2", "RC2_6_1", "RC2_6_2", "RC2_4_1", "RC2_4_2", "RC2_2_1", "RC2_2_2" 
	};

	/*for (auto ex : all63) {
		run(argc, argv,ex);
	}*/

	solverByEAX(argc, argv);
	//run(argc, argv);

	/*vector<string> boundCase;
	map<string,int> boundofex;
	map<string,int> sinofex;
	for (auto ex : all63) {
		debug(ex)
		int bound = getBound(ex);
		boundofex[ex] = bound;
		
		vrpSln::Environment env(ex);
		vrpSln::Configuration cfg;

		vrpSln::Input in(env,cfg);
		sinofex[ex] = in.sintefRecRN;
		if (bound == in.sintefRecRN) {
			boundCase.push_back(ex);
		}
		
	}

	for (auto i : all63) {
		cout << i << " " << sinofex[i] << " " << boundofex[i] << endl;;
	}
	cout << endl;

	for (string i : boundCase) {
		cout << i << ",";
	}
	cout << endl;*/

	return 0;
}


