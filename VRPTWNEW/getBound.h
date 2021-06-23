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

class GetBound
{


public:
	GetBound() {

	}


	void addNode(int inNodeIndex) {
		if (mNodes.find(inNodeIndex) != mNodes.end())
			return;  //Throw Exception that Node already exists
		mNodes.insert(inNodeIndex);
		mAdjacencyList.insert(std::make_pair(inNodeIndex, std::set<int>()));
	}

	void addEdge(int inStartNode, int inEndNode) {
		if (mNodes.find(inStartNode) == mNodes.end()) {
			cout << "Nodes doesn't adding the Start Node" << endl;
			mNodes.insert(inStartNode);
			mAdjacencyList.insert(std::make_pair(inStartNode, std::set<int>()));
		}
		if (mNodes.find(inEndNode) == mNodes.end()) {
			cout << "Nodes doesn't adding the End Node" << endl;
			mNodes.insert(inEndNode);
			mAdjacencyList.insert(std::make_pair(inEndNode, std::set<int>()));
		}

		//Check if there is already in the Adjacency List
		if (mAdjacencyList.find(inStartNode) == mAdjacencyList.end()) {
			mAdjacencyList.insert(std::make_pair(inStartNode, std::set<int>()));
		}
		else {
			mAdjacencyList.find(inStartNode)->second.insert(inEndNode);
		}

		if (mAdjacencyList.find(inEndNode) == mAdjacencyList.end()) {
			mAdjacencyList.insert(std::make_pair(inEndNode, std::set<int>()));
		}
		else {
			mAdjacencyList.find(inEndNode)->second.insert(inStartNode);
		}
	}


	bool are_adjacent(int inNode1, int inNode2) {
		//Checks if Node1 and Node2 are adjacent
		return  mAdjacencyList[inNode1].find(inNode2) != mAdjacencyList[inNode1].end() ? true : false;
	}

	std::set<int> get_node_neighbours(int inNode) {
		return mAdjacencyList[inNode];
	}


	std::vector< std::set<int> > find_all_cliques(int min_vertex_in_clique = 3) {
		//Implements Bron-Kerbosch algorithm , Version 2
		std::vector< std::set<int> > cliques;
		typedef std::tuple< std::set<int>, std::set<int>, std::set<int>, int, int   >  stackNode;
		std::stack< stackNode > stack_;
		int nd = -1;
		int disc_num = mNodes.size();
		stackNode search_node = std::make_tuple(set<int>(), set<int>(mNodes), set<int>(), nd, disc_num);
		stack_.push(search_node);
		while (stack_.size() > 0) {
			stackNode tupel = stack_.top();
			stack_.pop();

			set<int> c_compsub = std::get<0>(tupel);
			set<int> c_candidates = std::get<1>(tupel);
			set<int> c_not = std::get<2>(tupel);
			int c_nd = std::get<3>(tupel);
			int c_disc_cum = std::get<4>(tupel);

			if (c_candidates.size() == 0 && c_not.size() == 0) {
				//Here we are pushing the Cliques
				if (c_compsub.size() >= min_vertex_in_clique) {
					//Before Pushing Back check if it already exists or not
					bool alreadyPresent = false;
					for (int i = 0; i < cliques.size(); ++i) {
						if (cliques[i].size() == c_compsub.size()) {
							bool setMatched = true;
							set<int>::iterator it1, it2;
							for (it1 = cliques[i].begin(), it2 = c_compsub.begin();
								it1 != cliques[i].end() && it2 != c_compsub.end();
								++it1, ++it2) {
								if ((*it1) != (*it2)) {
									setMatched = false;
									break;
								}
							}

							if (setMatched) {
								alreadyPresent = true;
								break;
							}
						}
					}

					if (false == alreadyPresent)
						cliques.push_back(std::set<int>(c_compsub));
					continue;
				}
			}

			std::set<int>::iterator it = c_candidates.begin();
			while (it != c_candidates.end()) {
				int u = *it;
				it++;

				if ((c_nd == -1) || !(are_adjacent(u, c_nd))) {
					c_candidates.erase(u);
					std::set<int> Nu = get_node_neighbours(u);
					std::set<int> new_compsub = set<int>(c_compsub);
					new_compsub.insert(u);
					set<int> new_candidates;
					set_intersection(c_candidates.begin(), c_candidates.end(), Nu.begin(), Nu.end(), std::inserter(new_candidates, new_candidates.begin()));

					set<int> new_not;
					set_intersection(c_not.begin(), c_not.end(), Nu.begin(), Nu.end(), std::inserter(new_not, new_not.begin()));

					if (c_nd != -1) {
						if (new_not.find(c_nd) != new_not.end()) {
							int new_disc_num = c_disc_cum - 1;
							if (new_disc_num > 0) {
								stackNode new_search_node = std::make_tuple(new_compsub, new_candidates, new_not, c_nd, new_disc_num);
								stack_.push(new_search_node);
							}
						}
						else {
							int new_disc_num = mNodes.size();
							int new_nd = c_nd;
							for (int cand_nd : new_not) {
								set<int> tempSet;
								set<int> tempNeightBours = get_node_neighbours(c_nd);
								set_intersection(new_candidates.begin(), new_candidates.end(),
									tempNeightBours.begin(), tempNeightBours.end(),
									std::inserter(tempSet, tempSet.begin()));

								int cand_disc_num = new_candidates.size() - tempSet.size();

								if (cand_disc_num < new_disc_num) {
									new_disc_num = cand_disc_num;
									new_nd = cand_nd;
								}
							}

							stackNode new_search_node = std::make_tuple(new_compsub, new_candidates, new_not, new_nd, new_disc_num);
							stack_.push(new_search_node);
						}
					}
					else {
						stackNode new_search_node = std::make_tuple(new_compsub, new_candidates, new_not, c_nd, c_disc_cum);
						stack_.push(new_search_node);
					}

					c_not.insert(u);
					int new_disc_num = 0;
					for (int x : c_candidates) {
						if (false == are_adjacent(x, u)) {
							new_disc_num += 1;
						}
					}

					if (new_disc_num < c_disc_cum && new_disc_num > 0) {
						stackNode new1_search_node = std::make_tuple(c_compsub, c_candidates, c_not, u, new_disc_num);
						stack_.push(new1_search_node);
					}
					else {
						stackNode new1_search_node = std::make_tuple(c_compsub, c_candidates, c_not, c_nd, c_disc_cum);
						stack_.push(new1_search_node);

					}
				}

			}
		}


		return cliques;

	}

private:
	std::set<int> mNodes;
	std::map< int, std::set<int> > mAdjacencyList;


};


bool testGetbound1(int argc, char* argv[]) {

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


bool testGetbound(int n,vector<vector<int>>g) {

	vrpSln::Environment env("R1_2_6");

	vrpSln::Configuration cfg;

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);

	int n = input.custCnt / 2;

	GetBound g = GetBound();

	for (int i = 0; i < n; ++i) {
		g.addNode(i);
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

			if (s1 || s2) {
				g.addEdge(i - 1, j - 1);
			}
		}
	}

	std::vector< std::set<int >  > cliques = g.find_all_cliques();

	for (auto i : cliques) {
		for (auto j : i) {
			cout << j << " ";
		}
		cout << endl;
	}

	return true;
}

#endif // vrptwNew_GETBOUND_H