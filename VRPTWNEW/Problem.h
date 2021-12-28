#ifndef vrptwNew_PROBLEM_H
#define vrptwNew_PROBLEM_H

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>

#include "Configuration.h"
#include "Environment.h"
#include "Flag.h"

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
};

struct Data {

	LL CUSTNO = -1;
	LL XCOORD = -1;
	LL YCOORD = -1;
	LL DEMAND = -1;
	LL READYTIME = -1;
	LL DUEDATE = -1;
	LL SERVICETIME = -1;
	LL polarAngle = 0;
};

struct InsData {
	int minRN = 0;
	double minRL = 0;
	int isOpt = 0;
	InsData() :minRN(0), minRL(0), isOpt(0) {}
	InsData(int rn, double rl, int is) :minRN(rn), minRL(rl), isOpt(is) {}
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

	bool reSet() {
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

static InsData getInsData(std::string& ins);

static double getSintefRL(std::string& ins);

static double getNagataRL(std::string ins);

bool solveCommandLine(int argc, char* argv[]);

struct Input {

	std::string example = "";
	int custCnt = 0;
	DisType Q = 0;
	int vehicleCnt = 0;
	Vec<Data> datas;
	int sintefRecRN = 0;
	DisType dimacsRecRL = 0;
	DisType dimacsRecRN = 0;
	DisType sintefRecRL = 0;
	//  TODO[lyh][0]:提交之前要去掉
	DisType naRecRL = 0;
	bool isOptRL = false;

	Vec<Vec<DisType>> disOf;

	// disOf[v][w] 表示w和v之间的距离
	Vec<Vec<int>> allCloseOf;
	// input.allCloseOf[v][wpos] 表示v的地理位置第wpos近的点
	Vec<Vec<int>> addSTclose;
	// input.addSTclose[v][wpos] 表示v的地理位置加上v的服务时间第wpos近的点
	Vec<Vec<int>> addSTJIsxthcloseOf;
	//表示v的地理位置加上v的服务时间作为排序依据 input.addSTJIsxthcloseOf[v][w],w是v的第几近

	Vec<Vec<int>> iInNeicloseOfUnionNeiCloseOfI;

	Vec<Vec<int>> sectorClose;

	Vec<int> P;

	Input();

	bool initInput();

	bool readDimacsInstance(std::string& instanciaPath);

	bool readDimacsBKS();

};

struct Output
{
	Vec<Vec<int>> rts;
	Vec<int> EP;
	LL PtwNoWei = -1;
	LL Pc =-1;
	int minEP = -1;
	DisType state = -1;
	double runTime = 0.0;

};

bool saveSlnFile(Input& input, Output& output);

}


#endif // !vrptwNew_PROBLEM_H

