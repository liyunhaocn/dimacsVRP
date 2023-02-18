
#include <climits>
#include "Flag.h"
#include "Util_Common.h"

namespace hust {

int IntInf = INT_MAX/10;
//extern long long int LLInf = 0x3f3f3f3f3f3f3f3f / 4;
//extern long long int LLInf = LLONG_MAX / 4 - 1;
long long int LLInf = LLONG_MAX / 10;

double PI = 3.14159265359;
double PI_2 = PI / 2;
double PI_4 = PI / 4;
double PI_8 = PI / 8;
double PI_16 = PI / 16;
double PI_32 = PI / 32;

int vd2pi = 65536;
int vdpi = vd2pi/2;
int vd2fpi = vdpi/2;
int vd4fpi = vd2fpi/2;
int vd8fpi = vd4fpi/2;
int vd16fpi = vd8fpi/2;

// dimacs «10
int disMul = 10000;
DisType DisInf = LLInf;

//extern int disMul = 10000;
unsigned Mod = 1000000007;

}
