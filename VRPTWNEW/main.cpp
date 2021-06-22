// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

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

#define For(i, a, b) for(int i = (a); i <= (int)(b); i++)

struct GetBound {

	int INF = 0x3f3f3f3f;
	vector<vector<bool>> G;
	int n, S;
	vector<vector<int>> all, some, none;
	GetBound() {

		G = vector<vector<bool>>(1001,vector<bool>(1001));
		all = vector<vector<int>>(1001,vector<int>(1001));
		some = vector<vector<int>>(1001,vector<int>(1001));
		none = vector<vector<int>>(1001,vector<int>(1001));
	}

	void dfs(int d, int an, int sn, int nn) {
		if (!sn && !nn) S++;
		//if (S > 1000) return;

		int u = some[d][1];
		For(i, 1, sn) {
			int v = some[d][i];
			if (G[u][v]) continue;
			int tsn = 0, tnn = 0;

			For(j, 1, an) all[d + 1][j] = all[d][j];
			all[d + 1][an + 1] = v;

			For(j, 1, sn)
				if (G[v][some[d][j]]) some[d + 1][++tsn] = some[d][j];
			For(j, 1, nn)
				if (G[v][none[d][j]]) none[d + 1][++tnn] = none[d][j];

			dfs(d + 1, an + 1, tsn, tnn);

			some[d][i] = 0; none[d][++nn] = v;
		}
	}

	int getMinRouteNum() {
		//for()
		For(i, 1, n) some[0][i] = i;

		S = 0;
		dfs(0, 0, n, 0);

		if (S <= 1000) printf("%d\n", S);
		else printf("Too many maximal sets of friends.\n");

		return S;
	}

};

bool run(int argc, char* argv[]) {

	vrpSln::Environment env("C1_6_6");

	vrpSln::Configuration cfg;
	//cfg.breakRecord = 1;
	//cfg.runTimer = 3600;
	//env.seed = 1622779129;
	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
		debug(env.seed)
	}

	vrpSln::DateTime d(time(0));
	cout << d << endl;
	deOut("compile tag:")debug("0602-1")
	debug(argc)

	//env.seed = 1610679669;
	//env.seed = 1610863258;
	
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
	
	//solver.gamma = 0;

	//solver.testRoute();

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

	//solver.minimizeRL();
	//solver.testRoute();
	

	vrpSln::saveSln(input, solver.output, cfg, env);

	return true;
}

bool solverByEAX(int argc, char* argv[]) {

	vrpSln::Environment env;
	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	vrpSln::DateTime d(time(0));
	cout << d << endl;
	//deOut("compile tag:")debug("0424-2")
	debug(argc)

	solveCommandLine(argc, argv, cfg, env);

	if (env.seed == -1) {
		env.seed = std::time(nullptr) + std::clock();
	}

	//env.seed = 1611589828;
	//cfg.breakRecord = 1;

	vrpSln::Input input(env, cfg);

	vrpSln::Solver pa(input, cfg, env);
	vrpSln::Solver pb(input, cfg, env);
	
	/*pa.initBybestSolution();
	pa.patternAdjustment(-1,1000);
	pb.initBybestSolution();*/
	
	//pa.minimizeRN();
	//pa.updateRtsCost();
	////pa.initBybestSolution();
	//debug(pa.RoutesCost)
	//pa.patternAdjustment(-1,1000);
	//pa.minimizeRL();
	//debug(pa.RoutesCost)
	//debug(pa.verify())
	//debug(pa.verify())

	pb.minimizeRN();
	//pb.patternAdjustment(-1, 1000);
	pb.minimizeRL();
	debug(pb.RoutesCost)

	return true;

	vrpSln::Solver pc(pa);
	vrpSln::Solver pBest(pa);
	int contiNoDown = 0;

	while (pa.RoutesCost > input.sintefRecRL){

		pc = pa;
		vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
		bool eaSucceed = eax.doEAX(pa, pb, pc);

		while (!eaSucceed) {
			debug(eaSucceed)
			pb.patternAdjustment(-1, 100);
			pb.minimizeRL();
			pc = pa;
			vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
			eaSucceed = eax.doEAX(pa, pb, pc);
		}

		bool reSucceed = pc.repair();

		while(!reSucceed) {

			debug(reSucceed)
			pb.patternAdjustment(-1, 500);
			pb.minimizeRL();
			pc = pa;
			vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
			eax.doEAX(pa, pb, pc);
			reSucceed = pc.repair();
		}

		pc.minimizeRL();

		if (pc.RoutesCost < pa.RoutesCost) {
			pa = pc;
			contiNoDown = 0;

			//pc.saveToOutPut();
			//vrpSln::saveSln(input, pc.output, cfg, env);
			//debug(pc.RoutesCost)
		}
		
		if (pc.RoutesCost < pBest.RoutesCost) {
			pBest = pc;
			contiNoDown = 0;
			pBest.saveToOutPut();
			vrpSln::saveSln(input, pBest.output, cfg, env);
			debug(pc.RoutesCost)
		}
		else {
			++contiNoDown;
		}

		if (contiNoDown > 5) {
			pa = pBest;
			pb.patternAdjustment(-1, 5000);
			pb.minimizeRL();
			
			contiNoDown = 0;
		}
	}
	
	vrpSln::saveSln(input, pa.output, cfg, env);

	return true;
}


bool testGetbound(int argc, char* argv[]) {

	vrpSln::Environment env("C1_2_6");

	vrpSln::Configuration cfg;

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);

	int n = input.custCnt;

	GetBound g;
	//g.n = n;
	g.n = 100;

	auto& G = g.G;

	for (int i = 1; i < G.size(); ++i) {
		for (int j = 1; j < G[i].size(); ++j) {
			G[i][j] = 0;
		}
	}

	vrpSln::Route r = solver.rCreateRoute(0);

	for (int i = 1; i <= n; ++i) {
		for (int j = i + 1; j <= n; ++j) {
			
			solver.rInsAtPosPre(r,r.tail,i);
			solver.rInsAtPosPre(r,r.tail,j);
			solver.rUpdateAvQfrom(r,r.head);
			bool s1 = (r.rPtw == 0);
			solver.rRemoveAllCusInR(r);

			solver.rInsAtPosPre(r,r.tail,j);
			solver.rInsAtPosPre(r,r.tail,i);
			solver.rUpdateAvQfrom(r,r.head);
			bool s2 = (r.rPtw == 0);
			solver.rRemoveAllCusInR(r);

			if (s1 || s2) {
				g.G[i][j] = g.G[j][i] = true;
			}
			
		}
	}

	debug(g.getMinRouteNum())

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
	/*testGetbound(argc, argv);
	return 0;*/
	//for(;;)
	run(argc, argv);
	
	return 0;
}


