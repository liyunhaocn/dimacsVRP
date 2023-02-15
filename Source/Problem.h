
#ifndef CN_HUST_LYH_PROBLEM_H
#define CN_HUST_LYH_PROBLEM_H

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>
#include <functional>

#include "Algorithm_Parameters.h"
#include "Flag.h"
#include "Util_Arr2D.h"
#include "CommandLine.h"

namespace hust {

struct CircleSector
{
	int start = 0;
	int end = 0;

	// Positive modulo 65536
	static int positive_mod(int i) {
		// 1) Using the formula positive_mod(n,x) = (n % x + x) % x
		// 2) Moreover, remark that "n % 65536" should be automatically compiled in an optimized form as "n & 0xffff" for faster calculations
		return (i % 65536 + 65536) % 65536;
	}

	// Initialize a circle sector from a single point
	void initialize(int point) {
		start = point;
		end = point;
	}

	// Tests if a point is enclosed in the circle sector
	bool isEnclosed(int point) {
		return (positive_mod(point - start) <= positive_mod(end - start));
	}

	// Tests overlap of two circle sectors
	static bool overlap(const CircleSector& sector1, const CircleSector& sector2) {
		return ((positive_mod(sector2.start - sector1.start) <= positive_mod(sector1.end - sector1.start))
			|| (positive_mod(sector1.start - sector2.start) <= positive_mod(sector2.end - sector2.start)));
	}

	// Extends the circle sector to include an additional point 
	// Done in a "greedy" way, such that the resulting circle sector is the smallest
	void extend(int point) {
		if (!isEnclosed(point)) {
			if (positive_mod(point - end) <= positive_mod(start - point))
				end = point;
			else
				start = point;
		}
	}
	
	static int disofpointandsec(int point,CircleSector& sec) {
		if (!sec.isEnclosed(point)) {
			if (positive_mod(point - sec.end) <= positive_mod(sec.start - point))
				return positive_mod(point - sec.end);
			else
				return positive_mod(sec.start - point);
		}
		else {
			return 0;
		}
		return 65536;
	}

};

struct Data {

	int CUSTNO = -1;
	DisType XCOORD = -1;
	DisType YCOORD = -1;
	DisType DEMAND = -1;
	DisType READYTIME = -1;
	DisType DUEDATE = -1;
	DisType SERVICETIME = -1;
	bool must_dispatch = true;
	int polarAngle = 0;
};

struct Customer {
public:

	//int id = -1;
	int pre = -1;
	int next = -1;
	int routeID = -1;

	DisType av = 0;
	DisType zv = 0;

	DisType avp = 0;
	DisType zvp = 0;

	DisType TW_X = 0;
	DisType TWX_ = 0;

	DisType Q_X = 0;
	DisType QX_ = 0;

	Customer() :pre(-1), next(-1), av(0), zv(0), avp(0),
		zvp(0), TW_X(0), TWX_(0), Q_X(0), QX_(0) {}

	bool reset() {
		//id = -1;
		pre = -1;
		next = -1;
		routeID = -1;

		av = 0;
		zv = 0;

		avp = 0;
		zvp = 0;

		TW_X = 0;
		TWX_ = 0;

		Q_X = 0;
		QX_ = 0;

		return true;
	}
};

struct Input {

	std::string instanceName = "";
	int custCnt = 0;
	DisType Q = 0;
	int vehicleCnt = 0;
	Vec<Data> datas;
	int Qbound = -1;
	
	CommandLine* commandLine;
	AlgorithmParameters* aps;
	RandomTools* randomTools;
	Timer* timer;
	int mustDispatchNumber = -1;
	bool isExplicitDistanceMatrix = false;
	int totalDemand;													// Total demand required by the clients
	int maxDemand;														// Maximum demand of a client
	int maxDist;														// Maximum distance between two clients
	bool isDurationConstraint;											// Indicates if the problem includes duration constraints
	bool isTimeWindowConstraint;										// Indicates if the problem includes time window constraints
	int durationLimit;													// Route duration limit
	util::Array2D<DisType> disOf;

	//// disOf[v][w] 表示w和v之间的距离
	//Vec<Vec<int>> allCloseOf;
	//// input.allCloseOf[v][wpos] 表示v的地理位置第wpos近的点
	util::Array2D<int> addSTclose;

	// input.addSTclose[v][wpos] 表示v的地理位置加上v的服务时间第wpos近的点
	util::Array2D<int> addSTJIsxthcloseOf;

	//表示v的地理位置加上v的服务时间作为排序依据 input.addSTJIsxthcloseOf[v][w],w是v的第几近
	Vec< Vec<int> > iInNeicloseOfUnionNeiCloseOfI;

	util::Array2D<int> sectorClose;

	Vec<int> P;

	Input(CommandLine* commandLine,AlgorithmParameters* aps, RandomTools* randomTools,Timer*timer);

	bool initInput();

	bool readDimacsInstance(const std::string& instanciaPath);

	bool readDynamicInstance(const std::string& instanciaPath);
	
	void initDetail();

	int partition(int* arr, int start, int end, std::function<bool(int, int)>cmp);

	void getTopKmin(int* input,int n, int k, std::function<bool(int, int)> cmp);

	inline DisType getDisof2(int a, int b) {

		auto reCusNo = [=](int x) -> int {
			return x <= custCnt ? x : 0;
		};
		a = reCusNo(a);
		b = reCusNo(b);

		return disOf.at(a,b);
	}

	void displayInfo() {

		Logger::INFO("instanceName:", instanceName);
		Logger::INFO("custCnt:", custCnt);
		Logger::INFO("Q:", Q);
		Logger::INFO("vehicleCnt:", vehicleCnt);
	}
};

struct Output
{
	Vec<Vec<int>> rts;
	Vec<int> EP;
	DisType PtwNoWei = -1;
	DisType Pc =-1;
	int minEP = -1;
	DisType state = -1;
	double runTime = 0.0;

};


}


#endif // !CN_HUST_LYH_PROBLEM_H

