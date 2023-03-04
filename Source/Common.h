
#ifndef CN_HUST_LYH_FLAG_H
#define CN_HUST_LYH_FLAG_H

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <numeric>
#include <iostream>
#include <list>
#include <climits>
#include <cassert>

#include "Arr2D.h"
#include "json.h"

#define LYH_CHECKING 1
#if LYH_CHECKING

#define lyhCheckTrue(x) {				\
	if(!(x) ){							\
		std::cout						\
		<< "[" << __FILE__ << "]"		\
		<< "[line:" << __LINE__ << "]"  \
		<< "[" << __FUNCTION__ << "]: " \
		<< #x <<std::endl;				\
		assert(x);						\
	}									\
}										
#else
#define lyhCheckTrue(x) ;
#endif // LYH_CHECKING

namespace hust {

template<typename T, typename U>
using UnorderedMap = std::unordered_map<T, U>;

template<typename V>
using UnorderedSet = std::unordered_set<V>;

template<typename V>
using Vector = std::vector<V>;

using String = std::string;

template<typename V>
using List = std::list<V>;

using LL = long long int;

using DisType = LL;

using Json = nlohmann::json;

static constexpr int IntInf = INT_MAX / 10;
static constexpr long long int LLInf = LLONG_MAX / 10;

static constexpr double PI = 3.14159265359;
static constexpr double PI_2 = PI / 2;
static constexpr double PI_4 = PI / 4;
static constexpr double PI_8 = PI / 8;
static constexpr double PI_16 = PI / 16;
static constexpr double PI_32 = PI / 32;

static constexpr int vd2pi = 65536;
static constexpr int vdpi = vd2pi / 2;
static constexpr int vd2fpi = vdpi / 2;
static constexpr int vd4fpi = vd2fpi / 2;
static constexpr int vd8fpi = vd4fpi / 2;
static constexpr int vd16fpi = vd8fpi / 2;

// dimacs is 10
static constexpr int disMul = 10;
static constexpr DisType DisInf = LLInf;

//static constexpr int disMul = 10000;
static constexpr unsigned Mod = 1000000007;

}
#endif // !CN_HUST_LYH_FLAG_H


