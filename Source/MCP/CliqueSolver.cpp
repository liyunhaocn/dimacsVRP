#include "CliqueSolver.h"

#include <thread>

using namespace std;


namespace hustmcp {
namespace tsm {

int loadInput(NextEdge nextEdge, const Arr<Weight> &nodeWeights,
    InitOrdering initOrdering = InitOrdering::DegreeDegeneracy);
bool tsmMain(Clique &sln, int msTimeout);


bool solveWeightedMaxClique(Clique &sln, NextEdge nextEdge, const Arr<Weight> &nodeWeights, Millisecond timeout) {
    bool r = false;
    thread t([&]() {
        if (loadInput(nextEdge, nodeWeights, InitOrdering::DegreeDegeneracy)) {
            r = tsmMain(sln, timeout);
        }
    });
    t.join();
    return r;
}

bool solveWeightedMaxClique(Clique &sln, NextEdge nextEdge, ID nodeNum, Millisecond timeout) {
    return solveWeightedMaxClique(sln, nextEdge, Arr<Weight>(nodeNum, 1), timeout);
}

bool solveWeightedMaxClique(Clique &sln, const AdjList &adjList, const Arr<Weight> &nodeWeights, Millisecond timeout) {
    ID i = 0;
    ID j = 0;
    auto nextEdge = [&](ID &src, ID &dst) {
        if (j >= adjList[i].size()) {
            j = 0;
            ++i;
        }
        if (i >= adjList.size()) {
            i = 0;
            return false;
        }
        src = i;
        dst = adjList[i][j].dst;
        ++j;
        return true;
    };
    return solveWeightedMaxClique(sln, nextEdge, nodeWeights, timeout);
}

bool solveWeightedMaxClique(Clique &sln, const AdjList &adjList, Millisecond timeout) {
    return solveWeightedMaxClique(sln, adjList, Arr<Weight>(adjList.size(), 1), timeout);
}

bool solveWeightedMaxClique(Clique &sln, const EdgeList &edgeList, const Arr<Weight> &nodeWeights, Millisecond timeout) {
    auto j = edgeList.begin();
    auto nextEdge = [&](ID &src, ID &dst) {
        if (j == edgeList.end()) {
            j = edgeList.begin();
            return false;
        }
        src = j->src;
        dst = j->dst;
        ++j;
        return true;
    };
    return solveWeightedMaxClique(sln, nextEdge, nodeWeights, timeout);
}

bool solveWeightedMaxClique(Clique &sln, const EdgeList &edgeList, ID nodeNum, Millisecond timeout) {
    return solveWeightedMaxClique(sln, edgeList, Arr<Weight>(nodeNum, 1), timeout);
}

bool solveWeightedMaxClique(Clique &sln, const AdjMat &adjMat, const Arr<Weight> &nodeWeights, Millisecond timeout) {
    ID i = 0;
    ID j = 0;
    auto nextEdge = [&](ID &src, ID &dst) {
        for (;;) {
            if (j >= adjMat.size1()) {
                j = 0;
                ++i;
            }
            if (i >= adjMat.size1()) {
                i = 0;
                return false;
            }
            if (!adjMat.at(i, j)) {
                ++j;
                continue;
            }
            src = i;
            dst = j;
            ++j;
            return true;
        }
    };
    return solveWeightedMaxClique(sln, nextEdge, nodeWeights, timeout);
}

bool solveWeightedMaxClique(Clique &sln, const AdjMat &adjMat, Millisecond timeout) {
    return solveWeightedMaxClique(sln, adjMat, Arr<Weight>(adjMat.size1(), 1), timeout);
}

bool solveWeightedIndependentSet(Clique &sln, const AdjMat &adjMat, const Arr<Weight> &nodeWeights, Millisecond timeout) {
    ID i = 0;
    ID j = 0;
    auto nextEdge = [&](ID &src, ID &dst) {
        for (;;) {
            if (j >= adjMat.size1()) {
                j = 0;
                ++i;
            }
            if (i >= adjMat.size1()) {
                i = 0;
                return false;
            }
            if (adjMat.at(i, j)) {
                ++j;
                continue;
            }
            src = i;
            dst = j;
            ++j;
            return true;
        }
    };
    return solveWeightedMaxClique(sln, nextEdge, nodeWeights, timeout);
}

bool solveWeightedIndependentSet(Clique &sln, const AdjMat &adjMat, Millisecond timeout) {
    return solveWeightedIndependentSet(sln, adjMat, Arr<Weight>(adjMat.size1(), 1), timeout);
}

}
}
