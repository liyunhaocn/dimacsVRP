////////////////////////////////
/// usage : 1.	the interface for weighted max clique solvers.
/// 
/// note  : 1.	you may need to modify the MAX_NODE macro to imrpove performance or handle huge graphs.
///         2.	the core algorithm may fail if there are zero-weight nodes.
////////////////////////////////

#ifndef SMART_HUST_GOAL_MAX_CLIQUE_LIB_CLIQUE_SOLVER_H
#define SMART_HUST_GOAL_MAX_CLIQUE_LIB_CLIQUE_SOLVER_H


#include <functional>
#include <limits>

#include "Graph.h"


namespace hustmcp {
namespace tsm {

using ID = Graph::ID; // node ID counts from 0 in the interface, but counts from 1 inside the LKH.
using Weight = long long;
using Millisecond = int;

using AdjNode = Graph::AdjNode;
using Edge = Graph::Edge;

using AdjMat = Graph::AdjMat<bool>;
using AdjList = Graph::AdjList<AdjNode>;
using EdgeList = Graph::EdgeList<Edge>;

using NodeList = Graph::List<ID>; // `nodeList` is a list of node IDs in increasing order.
using NodeSet = Graph::List<bool>; // `nodeSet[n]` is true if node `n` is included in the set.

using Clique = Graph::Clique<Weight>;

using NextEdge = std::function<bool(ID &src, ID &dst)>; // auto reset after the end of edges.


static constexpr ID CliqueIdBase = 1;
static constexpr Millisecond Forever = (std::numeric_limits<Millisecond>::max)();
static constexpr Millisecond DefaultTimeout = Forever;

enum InitOrdering {
    DegreeDegeneracy = 1,
    WeightDegeneracy = 3
};


// EXTEND[szx][0]: support enumerating good cliques.
// EXTEND[szx][1]: support fixing node to be in some clique.
//                 this can be done by simple preprocess that remove all nodes other than the adjacent nodes of the nodes to be fixed.
bool solveWeightedMaxClique(Clique &sln, NextEdge nextEdge, const Arr<Weight> &nodeWeights, Millisecond timeout = DefaultTimeout);
bool solveWeightedMaxClique(Clique &sln, NextEdge nextEdge, ID nodeNum, Millisecond timeout = DefaultTimeout);

bool solveWeightedMaxClique(Clique &sln, const AdjList &adjList, const Arr<Weight> &nodeWeights, Millisecond timeout = DefaultTimeout);
bool solveWeightedMaxClique(Clique &sln, const AdjList &adjList, Millisecond timeout = DefaultTimeout);

bool solveWeightedMaxClique(Clique &sln, const EdgeList &edgeList, const Arr<Weight> &nodeWeights, Millisecond timeout = DefaultTimeout);
bool solveWeightedMaxClique(Clique &sln, const EdgeList &edgeList, ID nodeNum, Millisecond timeout = DefaultTimeout);

bool solveWeightedMaxClique(Clique &sln, const AdjMat &adjMat, const Arr<Weight> &nodeWeights, Millisecond timeout = DefaultTimeout);
bool solveWeightedMaxClique(Clique &sln, const AdjMat &adjMat, Millisecond timeout = DefaultTimeout);

bool solveWeightedIndependentSet(Clique &sln, const AdjMat &adjMat, const Arr<Weight> &nodeWeights, Millisecond timeout = DefaultTimeout);
bool solveWeightedIndependentSet(Clique &sln, const AdjMat &adjMat, Millisecond timeout = DefaultTimeout);

}
}


#endif // SMART_HUST_GOAL_MAX_CLIQUE_LIB_CLIQUE_SOLVER_H
