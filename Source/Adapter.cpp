
#include "Adapter.h"
#include "Util_Common.h"

#include "Util_Arr2D.h"
#include "CommandLine.h"
#include "YearTable.h"
#include "LowerBoundOfRouteNum.h"
#include "Util_Logger.h"
#include "MCP/CliqueSolver.h"

namespace hust {

	std::vector<std::vector<int>> dynamicRun(hust::CommandLine& commandline) {

		Timer timer(commandline.runTimer);

		AlgorithmParameters& aps = commandline.aps;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);

		commandline.displayInfo();
		input.displayInfo();
		aps.displayInfo();

		// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
		aps.repairByCusCnt(input.custCnt);

		YearTable yearTable(&input);

		BKS bks(&input, &yearTable, &timer);

		//hust::Goal goal(&input, &aps, &bks, &yearTable);

		hust::Solver solver(&input, &yearTable, &bks);

		solver.dynamicRuin(aps.ruinC_);

		return bks.bestSolFound.saveToOutPut().rts;
	}

	std::vector<std::vector<int>> dimacsRun(hust::CommandLine& commandline) {

		Timer timer(commandline.runTimer);

		AlgorithmParameters& aps = commandline.aps;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);

		commandline.displayInfo();
		input.displayInfo();
		aps.displayInfo();

		// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
		aps.repairByCusCnt(input.custCnt);

		YearTable yearTable(&input);

		BKS bks(&input, &yearTable, input.timer);

		hust::Goal goal(&input, &aps, &bks, &yearTable);

		goal.TwoAlgCombine();
		return bks.bestSolFound.saveToOutPut().rts;
	}

	std::vector<std::vector<int>> routeNumberMinimization(hust::CommandLine& commandline) {

		Timer timer(commandline.runTimer);

		AlgorithmParameters& aps = commandline.aps;
		aps.ejectLSMaxIter = IntInf;

		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);

		commandline.displayInfo();
		input.displayInfo();
		aps.displayInfo();

		// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
		aps.repairByCusCnt(input.custCnt);

		YearTable yearTable(&input);

		BKS bks(&input, &yearTable, &timer);

		hust::Goal goal(&input, &aps, &bks, &yearTable);

		goal.experimentRouteNumberMinimization();

		return bks.bestSolFound.saveToOutPut().rts;
	}

	std::vector<std::vector<int>> callSolverWithCustomizedInstance(
		const Input::DataModel& dm,
		CommandLine::SolverGoal solverGoal
	) {

		using namespace hust;

		static std::vector< std::string>  argvStrings = {
				"./DLLSMA","-instancePath", "customizedInstance",
		};
		int argc = argvStrings.size();

		char** argv = new char* [argc];
		for (int i = 0; i < argc; ++i) {
			int strSize = argvStrings[i].size();
			argv[i] = new char[strSize + 1];
			strcpy(argv[i], argvStrings[i].c_str());
		}

		hust::CommandLine commandline(argc, argv);

		commandline.solverGoal = solverGoal;

		Timer timer(commandline.runTimer);
		AlgorithmParameters& aps = commandline.aps;
		aps.ejectLSMaxIter = IntInf;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer, dm);

		commandline.displayInfo();
		input.displayInfo();
		aps.displayInfo();

		// TODO[lyh][0]:一定要记得aps用cusCnt合法化一下
		aps.repairByCusCnt(input.custCnt);

		YearTable yearTable(&input);

		BKS bks(&input, &yearTable, &timer);

		hust::Goal goal(&input, &aps, &bks, &yearTable);

		goal.experimentRouteNumberMinimization();

		if (commandline.solverGoal == hust::CommandLine::SolverGoal::DynamicRoutesDistance) {
			return hust::dynamicRun(commandline);
		}
		else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesDistance) {
			return hust::dimacsRun(commandline);
		}
		else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumber) {
			return hust::routeNumberMinimization(commandline);
		}
		else if (commandline.solverGoal == hust::CommandLine::SolverGoal::StaticRoutesNumberBound) {
			return { hust::getOneBound(commandline) };
		}
		else {
			hust::Logger::ERROR("unknow solver goal of running");
		}
		return {};
	}

}