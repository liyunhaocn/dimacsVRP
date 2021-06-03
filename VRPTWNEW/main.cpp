// dimacsVRP.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。

#include <iostream>
#include <functional>
//#include "./VRPTW/Environment.h"
//#include "./VRPTW/Utility.h"
//#include "./VRPTW/Solver.h"
//#include "./VRPTW/Problem.h"
//#include "./VRPTW/Configuration.h"

#include "./Environment.h"
#include "./Utility.h"
#include "./Solver.h"
#include "./Problem.h"
#include "./Configuration.h"
#include "./Flag.h"
#include "./EAX.h"

namespace vrpSln = vrptwNew;
//namespace vrpSln = lyh;
using namespace std;


bool run(int argc, char* argv[]) {

	vrpSln::Environment env("C1_6_6");
	vrpSln::Configuration cfg;
	//cfg.breakRecord = 1;
	solveCommandLine(argc, argv, cfg, env);

	vrpSln::DateTime d(time(0));
	cout << d << endl;
	deOut("compile tag:")debug("0602-1")
	debug(argc)

	
	//env.seed = 1610679669;
	//env.seed = 1610863258;
	
	vector<string> e1 = {"C1_", "C2_" ,"R1_","R2_","RC1_","RC2_"};
	vector<string> e2 = { "2_","4_","6_","8_","10_" };
	vector<string> e3 = { "11","12","13","14","15","16","17","18","19","20" };

	/*vector<string> allCase;
	for (string a : e1) {
	for (string b : e2) {
	for (string c : e3) {
	allCase.push_back(a + b + c);*/
	//cfg.breakRecord = 1;

	vrpSln::Input input(env, cfg);
	vrpSln::Solver solver(input, cfg, env);
	
	//solver.gamma = 0;
	solver.minimizeRN();
	//solver.minimizeRL();
	//solver.testRoute();
	

	vrpSln::saveSln(input, solver.output, cfg, env);
	/*}
	}
	}*/
	
	return true;
}

bool solverByEAX(int argc, char* argv[]) {

	vrpSln::Environment env;
	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	vrpSln::DateTime d(time(0));
	cout << d << endl;
	//deOut("compile tag:")debug("0424-2")
	debug(argc)

	solveCommandLine(argc, argv, cfg, env);

	//env.seed = 1611589828;
	//cfg.breakRecord = 1;

	vrpSln::Input input(env, cfg);

	vrpSln::Solver pa(input, cfg, env);
	vrpSln::Solver pb(input, cfg, env);
	
	/*pa.initBybestSolution();
	pa.patternAdjustment(-1,1000);
	pb.initBybestSolution();*/
	
	//pa.minimizeRN();
	//pa.updateRtsCost();
	////pa.initBybestSolution();
	//debug(pa.RoutesCost)
	//pa.patternAdjustment(-1,1000);
	//pa.minimizeRL();
	//debug(pa.RoutesCost)
	//debug(pa.verify())
	//debug(pa.verify())

	pb.minimizeRN();
	//pb.patternAdjustment(-1, 1000);
	pb.minimizeRL();
	debug(pb.RoutesCost)

	return true;

	vrpSln::Solver pc(pa);
	vrpSln::Solver pBest(pa);
	int contiNoDown = 0;

	while (pa.RoutesCost > input.sintefRecRL){

		pc = pa;
		vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
		bool eaSucceed = eax.doEAX(pa, pb, pc);

		while (!eaSucceed) {
			debug(eaSucceed)
			pb.patternAdjustment(-1, 100);
			pb.minimizeRL();
			pc = pa;
			vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
			eaSucceed = eax.doEAX(pa, pb, pc);
		}

		bool reSucceed = pc.repair();

		while(!reSucceed) {

			debug(reSucceed)
			pb.patternAdjustment(-1, 500);
			pb.minimizeRL();
			pc = pa;
			vrpSln::EAX eax(input.custCnt, pa.rts.cnt, env.seed);
			eax.doEAX(pa, pb, pc);
			reSucceed = pc.repair();
		}

		pc.minimizeRL();

		if (pc.RoutesCost < pa.RoutesCost) {
			pa = pc;
			contiNoDown = 0;

			//pc.saveToOutPut();
			//vrpSln::saveSln(input, pc.output, cfg, env);
			//debug(pc.RoutesCost)
		}
		
		if (pc.RoutesCost < pBest.RoutesCost) {
			pBest = pc;
			contiNoDown = 0;
			pBest.saveToOutPut();
			vrpSln::saveSln(input, pBest.output, cfg, env);
			debug(pc.RoutesCost)
		}
		else {
			++contiNoDown;
		}

		if (contiNoDown > 5) {
			pa = pBest;
			pb.patternAdjustment(-1, 5000);
			pb.minimizeRL();
			
			contiNoDown = 0;
		}
	}
	
	vrpSln::saveSln(input, pa.output, cfg, env);

	return true;
}

#if MAKECASE

