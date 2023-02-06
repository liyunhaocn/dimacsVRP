#include "pch.h"


TEST(EAXTest, Temp) {

	using namespace hust;

	std::vector< std::vector< std::string> > argvs = lyhtest::getTestCommandLineArgv();

	for (auto& argvItem : argvs) {

		int argc = argvItem.size();

		char** argv = new char* [argc];
		for (int i = 0; i < argc; ++i) {
			argv[i] = const_cast<char*>(argvItem[i].c_str());
		}
		CommandLine commandline(argc, argv);
		Timer timer(commandline.runTimer);

		AlgorithmParameters aps = commandline.aps;
		RandomTools randomTools(commandline.seed);

		Input input(&commandline, &aps, &randomTools, &timer);
		YearTable yearTable(&input);
		BKS bks(&input, &yearTable, &timer);

		Solver pa(&input, &yearTable, &bks);
		pa.initSolution(0);

		Solver pb(&input, &yearTable, &bks);
		pb.initSolution(0);

		EAX eax(pa, pb);

		EXPECT_EQ(eax.pa->rts.cnt, eax.pb->rts.cnt);
		
		eax.generateCycles();

	}
}