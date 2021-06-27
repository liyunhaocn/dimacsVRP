#ifndef vrptwNew_GETBOUND_H
#define vrptwNew_GETBOUND_H


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


#include "../MCP/CliqueSolver.h"

namespace tsm = szx::tsm;

struct GetBound1 {

	vector< vector<int> >G;//图
	int n;
	vector<int> inset;//当前在团中的点
	vector<int> chosen;//最大团的点集合
	int nownum;//当前最大团点的数量
	int bestnum;//最大团点的数量

	GetBound1(int maxSize) {

		G = vector<vector<int>>(maxSize + 1, vector<int>(maxSize + 1, 0));
		inset = vector<int>(maxSize + 1, 0);
		chosen = vector<int>(maxSize + 1, 0);
	}

	void dfs(int i) {
		//如果所有节点都被深搜了
		if (i > n) {
			chosen = inset;//更新最大团的点和数量
			bestnum = nownum;
			return;
		}
		bool flag = 1;//当前点是否与当前最大团中的点都相连
		for (int j = 0; j < nownum; j++) {
			if (G[i][inset[j]] == 0) {
				flag = false;
				break;
			}
		}
		//是的话，可以加入到最大团中
		if (flag) {
			inset[nownum++] = i;
			dfs(i + 1);
			nownum--;
		}

		//否则不加人，和一个小小的剪枝
		if (nownum + (n - i) > bestnum)
			dfs(i + 1);
	}

	int getMinRouteNum() {

		nownum = 0;
		bestnum = 0;
		dfs(1);//从第一个节点开始深搜

		printf("%d\n", bestnum);
		for (int i = 0; i < bestnum; i++) {
			if (i != bestnum - 1)
				printf("%d ", chosen[i]);
			else
				printf("%d\n", chosen[i]);
		}
		return bestnum;
	}

};


bool testGetbound1() {

	vrpSln::Environment env("R1_2_6");

	vrpSln::Configuration cfg;

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);

	int n = input.custCnt;

	GetBound1 g(n);
	//g.n = n;
	g.n = n;

	auto& G = g.G;

	for (int i = 1; i < G.size(); ++i) {
		for (int j = 1; j < G[i].size(); ++j) {
			G[i][j] = 0;
		}
	}

	vrpSln::Route r = solver.rCreateRoute(0);

	for (int i = 1; i <= n; ++i) {
		for (int j = i + 1; j <= n; ++j) {

			solver.rInsAtPosPre(r, r.tail, i);
			solver.rInsAtPosPre(r, r.tail, j);
			solver.rUpdateAvQfrom(r, r.head);
			bool s1 = (r.rPtw == 0);
			solver.rRemoveAllCusInR(r);

			solver.rInsAtPosPre(r, r.tail, j);
			solver.rInsAtPosPre(r, r.tail, i);
			solver.rUpdateAvQfrom(r, r.head);
			bool s2 = (r.rPtw == 0);
			solver.rRemoveAllCusInR(r);

			if (s1 && s2) {
				g.G[i][j] = g.G[j][i] = 1;
			}
		}
	}

	debug(g.getMinRouteNum())

		return true;
}


vector<int> testBoundJiangHua(int n, vector<vector<int>>& g) {

	tsm::AdjMat mt(n,n);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			mt.at(i, j) = g[i][j];
		}
	}

	szx::Arr<tsm::Weight> weights(n, 1);
	tsm::Clique sln;

	auto printSln = [](const tsm::Clique& sln) {
		cout << "weight=" << sln.weight << endl;
		cout << "clique=";
		for (auto n = sln.nodes.begin(); n != sln.nodes.end(); ++n) {
			cout << *n << " ";
		}
		cout << endl;
	};
	bool isFind = tsm::solveWeightedMaxClique(sln, mt, weights);
	/*if (isFind) {
		printSln(sln);
	}*/
	return sln.nodes;
}


int getBound(string ex ="R1_2_6") {

	vrpSln::Environment env(ex);

	vrpSln::Configuration cfg;

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);

	int n = input.custCnt;

	vector<vector<int>> G(n,vector<int>(n,1));

	vrpSln::Route r = solver.rCreateRoute(1);

	for (int i = 1; i <= n; ++i) {
		for (int j = i + 1; j <= n; ++j) {

			solver.rInsAtPosPre(r, r.tail, i);
			solver.rInsAtPosPre(r, r.tail, j);
			solver.rUpdateAvQfrom(r, r.head);
			bool s1 = (r.rPtw == 0 && r.rPc==0);
			solver.rRemoveAllCusInR(r);

			solver.rInsAtPosPre(r, r.tail, j);
			solver.rInsAtPosPre(r, r.tail, i);
			solver.rUpdateAvQfrom(r, r.head);
			bool s2 = (r.rPtw == 0 && r.rPc == 0);
			solver.rRemoveAllCusInR(r);

			if (s1 || s2) {
				G[i-1][j-1] = G[j-1][i-1] = 0;
			}
		}
	}

	//auto maxq = testBoundJiangHua(n, G);
	int delCnt = 0;

	auto maxq = testBoundJiangHua(n, G);
	debug(maxq.size())
		for (auto i : maxq) {
			cout << i << " ";
		}
	cout << endl;
	return maxq.size();

	while (n > 0) {

		auto maxq = testBoundJiangHua(n, G);
		vector<int> mp(n,-1);
		vector<bool> isdel(n,0);
		for (int i = 0; i < maxq.size(); ++i) {
			isdel[maxq[i]] = 1;
		}

		int id = 0;
		for (int i = 0; i < n; ++i) {
			if (isdel[i] == 0) {
				mp[id] = i;
				++id;
			}
		}

		vector<vector<int>> gclone(id,vector<int>(id,0));
		for (int i = 0; i < id; ++i) {
			for (int j = 0; j < id; ++j) {
				gclone[i][j] = G[ mp[i] ][ mp[j] ] ;
			}
		}

		n = id;
		G = gclone;
		++delCnt;
	}
	
	debug(input.sintefRecRN)
	debug(delCnt)
	return delCnt;
}

#endif // vrptwNew_GETBOUND_H