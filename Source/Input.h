
#ifndef CN_HUST_LYH_PROBLEM_H
#define CN_HUST_LYH_PROBLEM_H

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>
#include <functional>

#include "AlgorithmParameters.h"
#include "Common.h"
#include "Arr2D.h"
#include "CommandLine.h"
#include "Util.h"

namespace hust {

// Simple data structure to represent circle sectors
// Angles are measured in [0,65535] instead of [0,359], in such a way that modulo operations are much faster (since 2^16 = 65536)
// Credit to Fabian Giesen at "https://web.archive.org/web/20200912191950/https://fgiesen.wordpress.com/2015/09/24/intervals-in-modular-arithmetic/" for useful implementation tips regarding interval overlaps in modular arithmetics 

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
	int polarAngle = 0;
};

struct Input {

	String instanceName = "";
	int customerNumer = 0;
	DisType vehicleCapacity = 0;
	int vehicleNumber = 0;
	Vector<Data> datas;

	int Qbound = -1;
	CommandLine* commandLine;
	AlgorithmParameters* aps;

	Random random;
	RandomX randomx;
	Timer timer;
	
	util::Array2D<DisType> disOf;

	//// disOf[v][w] 表示w和v之间的距离
	//Vector<Vector<int>> allCloseOf;
	//// input.allCloseOf[v][wpos] 表示v的地理位置第wpos近的点
	util::Array2D<int> addSTclose;

	// input.addSTclose[v][wpos] 表示v的地理位置加上v的服务时间第wpos近的点
	util::Array2D<int> addSTJIsxthcloseOf;

	//表示v的地理位置加上v的服务时间作为排序依据 input.addSTJIsxthcloseOf[v][w],w是v的第几近
	Vector< Vector<int> > iInNeicloseOfUnionNeiCloseOfI;

	util::Array2D<int> sectorClose;

	Vector<int> P;

	Input(CommandLine* commandLine);
	
	void initInput();

	void readInstanceFormatCVRPLIB();

	void initDetail();

	int partition(int* arr, int start, int end, std::function<bool(int, int)>cmp);

	void getTopKmin(int* input,int n, int k, std::function<bool(int, int)> cmp);

	inline DisType getDisof2(int a, int b) {

		auto reCusNo = [=](int x) -> int {
			return x <= customerNumer ? x : 0;
		};
		a = reCusNo(a);
		b = reCusNo(b);

		return disOf.at(a,b);
	}

	void displayInfo() {

		Logger::INFO("instanceName:", instanceName);
		Logger::INFO("customerNumer:", customerNumer);
		Logger::INFO("vehicleCapacity:", vehicleCapacity);
		Logger::INFO("vehicleNumber:", vehicleNumber);
	}
};

}


#endif // !CN_HUST_LYH_PROBLEM_H