bool makeOneCase(string exName, vrpSln::Input& input, int minDe, int maxDe, int minTw, int maxTw, int minSt, int maxSt) {

	vrpSln::Random mr;

	struct Pos :public vrpSln::Data
	{
		bool operator < (const Pos& g)const
		{//define operator for custom key
			if (XCOORD != g.XCOORD)
				return XCOORD < g.XCOORD;
			else
				return YCOORD < g.YCOORD;
		}
	};

	set<Pos> setPos;

	Pos deopt;
	deopt.XCOORD = input.datas[0].XCOORD;
	deopt.YCOORD = input.datas[0].YCOORD;
	deopt.CUSTNO = input.datas[0].CUSTNO;
	deopt.DEMAND = input.datas[0].DEMAND;
	deopt.READYTIME = input.datas[0].READYTIME;
	deopt.DUEDATE = input.datas[0].DUEDATE;
	deopt.SERVICETIME = input.datas[0].SERVICETIME;

	setPos.insert(deopt);
	vector<Pos> cusVe;

	int cluSize = input.custCnt / 20;
	for (int i = 0; i < cluSize; ++i) {
		Pos p1;
		//pick from[min, max).
		p1.XCOORD = mr.pick(0, deopt.XCOORD * 2);
		p1.YCOORD = mr.pick(0, deopt.YCOORD * 2);
		while (setPos.count(p1) > 0) {
			p1.XCOORD = mr.pick(0, deopt.XCOORD * 2);
			p1.YCOORD = mr.pick(0, deopt.YCOORD * 2);
		}
		cusVe.push_back(p1);
		setPos.insert(p1);
	}

	int areaRec = deopt.XCOORD * 2 * deopt.YCOORD * 2;
	int eachCluArea = areaRec / cluSize;
	int eachCluR = sqrtl(eachCluArea / vrpSln::PI);

	auto calDisOf = [=](Pos a, Pos b) {
		return sqrt((a.XCOORD - b.XCOORD) * (a.XCOORD - b.XCOORD) + (a.YCOORD - b.YCOORD) * (a.YCOORD - b.YCOORD));
	};

	while (cusVe.size() < input.custCnt/2) {

		Pos p1;

		p1.XCOORD = mr.pick(0, deopt.XCOORD * 2);
		p1.YCOORD = mr.pick(0, deopt.YCOORD * 2);

		while (setPos.count(p1) > 0) {
			p1.XCOORD = mr.pick(0, deopt.XCOORD * 2);
			p1.YCOORD = mr.pick(0, deopt.YCOORD * 2);
		}

		cusVe.push_back(p1);
		setPos.insert(p1);
	}

	while (cusVe.size() < input.custCnt) {

		int cluIndex = mr.pick(cluSize);
		Pos choseClu = cusVe[cluIndex];

		Pos p1;
		//pick from[min, max).
		p1.XCOORD = mr.pick(max(0, choseClu.XCOORD - eachCluR *0.5), min(choseClu.XCOORD + eachCluR * 0.5, deopt.XCOORD * 2));
		p1.YCOORD = mr.pick(max(0, choseClu.YCOORD - eachCluR * 0.5), min(choseClu.YCOORD + eachCluR * 0.5, deopt.YCOORD * 2));
		
		while (calDisOf(p1, choseClu) > eachCluR || setPos.count(p1) > 0) {
			p1.YCOORD = mr.pick(max(0, choseClu.YCOORD - eachCluR * 0.5), min(deopt.YCOORD * 2,choseClu.YCOORD + eachCluR * 0.5));
		}
		
		cusVe.push_back(p1);
		setPos.insert(p1);
	}


	for (int i = 0; i < input.custCnt; ++i) {

		Pos& p = cusVe[i];
		p.CUSTNO = i+1;
		p.DEMAND = mr.pick(minDe / 10, maxDe / 10 + 1) * 10;

		p.SERVICETIME = mr.pick(minSt / 10, maxSt / 10 + 1) * 10;

		int tw = mr.pick(minTw, maxTw + 1);
		int maxDue = deopt.DUEDATE - calDisOf(deopt, p) - p.SERVICETIME;
		int minDue = calDisOf(deopt, p) + tw;

		if (maxDue < 0) {
			debug(minDue)
				debug(maxDue)
				debug(calDisOf(deopt, p))
				debug(p.SERVICETIME)
				debug(deopt.DUEDATE)
				debug(deopt.DUEDATE)
				debug(minSt)
				debug(maxSt)
				debug(p.XCOORD)
				debug(p.YCOORD)
				debug(deopt.XCOORD)
				debug(deopt.YCOORD)
				debug(i)
				debug(maxSt)
		}

		while (minDue >= maxDue) {
			tw = mr.pick(minTw, maxTw + 1);
			//maxDue = deopt.DUEDATE - calDisOf(deopt, p) - p.SERVICETIME;
			minDue = calDisOf(deopt, p) + tw;
			/*debug(minDue)
				debug(maxDue)*/
		}

		p.DUEDATE = mr.pick(minDue, maxDue + 1);
		p.READYTIME = p.DUEDATE - tw;

	}

	stringstream ss;

	int capacity = input.Q;
	int vehCnt = input.vehicleCnt;

	ss << exName;
	ss << "\n\n";
	ss << "VEHICLE\n";
	ss << "NUMBER     CAPACITY\n";
	ss << vehCnt;
	ss << "\t";
	ss << capacity;
	ss<<"\n";
	ss << "\n";
	ss << "CUSTOMER\n";
	ss << "CUST NO.\tXCOORD.\tYCOORD.\tDEMAND\tREADY TIME\tDUE DATE\tSERVICE TIME\n";
	/*const type_info& nInfo = typeid(calDisOf(otherCus[0], otherCus[1]));
	cout << nInfo.name() << " | " << nInfo.raw_name() << " | " << nInfo.hash_code() << endl;*/
	ss << "\n";

	Pos& p = deopt;
	ss << p.CUSTNO;
	ss << "\t\t";
	ss << p.XCOORD;
	ss << "\t\t";
	ss << p.YCOORD;
	ss << "\t\t";
	ss << p.DEMAND;
	ss << "\t\t";
	ss << p.READYTIME;
	ss << "\t\t";
	ss << p.DUEDATE;
	ss << "\t\t";
	ss << p.SERVICETIME;
	ss << "\t\t";
	ss << "\n";

	for (Pos& p : cusVe) {
		ss << p.CUSTNO;
		ss << "\t\t";
		ss << p.XCOORD;
		ss << "\t\t";
		ss << p.YCOORD;
		ss << "\t\t";
		ss << p.DEMAND;
		ss << "\t\t";
		ss << p.READYTIME;
		ss << "\t\t";
		ss << p.DUEDATE;
		ss << "\t\t";
		ss << p.SERVICETIME;
		ss << "\t\t";
		ss << "\n";
	}

	std::ofstream fout;

	vrpSln::MyString ms;
	vector<string> sp = ms.split(exName, "_");
	debug(sp[0])
		debug(sp[1])
		debug(sp[2])
		
	string basePath = "./hustlyh_" + sp[1] + "00"+ "_customer_instances/";
	fout.open(basePath + exName + ".TXT", std::ios::out | std::ios::app);

	fout << ss.str();
	debug(ss.str())
	ss.clear();
	//ss.str("");
	fout.close();

	return true;
}
    
