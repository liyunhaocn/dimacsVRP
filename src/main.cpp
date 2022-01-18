// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <numeric>

#include<cstring>
#include<cmath>
#include<iostream>
#include<vector>

#include "Goal.h"
#include "Utility.h"

namespace hust {

bool allocGlobalMem(int argc, char* argv[]) {
//bool allocGlobalMem(std::string inpath) {

	// ../Instances/Homberger/RC2_4_8.txt
	// ../Instances/Solomon/R103.txt
	// ../Instances/Solomon/RC201.txt
	// ../Instances/Homberger/R1_10_10.txt
	// ../Instances/Homberger/R1_6_8.txt
	// ../Instances/Homberger/R1_2_2.txt
	// ../Instances/Homberger/RC2_6_4.txt
	// ../Instances/Homberger/RC2_10_5.txt
	//../Instances/Solomon/R204.txt
	//../Instances/Solomon/R210.txt
	//../Instances/Homberger/R2_2_4.txt 
	//../Instances/Homberger/RC1_8_1.txt 
	//../Instances/Homberger/C1_10_6.txt 
	//../Instances/Homberger/C2_10_6.txt 
	//../Instances/Homberger/RC1_8_5.txt
	//../Instances/Homberger/RC2_10_1.txt

	globalCfg = new hust::Configuration();

	globalCfg->solveCommandLine(argc, argv);

	globalCfg->seed = std::time(nullptr) + std::clock();

	//globalCfg->seed = 1642148308;

	myRand = new Random(globalCfg->seed);
	myRandX = new RandomX(globalCfg->seed);

	globalInput = new Input();

	globalCfg->addHelpInfo();

	globalCfg->show();

	// TODO[lyh][0]:一定要记得globalCfg用cusCnt合法化一下
	globalCfg->repairByCusCnt(globalInput->custCnt);

	yearTable = new Vec<Vec<int>>
		(globalInput->custCnt + 1, Vec<int>(globalInput->custCnt + 1, 0));

	bks = new BKS();
	gloalTimer = new Timer(globalCfg->runTimer);

	return true;
}

bool deallocGlobalMem() {

	delete myRand;
	delete myRandX;
	delete yearTable;
	delete globalCfg;
	delete globalInput;
	delete bks;
	delete gloalTimer;

	return true;
}

}//namespace hust

#if 0
				  
std::string Trim(std::string& str)
{
	//str.find_first_not_of(" \t\r\n"),在字符串str中从索引0开始，返回首次不匹配"\t\r\n"的位置
	str.erase(0, str.find_first_not_of(" \t\r\n"));
	str.erase(str.find_last_not_of(" \t\r\n") + 1);
	return str;
}

#define debug(x) std::cout<<#x<<": "<<(x)<<std::endl;

void verCSVSol() {
	
	std::ifstream fin("./ver1.csv"); //打开文件流操作
	std::string line;

	int i = 0;
	while (getline(fin, line))   //整行读取，换行符“\n”区分，遇到文件尾标志eof终止读取
	{
		//std::cout << "原始字符串：" << line << std::endl; //整行输出
		std::istringstream sin(line); //将整行字符串line读入到字符串流istringstream中
		std::vector<std::string> fields; //声明一个字符串向量
		std::string field;
		while (std::getline(sin, field, ',')) //将字符串流sin中的字符读入到field字符串中，以逗号为分隔符
		{
			fields.push_back(field); //将刚刚读取的字符串添加到向量fields中
		}

		++i;
		if (i == 1) {
			continue;
		}

		std::string  ins = fields[0];
		std::string  lyhrl = fields[2];
		std::string  rts = fields[14];

		hust::MyString ms;

		auto arr = ms.split(rts,"|");

		std::vector<std::vector<int>> rt2;
		for (auto r : arr) {
			if (r.find(":") != std::string::npos) {

				std::vector<int> rt1;
				int pos = r.find(":");
				r = r.substr(pos+1);
				//debug(r);
				std::stringstream ss;
				ss << r;
				int c=-1;
				while (ss>>c)
				{
					rt1.push_back(c);
				}
				rt2.push_back(rt1);
			}
		}

		int cuscnt = 0;
		for (auto rt1 : rt2) {
			cuscnt += rt1.size();
			for (int c : rt1) {
				std::cout << c << " ";
			}
			std::cout << std::endl;
		}
		debug(cuscnt);
		
		std::string Homberger = "../Instances/Homberger/"; //RC2_4_8.txt
		std::string Solomon = "../Instances/Solomon/"; // R204.txt

		std::string path = "";
		if (cuscnt==100) {
			path = Solomon + ins + ".txt";
		}
		else {
			path = Homberger + ins + ".txt";
		}

		char* argv[2] = { const_cast < char*>(""),const_cast<char*>(path.c_str()) };

		hust::allocGlobalMem(2, argv);
		hust::Goal goal;

		hust::Solver s;
		s.initByArr2(rt2);

		debug(ins);
		debug(lyhrl);

		hust::DisType myrl =  std::stoi(lyhrl, nullptr, 10);

		if (myrl != s.verify()) {
			debug(ins);
			debug(s.verify());
			debug(myrl);
			debug(myrl);

		}

		hust::deallocGlobalMem();

	}
}
#endif // 0

int main(int argc, char* argv[])
{
	//verCSVSol();
	//return 0;

	//hust::INFO(sizeof(std::mt19937));
	//hust::INFO(sizeof(hust::Solver));
	//hust::INFO(sizeof(hust::Timer));
	//hust::INFO(sizeof(hust::Solver::alpha));
	//hust::INFO(sizeof(hust::Solver::input));
	//hust::INFO(sizeof(hust::Solver::globalCfg));
	//hust::INFO(sizeof(hust::Input));

	hust::allocGlobalMem(argc,argv);
	hust::Goal goal;

	//goal.callSimulatedannealing();
	goal.TwoAlgCombine();
	//goal.test();

	hust::deallocGlobalMem();

	return 0;
}