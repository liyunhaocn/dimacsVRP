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

		//{ "./DLLSMA", "../../Instances/Homberger/C1_8_2.txt",
		//"../../SolverParameters.json","../../Results"},

		{ "./DLLSMA",
		"../../Instances/ortec-instances/ORTEC-VRPTW-ASYM-00c5356f-d1-n258-k12.txt",
		"../../SolverParameters.json","../../Results"},
	};
	return argvs;
}

}
}
