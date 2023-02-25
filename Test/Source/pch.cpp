//
// pch.cpp
//

#include "pch.h"
#include "../../Source/Solver.cpp"
#include "../../Source/YearTable.cpp"
#include "../../Source/EAX.cpp"
#include "../../Source/AlgorithmParameters.cpp"
#include "../../Source/Input.cpp"
#include "../../Source/Util.cpp"

namespace hust {
namespace lyhtest {

std::vector< std::vector< std::string> > getTestCommandLineArgv() {
	static std::vector< std::vector< std::string> > argvs = {
		{ "./DLLSMA","-instancePath", "../../Instances/Homberger/C1_8_2.txt", "-time", "3600", "-psizemulpsum", "0", "-seed", "1665441954","-ejectLSMaxIter","10","-bksDataFileBasePath","../../Instances/DataFromLiterature/"},
	};
	return argvs;
}

}
}