bool makeCases(int argc, char* argv[]) {

	vrpSln::Environment env;
	vrpSln::Configuration cfg;
	//lyh::MyString ms;

	vrpSln::DateTime d(time(0));
	cout << d << endl;
	deOut("compile tag:")debug("0424-2")
	debug(argc)

	solveCommandLine(argc, argv, cfg, env);
	env.seed = 1608742675;

	//vector<string> e1 = {/*"C1_", "C2_" ,"R1_","R2_",*/};
	vector<string> e1 = {"RC1_",};

	vector<string> e2 = {"2_","4_","6_","8_","10_"};
	vector<string> e3 = {"1"/*,"2","3","4","5","6","7","8","9","10"*/};
	
	vector<string> allCase;
	for (string a : e1) {
		for (string b : e2) {
			for (string c : e3) {
				allCase.push_back(a + b + c);
			}
		}
	}

	for (string ex : allCase) {

		vrpSln::Input input(env.example != "" ? env.example : ex, cfg);

		int maxTw = -1;
		int minTw = vrpSln::IntInf;
		int avgTw = 0;
		int maxDe = -1;
		int minDe = vrpSln::IntInf;
		int avgDe = 0;

		int maxSt = -1;
		int minSt = vrpSln::IntInf;
		int avgSt = 0;


		for (int i = 1; i <= input.custCnt; ++i) {
			avgDe += input.datas[i].DEMAND;
			maxDe = max(maxDe, input.datas[i].DEMAND);
			minDe = min(minDe, input.datas[i].DEMAND);

			avgTw += input.datas[i].DUEDATE - input.datas[i].READYTIME;
			maxTw = max(maxTw, input.datas[i].DUEDATE - input.datas[i].READYTIME);
			minTw = min(minTw, input.datas[i].DUEDATE - input.datas[i].READYTIME);

			avgSt += input.datas[i].SERVICETIME;
			maxSt = max(maxSt, input.datas[i].SERVICETIME);
			minSt = min(minSt, input.datas[i].SERVICETIME);

		}
		avgDe /= input.custCnt;
		avgTw /= input.custCnt;
		avgSt /= input.custCnt;

		/*debug(minDe)
		debug(maxDe)
		debug(avgDe)

		debug(minTw)
		debug(maxTw)
		debug(avgTw)

		debug(minSt)
		debug(maxSt)
		debug(avgSt)*/

		vrpSln::MyString ms;
		vector<string> sp = ms.split(ex,"_");
		debug(sp[0])
		debug(sp[1])
		debug(sp[2])
		for (int ca = 11; ca <= 20; ++ca) {
			makeOneCase(sp[0]+"_"+ sp[1]+"_" + ms.LL_str(ca) ,input, minDe, maxDe, minTw, maxTw,minSt,maxSt);
		}
	}
	
	return true;
}

#endif // MAKECASE

int main(int argc, char* argv[])
{

	//solverByEAX(argc, argv);
	//makeCases(argc, argv);
	//for(;;)
	run(argc, argv);
	
	return 0;
}
