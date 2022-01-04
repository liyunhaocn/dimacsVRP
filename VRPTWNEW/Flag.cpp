
#include <climits>
#include "Flag.h"
#include "Utility.h"

namespace hust {

extern int IntInf = INT_MAX/10;
//extern long long int LLInf = 0x3f3f3f3f3f3f3f3f / 4;
//extern long long int LLInf = LLONG_MAX / 4 - 1;
extern long long int LLInf = LLONG_MAX / 10;

extern double PI = 3.14159265359;
extern double PI_2 = PI / 2;
extern double PI_4 = PI / 4;
extern double PI_8 = PI / 8;
extern double PI_16 = PI / 16;
extern double PI_32 = PI / 32;

#if DIMACSGO
//TODO[lyh][000]:dimacs «10
extern int disMul = 10;
extern DisType DisInf = LLInf;
#else
//TODO[lyh][000]:dimacs «10
extern int disMul = 10000;
extern DisType DisInf = LLInf;
#endif // 0

//extern int disMul = 10000;
extern unsigned Mod = 1000000007;

extern Random* myRand = nullptr;
extern RandomX* myRandX = nullptr;
extern Vec<Vec<int>>* yearTable = nullptr;
extern Configuration* globalCfg = nullptr;
extern Input* globalInput = nullptr;
extern BKS* bks = nullptr;
extern int squIter = 1;

}
