//
// pch.cpp
//

#include "pch.h"
#include "../Source/Flag.cpp"
#include "../Source/Solver.cpp"
#include "../Source/yearTable.cpp"
#include "../Source/EAX.cpp"
#include "../Source/Algorithm_Parameters.cpp"
#include "../Source/Problem.cpp"
//#include "../../Source/LowerBoundOfRouteNum.h"

namespace hust {
namespace lyhtest {

std::vector< std::vector< std::string> > getTestCommandLineArgv() {
	static std::vector< std::vector< std::string> > argvs = {
		{ "./DLLSMA","-ins", "../Instances/Homberger/C1_8_2.txt", "-time", "3600", "-psizemulpsum", "0", "-seed", "1665441954","-ejectLSMaxIter","10"},
	};
	return argvs;
}

}
}
