#include "./Flag.h"
#include "./Environment.h"
#include "./Configuration.h"

namespace hust {

extern int IntInf = 0x3f3f3f3f;
extern long long int LLInf = 0x3f3f3f3f3f3f3f3f / 4;
extern double LFInf = DBL_MAX;

double LFINF = 1.79769e+308;
double PI = 3.14159265359;
double PI_2 = PI / 2;
double PI_4 = PI / 4;
double PI_8 = PI / 8;
double PI_16 = PI / 16;
double PI_32 = PI / 32;

extern DisType DisInf = LLInf;

//TODO[lyh][000]:dimacs «10
int disMul = 10000;
unsigned Mod = 1000000007;

extern Random* myRand = nullptr;
extern RandomX* myRandX = nullptr;
extern Vec<Vec<LL>>* yearTable = nullptr;
extern Configuration* cfg = nullptr;
extern Environment* globalEnv = nullptr;
extern Input* globalInput = nullptr;

extern LL squIter = 1;



}
