
#ifndef CN_HUST_LYH_FLAG_H
#define CN_HUST_LYH_FLAG_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <iostream>
#include <list>

#define DIMACSGO 1
#define ATTRIBUTETABU 1

#define CHECKING 0

#define lyhCheckTrue(x) {				\
	if(!(x) ){							\
		std::cout						\
		<< "[" << __FILE__ << "]"		\
		<< "[line:" << __LINE__ << "]"  \
		<< "[" << __FUNCTION__ << "]: " \
		<< #x <<std::endl;				\
	}									\
}

namespace hust {

template<typename T, typename U>
using UnorderedMap = std::unordered_map<T, U>;

template<typename V>
using UnorderedSet = std::unordered_set<V>;

template<typename V>
using Vec = std::vector<V>;

template<typename V>
using List = std::list<V>;

using LL = long long int;

//TODO[0] 都用LL了 为了找10倍DIMACSGO的bug
#if DIMACSGO
using DisType = LL;
#else
using DisType = LL;
#endif // DIMACSGO

extern int IntInf;
extern long long int LLInf;
extern DisType DisInf;

extern double PI;
extern double PI_2;
extern double PI_4;
extern double PI_8;
extern double PI_16;
extern double PI_32;

extern int disMul;
extern unsigned Mod;

struct Random;
struct RandomX;
struct Configuration;
struct Input;
struct BKS;

extern Random* myRand;
extern RandomX* myRandX;
extern int squIter;
extern Vec<Vec<int>>* yearTable;
extern Configuration* globalCfg;
extern Input* globalInput;
extern BKS* bks;

}
#endif // !CN_HUST_LYH_FLAG_H


