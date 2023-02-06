
#include <climits>
#include "Flag.h"
#include "Util_Common.h"

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

extern int vd2pi = 65536;
extern int vdpi = vd2pi/2;
extern int vd2fpi = vdpi/2;
extern int vd4fpi = vd2fpi/2;
extern int vd8fpi = vd4fpi/2;
extern int vd16fpi = vd8fpi/2;

// dimacs «10
extern int disMul = 10000;
extern DisType DisInf = LLInf;

//extern int disMul = 10000;
extern unsigned Mod = 1000000007;

extern bool dimacsGo = false;

}
