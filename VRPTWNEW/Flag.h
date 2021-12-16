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

	int IntInf = 0x3f3f3f3f;
	long long int LLInf = 0x3f3f3f3f3f3f3f3f/4;
	double LFInf = DBL_MAX;

	double LFINF = 1.79769e+308;
	double PI = 3.14159265359;
	double PI_2 = PI / 2;
	double PI_4 = PI / 4;
	double PI_8 = PI / 8;
	double PI_16 = PI / 16;
	double PI_32 = PI / 32;

	//TODO[lyh][000]:dimacsÊÇ10
	int disMul = 10000;
	unsigned Mod = 1000000007;

	using LL = long long int;

	template<typename T, typename U>
	using UnorderedMap = std::unordered_map<T, U>;

	template<typename V>
	using UnorderedSet = std::unordered_set<V>;

	template<typename V>
	using Vec = std::vector<V>;
	
	using DisType = LL;
	DisType DisInf = LLInf;

	struct Random;
	struct RandomX;
	struct Configuration;

	Random* myRand = nullptr;
	RandomX* myRandX = nullptr;
	Vec<Vec<LL>> * yearTable = nullptr;
	Configuration* cfg = nullptr;
	LL squIter = 1;
}

#endif // !vrptwNew_FLAG_H


