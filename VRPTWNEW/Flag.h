#ifndef vrptwNew_FLAG_H
#define vrptwNew_FLAG_H

//#if LINUX
//#include <libgen.h>
//#include <unistd.h>
//#endif // LINUX
#include <float.h>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <iostream>

//#include "Environment.h"
//#include "Solver.h"

#define LINUX 0
#define ATTRIBUTETABU 1
#define LOGINFO 1

#define CHECKING 0

#define debug(x) std::cout<<#x<<": "<<(x)<<std::endl
#define deOut(x) std::cout<<#x<<": "<<(x)<<" "
#define de15lf(x) {std::cout<<#x<<": "; printf("%.15lf\n",x);}
#define outVe(arr) {std::cout<<#arr<<": ";for(auto i:arr){std::cout<<i<<", ";}std::cout<<std::endl;}
#define DEFabs(a,b) ((a)>=(b))?((a)-(b)):((b)-(a))

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
using DisType = LL;

extern int IntInf;
extern long long int LLInf;
extern double LFInf;
extern DisType DisInf;

extern double LFINF;
extern double PI;
extern double PI_2;
extern double PI_4;
extern double PI_8;
extern double PI_16;
extern double PI_32;

extern int disMul;
extern unsigned Mod;
extern LL squIter;

struct Random;
struct RandomX;
struct Configuration;
struct Environment;
struct Input;

extern Random* myRand;
extern RandomX* myRandX;
extern Vec<Vec<LL>>* yearTable;
extern Configuration* cfg;
extern Environment* globalEnv;
extern Input* globalInput;

}
#endif // !vrptwNew_FLAG_H


