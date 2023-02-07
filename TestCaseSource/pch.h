//
// pch.h
//

#pragma once

#include "gtest/gtest.h"
#include "../Source/Flag.h"
#include "../Source/Solver.h"
#include "../Source/YearTable.h"
#include "../Source/EAX.h"
#include "../Source/Algorithm_Parameters.h"
#include "../Source/Problem.h"
#include "../Source/CommandLine.h"
#include "../Source/Util_Logger.h"

namespace hust {
namespace lyhtest {
	
std::vector< std::vector< std::string> > getTestCommandLineArgv();

}
}