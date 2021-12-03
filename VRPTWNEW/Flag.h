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

#define LINUX 0
#define ATTRIBUTETABU 1

#define LOGINFO 1

#define CHECKING 0
#define DISDOUBLE 0

#define max(a,b)  (((a) > (b)) ? (a) : (b))
#define min(a,b)  (((a) < (b)) ? (a) : (b))

#define debug(x) cout<<#x<<": "<<(x)<<endl
#define deOut(x) cout<<#x<<": "<<(x)<<" "
#define de15lf(x) {cout<<#x<<": "; printf("%.15lf\n",x);}
#define outVe(arr) {cout<<#arr<<": ";for(auto i:arr){cout<<i<<", ";}cout<<endl;}
#define DEFabs(a,b) ((a)>=(b))?((a)-(b)):((b)-(a))

#define lfeq4(a,b)  ((a) > (b) ? ((a)-(b) < 1e-4):((b)-(a) < 1e-4) )
#define lfeq5(a,b)  ((a) > (b) ? ((a)-(b) < 1e-5):((b)-(a) < 1e-5) )
#define lfeq7(a,b)  ((a) > (b) ? ((a)-(b) < 1e-7):((b)-(a) < 1e-7) )
#define lfeq8(a,b)  ((a) > (b) ? ((a)-(b) < 1e-8):((b)-(a) < 1e-8) )
#define lfeq9(a,b)  ((a) > (b) ? ((a)-(b) < 1e-9):((b)-(a) < 1e-9) )
#define lfeq10(a,b)  ((a) > (b) ? ((a)-(b) < 1e-10):((b)-(a) < 1e-10) )
#define lfeq11(a,b)  ((a) > (b) ? ((a)-(b) < 1e-11):((b)-(a) < 1e-11) )
#define lfeq12(a,b)  ((a) > (b) ? ((a)-(b) < 1e-12):((b)-(a) < 1e-12) )
#define lfeq13(a,b)  ((a) > (b) ? ((a)-(b) < 1e-13):((b)-(a) < 1e-13) )
#define DISlfeq lfeq10


namespace vrptwNew {

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

	using LL = long long int;

	template<typename T, typename U>
	using UnorderedMap = std::unordered_map<T, U>;

	template<typename V>
	using UnorderedSet = std::unordered_set<V>;

	template<typename V>
	using Vec = std::vector<V>;

#if DISDOUBLE
	using DisType = double;
	DisType DisInf = LFInf;
#else
	using DisType = LL;
	DisType DisInf = LLInf;
#endif // DISDOUBLE

}

#endif // !vrptwNew_FLAG_H


