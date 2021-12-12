#ifndef vrptwNew_PROBLEM_H
#define vrptwNew_PROBLEM_H

using namespace std;

#include <algorithm>
#include <fstream>
#include <map>
#include <string>
#include <cstdio>

#include "./Configuration.h"
#include "./Environment.h"
#include "./Flag.h"

namespace hust {

struct CircleSector
{
	int start = -1;
	int end = -1;

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
	LL polarAngle = -1;
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

	int id = -1;
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

static InsData getInsData(string& ins) {
	static unordered_map<string, InsData> dataBase = {
{"C101", {10,827.3,1}},
{"C102", {10,827.3,1}},
{"C103", {10,826.3,1}},
{"C104", {10,822.9,1}},
{"C105", {10,827.3,1}},
{"C106", {10,827.3,1}},
{"C107", {10,827.3,1}},
{"C108", {10,827.3,1}},
{"C109", {10,827.3,1}},
{"C201", {3,589.1,1}},
{"C202", {3,589.1,1}},
{"C203", {3,588.7,1}},
{"C204", {3,588.1,1}},
{"C205", {3,586.4,1}},
{"C206", {3,586.0,1}},
{"C207", {3,585.8,1}},
{"C208", {3,585.8,1}},
{"R101", {19,1637.7,1}},
{"R102", {17,1466.6,1}},
{"R103", {13,1208.7,1}},
{"R104", {9,971.5,1}},
{"R105", {14,1355.3,1}},
{"R106", {12,1234.6,1}},
{"R107", {10,1064.6,1}},
{"R108", {9,932.1,1}},
{"R109", {11,1146.9,1}},
{"R110", {10,1068.0,1}},
{"R111", {10,1048.7,1}},
{"R112", {9,948.9,1}},
{"R201", {4,1143.2,1}},
{"R202", {3,1029.6,1}},
{"R203", {3,870.8,1}},
{"R204", {2,731.3,1}},
{"R205", {3,949.8,1}},
{"R206", {3,875.9,1}},
{"R207", {2,794.0,1}},
{"R208", {2,701.0,1}},
{"R209", {3,854.8,1}},
{"R210", {3,900.5,1}},
{"R211", {2,746.7,1}},
{"RC101", {14,1619.8,1}},
{"RC102", {12,1457.4,1}},
{"RC103", {11,1258.0,1}},
{"RC104", {10,1132.3,1}},
{"RC105", {13,1513.7,1}},
{"RC106", {11,1372.7,1}},
{"RC107", {11,1207.8,1}},
{"RC108", {10,1114.2,1}},
{"RC201", {4,1261.8,1}},
{"RC202", {3,1092.3,1}},
{"RC203", {3,923.7,1}},
{"RC204", {3,783.5,1}},
{"RC205", {4,1154.0,1}},
{"RC206", {3,1051.1,1}},
{"RC207", {3,962.9,1}},
{"RC208", {3,776.1,1}},
{"C1_2_10", {18,2624.7,1}},
{"C1_2_1", {20,2698.6,1}},
{"C1_2_2", {18,2694.3,1}},
{"C1_2_3", {18,2675.8,1}},
{"C1_2_4", {18,2625.6,1}},
{"C1_2_5", {20,2694.9,1}},
{"C1_2_6", {20,2694.9,1}},
{"C1_2_7", {20,2694.9,1}},
{"C1_2_8", {19,2684.0,1}},
{"C1_2_9", {18,2639.6,1}},
{"C2_2_10", {6,1791.2,1}},
{"C2_2_1", {6,1922.1,1}},
{"C2_2_2", {6,1851.4,1}},
{"C2_2_3", {6,1763.4,1}},
{"C2_2_4", {6,1695.0,0}},
{"C2_2_5", {6,1869.6,1}},
{"C2_2_6", {6,1844.8,1}},
{"C2_2_7", {6,1842.2,1}},
{"C2_2_8", {6,1813.7,1}},
{"C2_2_9", {6,1815.0,1}},
{"R1_2_10", {18,3293.1,1}},
{"R1_2_1", {20,4667.2,1}},
{"R1_2_2", {18,3919.9,1}},
{"R1_2_3", {18,3373.9,1}},
{"R1_2_4", {18,3047.6,1}},
{"R1_2_5", {18,4053.2,1}},
{"R1_2_6", {18,3559.1,1}},
{"R1_2_7", {18,3141.9,1}},
{"R1_2_8", {18,2938.4,1}},
{"R1_2_9", {18,3734.7,1}},
{"R2_2_10", {4,2549.4,1}},
{"R2_2_1", {4,3468.0,1}},
{"R2_2_2", {4,3008.2,1}},
{"R2_2_3", {4,2537.5,1}},
{"R2_2_4", {4,1928.5,1}},
{"R2_2_5", {4,3061.1,1}},
{"R2_2_6", {4,2675.4,1}},
{"R2_2_7", {4,2304.7,1}},
{"R2_2_8", {4,1842.4,1}},
{"R2_2_9", {4,2843.3,1}},
{"RC1_2_10", {18,2990.5,0}},
{"RC1_2_1", {18,3516.9,1}},
{"RC1_2_2", {18,3221.6,1}},
{"RC1_2_3", {18,3001.4,1}},
{"RC1_2_4", {18,2845.2,1}},
{"RC1_2_5", {18,3325.6,1}},
{"RC1_2_6", {18,3300.7,1}},
{"RC1_2_7", {18,3177.8,1}},
{"RC1_2_8", {18,3060.0,1}},
{"RC1_2_9", {18,3073.3,0}},
{"RC2_2_10", {4,1989.2,0}},
{"RC2_2_1", {6,2797.4,1}},
{"RC2_2_2", {5,2481.6,1}},
{"RC2_2_3", {4,2227.7,1}},
{"RC2_2_4", {4,1854.8,1}},
{"RC2_2_5", {4,2491.4,1}},
{"RC2_2_6", {4,2495.1,1}},
{"RC2_2_7", {4,2287.7,1}},
{"RC2_2_8", {4,2151.2,1}},
{"RC2_2_9", {4,2086.6,1}},
{"C1_4_10", {36,6825.4,1}},
{"C1_4_1", {40,7138.8,1}},
{"C1_4_2", {36,7113.3,1}},
{"C1_4_3", {36,6929.9,1}},
{"C1_4_4", {36,6777.7,1}},
{"C1_4_5", {40,7138.8,1}},
{"C1_4_6", {40,7140.1,1}},
{"C1_4_7", {39,7136.2,1}},
{"C1_4_8", {37,7083.0,1}},
{"C1_4_9", {36,6927.8,1}},
{"C2_4_10", {11,3665.1,1}},
{"C2_4_1", {12,4100.3,1}},
{"C2_4_2", {12,3914.1,1}},
{"C2_4_3", {11,3758.3,0}},
{"C2_4_4", {11,3523.7,0}},
{"C2_4_5", {12,3923.2,1}},
{"C2_4_6", {12,3860.1,1}},
{"C2_4_7", {12,3870.9,1}},
{"C2_4_8", {11,3773.7,1}},
{"C2_4_9", {12,3842.1,0}},
{"R1_4_10", {36,8086.4,0}},
{"R1_4_1", {40,10305.8,1}},
{"R1_4_2", {36,8876.4,0}},
{"R1_4_3", {36,7802.8,0}},
{"R1_4_4", {36,7269.0,0}},
{"R1_4_5", {36,9188.6,0}},
{"R1_4_6", {36,8340.4,1}},
{"R1_4_7", {36,7602.8,0}},
{"R1_4_8", {36,7255.8,0}},
{"R1_4_9", {36,8680.6,0}},
{"R2_4_10", {8,5663.5,0}},
{"R2_4_1", {8,7520.7,1}},
{"R2_4_2", {8,6482.8,1}},
{"R2_4_3", {8,5372.9,1}},
{"R2_4_4", {8,4211.2,0}},
{"R2_4_5", {8,6567.9,0}},
{"R2_4_6", {8,5818.3,0}},
{"R2_4_7", {8,4893.5,0}},
{"R2_4_8", {8,4001.0,0}},
{"R2_4_9", {8,6070.2,0}},
{"RC1_4_10", {36,7586.3,0}},
{"RC1_4_1", {36,8524.0,0}},
{"RC1_4_2", {36,7889.4,0}},
{"RC1_4_3", {36,7516.9,0}},
{"RC1_4_4", {36,7295.3,0}},
{"RC1_4_5", {36,8172.4,0}},
{"RC1_4_6", {36,8148.7,0}},
{"RC1_4_7", {36,7942.3,0}},
{"RC1_4_8", {36,7765.7,0}},
{"RC1_4_9", {36,7723.6,0}},
{"RC2_4_10", {8,4258.1,0}},
{"RC2_4_1", {11,6147.3,1}},
{"RC2_4_2", {9,5407.5,1}},
{"RC2_4_3", {8,4573.0,0}},
{"RC2_4_4", {8,3597.9,0}},
{"RC2_4_5", {8,5396.9,0}},
{"RC2_4_6", {8,5332.0,0}},
{"RC2_4_7", {8,4987.8,0}},
{"RC2_4_8", {8,4706.7,0}},
{"RC2_4_9", {8,4510.4,0}},
{"C1_6_10", {56,13636.5,0}},
{"C1_6_1", {60,14076.6,0}},
{"C1_6_2", {56,13948.3,0}},
{"C1_6_3", {56,13757.0,0}},
{"C1_6_4", {56,13572.2,0}},
{"C1_6_5", {60,14066.8,0}},
{"C1_6_6", {59,14070.9,0}},
{"C1_6_7", {57,14066.8,0}},
{"C1_6_8", {56,13991.2,0}},
{"C1_6_9", {56,13701.5,0}},
{"C2_6_10", {17,7123.9,0}},
{"C2_6_1", {18,7752.2,0}},
{"C2_6_2", {17,7474.9,0}},
{"C2_6_3", {17,7215.0,0}},
{"C2_6_4", {17,6880.7,0}},
{"C2_6_5", {18,7553.8,0}},
{"C2_6_6", {18,7449.8,0}},
{"C2_6_7", {18,7496.7,0}},
{"C2_6_8", {17,7303.7,0}},
{"C2_6_9", {17,7303.2,0}},
{"R1_6_10", {54,17648.1,0}},
{"R1_6_1", {59,21276.5,0}},
{"R1_6_2", {54,18581.5,0}},
{"R1_6_3", {54,16941.8,0}},
{"R1_6_4", {54,15774.6,0}},
{"R1_6_5", {54,19324.8,0}},
{"R1_6_6", {54,17805.4,0}},
{"R1_6_7", {54,16555.1,0}},
{"R1_6_8", {54,15624.3,0}},
{"R1_6_9", {54,18533.1,0}},
{"R2_6_10", {11,11909.5,0}},
{"R2_6_1", {11,15153.0,0}},
{"R2_6_2", {11,12991.9,0}},
{"R2_6_3", {11,10467.2,0}},
{"R2_6_4", {11,7941.7,0}},
{"R2_6_5", {11,13820.6,0}},
{"R2_6_6", {11,11891.3,0}},
{"R2_6_7", {11,9841.2,0}},
{"R2_6_8", {11,7512.3,0}},
{"R2_6_9", {11,12775.9,0}},
{"RC1_6_10", {55,15673.9,0}},
{"RC1_6_1", {55,16985.9,0}},
{"RC1_6_2", {55,15989.9,0}},
{"RC1_6_3", {55,15236.2,0}},
{"RC1_6_4", {55,14778.2,0}},
{"RC1_6_5", {55,16599.8,0}},
{"RC1_6_6", {55,16543.9,0}},
{"RC1_6_7", {55,16092.6,0}},
{"RC1_6_8", {55,15959.8,0}},
{"RC1_6_9", {55,15856.0,0}},
{"RC2_6_10", {11,9014.8,0}},
{"RC2_6_1", {14,11981.0,0}},
{"RC2_6_2", {12,10336.9,0}},
{"RC2_6_3", {11,8907.4,0}},
{"RC2_6_4", {11,7008.1,0}},
{"RC2_6_5", {11,11084.2,0}},
{"RC2_6_6", {11,10869.0,0}},
{"RC2_6_7", {11,10343.4,0}},
{"RC2_6_8", {11,9820.0,0}},
{"RC2_6_9", {11,9483.2,0}},
{"C1_8_10", {72,24038.2,0}},
{"C1_8_1", {80,25156.9,0}},
{"C1_8_2", {72,24975.4,0}},
{"C1_8_3", {72,24201.6,0}},
{"C1_8_4", {72,23841.0,0}},
{"C1_8_5", {80,25138.6,0}},
{"C1_8_6", {79,25133.3,0}},
{"C1_8_7", {77,25127.3,0}},
{"C1_8_8", {73,24905.9,0}},
{"C1_8_9", {72,24219.0,0}},
{"C2_8_10", {23,10946.0,0}},
{"C2_8_1", {24,11631.9,0}},
{"C2_8_2", {23,11394.5,0}},
{"C2_8_3", {23,11138.1,0}},
{"C2_8_4", {22,10688.0,0}},
{"C2_8_5", {24,11395.6,0}},
{"C2_8_6", {23,11316.3,0}},
{"C2_8_7", {24,11332.9,0}},
{"C2_8_8", {23,11133.9,0}},
{"C2_8_9", {23,11140.4,0}},
{"R1_8_10", {72,31116.7,0}},
{"R1_8_1", {80,36429.0,0}},
{"R1_8_2", {72,32400.8,0}},
{"R1_8_3", {72,29439.6,0}},
{"R1_8_4", {72,27866.5,0}},
{"R1_8_5", {72,33640.7,0}},
{"R1_8_6", {72,31041.3,0}},
{"R1_8_7", {72,28989.1,0}},
{"R1_8_8", {72,27723.2,0}},
{"R1_8_9", {72,32390.5,0}},
{"R2_8_10", {15,20137.8,0}},
{"R2_8_1", {15,25033.5,0}},
{"R2_8_2", {15,21452.6,0}},
{"R2_8_3", {15,17239.2,0}},
{"R2_8_4", {15,13198.4,0}},
{"R2_8_5", {15,22875.7,0}},
{"R2_8_6", {15,19805.0,0}},
{"R2_8_7", {15,16470.1,0}},
{"R2_8_8", {15,12727.5,0}},
{"R2_8_9", {15,21380.9,0}},
{"RC1_8_10", {72,28228.7,0}},
{"RC1_8_1", {72,30062.4,0}},
{"RC1_8_2", {72,28375.4,0}},
{"RC1_8_3", {72,27505.5,0}},
{"RC1_8_4", {72,26595.5,0}},
{"RC1_8_5", {72,29306.4,0}},
{"RC1_8_6", {72,29272.3,0}},
{"RC1_8_7", {72,28868.9,0}},
{"RC1_8_8", {72,28517.8,0}},
{"RC1_8_9", {72,28411.5,0}},
{"RC2_8_10", {15,14515.8,0}},
{"RC2_8_1", {18,19269.4,0}},
{"RC2_8_2", {16,16772.6,0}},
{"RC2_8_3", {15,14079.3,0}},
{"RC2_8_4", {15,11050.2,0}},
{"RC2_8_5", {15,17541.7,0}},
{"RC2_8_6", {15,17250.1,0}},
{"RC2_8_7", {15,16521.1,0}},
{"RC2_8_8", {15,15615.3,0}},
{"RC2_8_9", {15,15252.4,0}},
{"C1_10_10", {90,39884.0,0}},
{"C1_10_1", {100,42444.8,0}},
{"C1_10_2", {90,41510.8,0}},
{"C1_10_3", {90,40160.0,0}},
{"C1_10_4", {90,39490.9,0}},
{"C1_10_5", {100,42434.8,0}},
{"C1_10_6", {99,42437.0,0}},
{"C1_10_7", {97,42420.4,0}},
{"C1_10_8", {92,41931.0,0}},
{"C1_10_9", {90,40387.1,0}},
{"C2_10_10", {28,15728.6,0}},
{"C2_10_1", {30,16841.1,0}},
{"C2_10_2", {29,16462.6,0}},
{"C2_10_3", {28,16036.5,0}},
{"C2_10_4", {28,15593.6,0}},
{"C2_10_5", {30,16521.3,0}},
{"C2_10_6", {29,16290.7,0}},
{"C2_10_7", {29,16378.4,0}},
{"C2_10_8", {28,16030.7,0}},
{"C2_10_9", {29,16077.0,0}},
{"R1_10_10", {91,47640.5,0}},
{"R1_10_1", {100,53245.4,0}},
{"R1_10_2", {91,48391.5,0}},
{"R1_10_3", {91,44884.6,0}},
{"R1_10_4", {91,42590.0,0}},
{"R1_10_5", {91,50493.7,0}},
{"R1_10_6", {91,47205.5,0}},
{"R1_10_7", {91,44284.2,0}},
{"R1_10_8", {91,42438.6,0}},
{"R1_10_9", {91,49353.2,0}},
{"R2_10_10", {19,30110.0,0}},
{"R2_10_1", {19,37147.6,0}},
{"R2_10_2", {19,31416.6,0}},
{"R2_10_3", {19,24568.8,0}},
{"R2_10_4", {19,18058.2,0}},
{"R2_10_5", {19,34361.4,0}},
{"R2_10_6", {19,29437.8,0}},
{"R2_10_7", {19,23309.0,0}},
{"R2_10_8", {19,17606.8,0}},
{"R2_10_9", {19,32289.9,0}},
{"RC1_10_10", {90,43750.9,0}},
{"RC1_10_1", {90,46045.5,0}},
{"RC1_10_2", {90,43870.6,0}},
{"RC1_10_3", {90,42402.1,0}},
{"RC1_10_4", {90,41499.3,0}},
{"RC1_10_5", {90,45241.0,0}},
{"RC1_10_6", {90,45174.0,0}},
{"RC1_10_7", {90,44692.5,0}},
{"RC1_10_8", {90,44159.6,0}},
{"RC1_10_9", {90,44118.3,0}},
{"RC2_10_10", {18,21914.4,0}},
{"RC2_10_1", {20,28269.8,0}},
{"RC2_10_2", {18,24387.5,0}},
{"RC2_10_3", {18,19768.0,0}},
{"RC2_10_4", {18,15804.9,0}},
{"RC2_10_5", {18,25990.2,0}},
{"RC2_10_6", {18,25911.6,0}},
{"RC2_10_7", {18,24544.0,0}},
{"RC2_10_8", {18,23473.0,0}},
{"RC2_10_9", {18,22945.5,0}},
	};

	if (dataBase.count(ins) > 0) {
		auto ret = dataBase[ins];
		return ret;
	}
	else {
		return InsData{ 0,0.0,0 };
	}
}

static double getSintefRL(string& ins) {

	static unordered_map<string, double> sintefMinlen = {
{"C1_2_1", 2704.57},
{"C1_2_2", 2917.89},
{"C1_2_3", 2707.35},
{"C1_2_4", 2643.31},
{"C1_2_5", 2702.05},
{"C1_2_6", 2701.04},
{"C1_2_7", 2701.04},
{"C1_2_8", 2775.48},
{"C1_2_9", 2687.83},
{"C1_2_10", 2643.51},
{"C2_2_1", 1931.44},
{"C2_2_2", 1863.16},
{"C2_2_3", 1775.08},
{"C2_2_4", 1703.43},
{"C2_2_5", 1878.85},
{"C2_2_6", 1857.35},
{"C2_2_7", 1849.46},
{"C2_2_8", 1820.53},
{"C2_2_9", 1830.05},
{"C2_2_10", 1806.58},
{"R1_2_1", 4784.11},
{"R1_2_2", 4039.86},
{"R1_2_3", 3381.96},
{"R1_2_4", 3057.81},
{"R1_2_5", 4107.86},
{"R1_2_6", 3583.14},
{"R1_2_7", 3150.11},
{"R1_2_8", 2951.99},
{"R1_2_9", 3760.58},
{"R1_2_10", 3301.18},
{"R2_2_1", 4483.16},
{"R2_2_2", 3621.20},
{"R2_2_3", 2880.62},
{"R2_2_4", 1981.29},
{"R2_2_5", 3366.79},
{"R2_2_6", 2913.03},
{"R2_2_7", 2451.14},
{"R2_2_8", 1849.87},
{"R2_2_9", 3092.04},
{"R2_2_10", 2654.97},
{"RC1_2_1", 3602.80},
{"RC1_2_2", 3249.05},
{"RC1_2_3", 3008.33},
{"RC1_2_4", 2851.68},
{"RC1_2_5", 3371.00},
{"RC1_2_6", 3324.80},
{"RC1_2_7", 3189.32},
{"RC1_2_8", 3083.93},
{"RC1_2_9", 3081.13},
{"RC1_2_10", 3000.30},
{"RC2_2_1", 3099.53},
{"RC2_2_2", 2825.24},
{"RC2_2_3", 2601.87},
{"RC2_2_4", 2038.56},
{"RC2_2_5", 2911.46},
{"RC2_2_6", 2873.12},
{"RC2_2_7", 2525.83},
{"RC2_2_8", 2292.53},
{"RC2_2_9", 2175.04},
{"RC2_2_10", 2015.60},
{"C1_4_1", 7152.02},
{"C1_4_2", 7686.38},
{"C1_4_3", 7060.67},
{"C1_4_4", 6803.24},
{"C1_4_5", 7152.02},
{"C1_4_6", 7153.41},
{"C1_4_7", 7417.92},
{"C1_4_8", 7347.23},
{"C1_4_9", 7042.53},
{"C1_4_10", 6860.63},
{"C2_4_1", 4116.05},
{"C2_4_2", 3929.89},
{"C2_4_3", 4018.02},
{"C2_4_4", 3702.49},
{"C2_4_5", 3938.69},
{"C2_4_6", 3875.94},
{"C2_4_7", 3894.13},
{"C2_4_8", 4233.20},
{"C2_4_9", 3864.68},
{"C2_4_10", 3825.67},
{"R1_4_1", 10372.31},
{"R1_4_2", 8898.15},
{"R1_4_3", 7808.34},
{"R1_4_4", 7282.78},
{"R1_4_5", 9223.00},
{"R1_4_6", 8358.69},
{"R1_4_7", 7616.15},
{"R1_4_8", 7257.28},
{"R1_4_9", 8694.78},
{"R1_4_10", 8094.10},
{"R2_4_1", 9210.15},
{"R2_4_2", 7606.75},
{"R2_4_3", 5911.07},
{"R2_4_4", 4241.47},
{"R2_4_5", 7127.83},
{"R2_4_6", 6122.60},
{"R2_4_7", 5018.53},
{"R2_4_8", 4015.60},
{"R2_4_9", 6400.10},
{"R2_4_10", 5779.03},
{"RC1_4_1", 8572.36},
{"RC1_4_2", 7892.52},
{"RC1_4_3", 7533.05},
{"RC1_4_4", 7308.55},
{"RC1_4_5", 8172.64},
{"RC1_4_6", 8164.98},
{"RC1_4_7", 7948.51},
{"RC1_4_8", 7772.57},
{"RC1_4_9", 7733.35},
{"RC1_4_10", 7596.04},
{"RC2_4_1", 6682.37},
{"RC2_4_2", 6180.62},
{"RC2_4_3", 4930.84},
{"RC2_4_4", 3631.01},
{"RC2_4_5", 6710.12},
{"RC2_4_6", 5766.61},
{"RC2_4_7", 5334.72},
{"RC2_4_8", 4792.69},
{"RC2_4_9", 4551.11},
{"RC2_4_10", 4278.61},
{"C1_6_1", 14095.64},
{"C1_6_2", 14163.31},
{"C1_6_3", 13777.81},
{"C1_6_4", 13558.54},
{"C1_6_5", 14085.72},
{"C1_6_6", 15832.68},
{"C1_6_7", 15742.78},
{"C1_6_8", 14389.82},
{"C1_6_9", 13693.42},
{"C1_6_10", 13637.34},
{"C2_6_1", 7774.10},
{"C2_6_2", 8258.20},
{"C2_6_3", 7506.62},
{"C2_6_4", 6909.58},
{"C2_6_5", 7575.20},
{"C2_6_6", 7470.36},
{"C2_6_7", 7512.07},
{"C2_6_8", 7539.73},
{"C2_6_9", 7911.61},
{"C2_6_10", 7255.69},
{"R1_6_1", 21394.95},
{"R1_6_2", 18605.32},
{"R1_6_3", 16908.87},
{"R1_6_4", 15749.74},
{"R1_6_5", 19385.64},
{"R1_6_6", 17801.30},
{"R1_6_7", 16523.93},
{"R1_6_8", 15610.00},
{"R1_6_9", 18530.96},
{"R1_6_10", 17610.12},
{"R2_6_1", 18205.58},
{"R2_6_2", 14754.13},
{"R2_6_3", 11188.70},
{"R2_6_4", 8008.14},
{"R2_6_5", 15067.34},
{"R2_6_6", 12498.27},
{"R2_6_7", 10064.56},
{"R2_6_8", 7571.99},
{"R2_6_9", 13377.56},
{"R2_6_10", 12202.28},
{"RC1_6_1", 16982.86},
{"RC1_6_2", 15914.70},
{"RC1_6_3", 15204.64},
{"RC1_6_4", 14777.19},
{"RC1_6_5", 16561.90},
{"RC1_6_6", 16504.25},
{"RC1_6_7", 16077.12},
{"RC1_6_8", 15914.91},
{"RC1_6_9", 15826.24},
{"RC1_6_10", 15675.99},
{"RC2_6_1", 13324.93},
{"RC2_6_2", 11555.51},
{"RC2_6_3", 9438.52},
{"RC2_6_4", 7057.94},
{"RC2_6_5", 12919.64},
{"RC2_6_6", 11913.11},
{"RC2_6_7", 10712.78},
{"RC2_6_8", 9990.40},
{"RC2_6_9", 9574.99},
{"RC2_6_10", 9058.90},
{"C1_8_1", 25030.36},
{"C1_8_2", 26540.53},
{"C1_8_3", 24242.49},
{"C1_8_4", 23824.17},
{"C1_8_5", 25166.28},
{"C1_8_6", 26911.94},
{"C1_8_7", 26480.07},
{"C1_8_8", 26099.74},
{"C1_8_9", 24300.21},
{"C1_8_10", 24070.17},
{"C2_8_1", 11662.08},
{"C2_8_2", 12285.31},
{"C2_8_3", 11410.69},
{"C2_8_4", 10990.89},
{"C2_8_5", 11425.23},
{"C2_8_6", 12235.30},
{"C2_8_7", 11370.84},
{"C2_8_8", 11288.01},
{"C2_8_9", 11592.52},
{"C2_8_10", 10977.36},
{"R1_8_1", 36769.51},
{"R1_8_2", 32322.85},
{"R1_8_3", 29342.53},
{"R1_8_4", 27771.88},
{"R1_8_5", 33529.73},
{"R1_8_6", 30925.26},
{"R1_8_7", 28861.89},
{"R1_8_8", 27652.70},
{"R1_8_9", 32301.66},
{"R1_8_10", 30959.95},
{"R2_8_1", 28112.36},
{"R2_8_2", 22795.79},
{"R2_8_3", 17703.99},
{"R2_8_4", 13192.32},
{"R2_8_5", 24255.00},
{"R2_8_6", 20412.02},
{"R2_8_7", 16597.87},
{"R2_8_8", 12642.67},
{"R2_8_9", 22277.04},
{"R2_8_10", 20358.61},
{"RC1_8_1", 30464.65},
{"RC1_8_2", 28511.55},
{"RC1_8_3", 27559.25},
{"RC1_8_4", 26675.45},
{"RC1_8_5", 29508.27},
{"RC1_8_6", 29458.38},
{"RC1_8_7", 29021.59},
{"RC1_8_8", 28607.51},
{"RC1_8_9", 28555.53},
{"RC1_8_10", 28371.40},
{"RC2_8_1", 20981.14},
{"RC2_8_2", 18151.95},
{"RC2_8_3", 14427.39},
{"RC2_8_4", 10999.03},
{"RC2_8_5", 19074.02},
{"RC2_8_6", 18143.04},
{"RC2_8_7", 16817.46},
{"RC2_8_8", 15759.14},
{"RC2_8_9", 15333.59},
{"RC2_8_10", 14411.81},
{"C1_10_1", 42478.95},
{"C1_10_2", 42222.96},
{"C1_10_3", 40101.36},
{"C1_10_4", 39468.60},
{"C1_10_5", 42469.18},
{"C1_10_6", 43830.21},
{"C1_10_7", 43372.03},
{"C1_10_8", 42660.70},
{"C1_10_9", 40327.62},
{"C1_10_10", 39852.44},
{"C2_10_1", 16879.24},
{"C2_10_2", 17126.39},
{"C2_10_3", 16829.47},
{"C2_10_4", 15607.48},
{"C2_10_5", 16561.29},
{"C2_10_6", 16863.71},
{"C2_10_7", 17602.84},
{"C2_10_8", 16512.43},
{"C2_10_9", 16363.94},
{"C2_10_10", 15937.45},
{"R1_10_1", 53435.33},
{"R1_10_2", 48307.81},
{"R1_10_3", 44722.22},
{"R1_10_4", 42485.14},
{"R1_10_5", 50452.87},
{"R1_10_6", 46974.45},
{"R1_10_7", 44041.02},
{"R1_10_8", 42322.00},
{"R1_10_9", 49208.10},
{"R1_10_10", 47414.38},
{"R2_10_1", 42182.57},
{"R2_10_2", 33411.21},
{"R2_10_3", 24916.88},
{"R2_10_4", 17851.96},
{"R2_10_5", 36216.05},
{"R2_10_6", 29978.02},
{"R2_10_7", 23219.61},
{"R2_10_8", 17442.29},
{"R2_10_9", 32995.71},
{"R2_10_10", 30207.49},
{"RC1_10_1", 45830.62},
{"RC1_10_2", 43718.84},
{"RC1_10_3", 42163.46},
{"RC1_10_4", 41408.92},
{"RC1_10_5", 45069.37},
{"RC1_10_6", 44944.95},
{"RC1_10_7", 44466.70},
{"RC1_10_8", 43985.99},
{"RC1_10_9", 43899.45},
{"RC1_10_10", 43576.10},
{"RC2_10_1", 30276.27},
{"RC2_10_2", 26104.09},
{"RC2_10_3", 19913.46},
{"RC2_10_4", 15693.28},
{"RC2_10_5", 27067.04},
{"RC2_10_6", 26741.27},
{"RC2_10_7", 25017.97},
{"RC2_10_8", 23595.33},
{"RC2_10_9", 22943.42},
{"RC2_10_10", 21834.94},
	};
	if (sintefMinlen.count(ins) > 0) {
		auto ret = sintefMinlen[ins];
		return ret;
	}
	else {
		return 0.0;
	}
}

struct Input {

	Configuration cfg;

	string example = "";
	int custCnt = 0;
	DisType Q = 0;
	int vehicleCnt = 0;
	Vec<Data> datas;
	int sintefRecRN = 0;
	DisType dimacsRecRL = 0;
	DisType sintefRecRL = 0;
	bool isOptRL = false;

	Vec<Vec<DisType>> disOf;

	// disOf[v][w] 表示w和v之间的距离
	Vec<Vec<int>> allCloseOf;
	// input.allCloseOf[v][wpos] 表示v的地理位置第wpos近的点
	Vec<Vec<int>> addSTclose;
	// input.addSTclose[v][wpos] 表示v的地理位置加上v的服务时间第wpos近的点
	Vec<Vec<int>> addSTJIsxthcloseOf;
	//表示v的地理位置加上v的服务时间作为排序依据 input.addSTJIsxthcloseOf[v][w],w是v的第几近

	Vec<Vec<int>> jIsxthcloseOf;
	//表示v的地理位置距离作为排序依据 jIsxthcloseOf[v][w],w是v的第几近

	Vec<Vec<int>> iInNeicloseOf;

	Vec<Vec<int>> iInNeicloseOfUnionNeiCloseOfI;

	Input(Environment& env,Configuration& cfg):cfg(cfg) {
		initInput(env);
	}

	bool initInput(Environment env) {

		readDimacsInstance(env.inputPath);

		for (int i = custCnt + 1; i < datas.size(); ++i) {
			datas[i] = datas[0];
			datas[i].CUSTNO = i;
		}

		disOf = Vec< Vec<DisType> >
			(custCnt + 2, Vec<DisType>(custCnt + 2, 0));

		/*
			sqrt函数有三种形式
			double sqrt(double x);
			float sqrtf(float x);
			long double sqrtl(long double x);
			float精度最低，double较高，long double精度最高
		*/

		for (int i = 0; i <= custCnt + 1; ++i) {
			for (int j = i + 1; j <= custCnt + 1; ++j) {
				Data& d1 = datas[i];
				Data& d2 = datas[j];
				double dis = sqrtl((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
					+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));

#if DISDOUBLE 
				disOf[j][i] = disOf[i][j] = dis;
#else
				disOf[j][i] = disOf[i][j] = dis;
				//disOf[j][i] = disOf[i][j] = dis;
#endif // DISDOUBLE

			}
		}

		allCloseOf = Vec< Vec<int>>(custCnt + 1, Vec<int>(0));

		auto canlinkDir = [&](int v, int w) {

			DisType av = disOf[0][v];
			DisType aw = av + datas[v].SERVICETIME + disOf[v][w];
			DisType ptw = std::max<DisType>(0, aw - datas[w].DUEDATE);
			DisType an = aw + datas[w].SERVICETIME + disOf[w][0];
			ptw += std::max<DisType>(0, an - datas[0].DUEDATE);
			return ptw == 0;
		};

		auto canLinkNode = [&](int v, int w) {
			
			if (!canlinkDir(v,w) && !canlinkDir(w,v)) {
				return false;
			}
			return true;
		};

		for (int v = 0; v <= custCnt; ++v) {

			Vec<int> nums;
			nums.reserve(custCnt + 1);

			for (int pos = 0; pos <= custCnt; ++pos) {
				if (pos != v) {
					nums.push_back(pos);
				}
			}

			auto cmp = [&](const int a, const int b) {
				int aLinkv = canLinkNode(a, v);
				int bLinkv = canLinkNode(b, v);
				if ( (aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
					return disOf[a][v] < disOf[b][v];
				}
				else {
					return aLinkv ? true : false;
				}
				return false;
			};

			sort(nums.begin(), nums.end(), cmp);

			allCloseOf[v] = nums;
		}

		jIsxthcloseOf = Vec< Vec<int> >
			(custCnt + 1, Vec<int>(custCnt + 1, -1));

		for (int i = 0; i <= custCnt; ++i) {
			for (int j = 0; j < custCnt; ++j) {
				jIsxthcloseOf[i][allCloseOf[i][j]] = j;
			}
		}

		iInNeicloseOfUnionNeiCloseOfI =  Vec< Vec<int> > (custCnt+1);

		Vec<int> devs = cfg.outNeiSize;
		int deNeiSize = devs[0];
		iInNeicloseOf = Vec< Vec<int> >
			(custCnt + 1, Vec<int>());
		for (int i = 0; i < custCnt + 1; ++i) {
			iInNeicloseOf[i].reserve(custCnt);
		}
		for (int v = 0; v <= custCnt; ++v) {
			for (int wpos = 0; wpos < deNeiSize; ++wpos) {
				LL w = allCloseOf[v][wpos];
				iInNeicloseOf[w].push_back(v);
			}
		}
		iInNeicloseOfUnionNeiCloseOfI
			= Vec<Vec<int>>(custCnt + 1);
		for (std::size_t v = 0; v <= custCnt; ++v) {

			iInNeicloseOfUnionNeiCloseOfI[v] = Vec<int>
				(allCloseOf[v].begin(), allCloseOf[v].begin() + deNeiSize);

			for (std::size_t wpos = 0; wpos < iInNeicloseOf[v].size(); ++wpos) {

				int w = iInNeicloseOf[v][wpos];
				if (jIsxthcloseOf[v][w] >= deNeiSize) {
					iInNeicloseOfUnionNeiCloseOfI[v].push_back(w);
				}
			}
		}

		addSTclose = Vec< Vec<int>>
			(custCnt + 1, Vec<int>());

		for (int v = 0; v <= custCnt; ++v) {
			addSTclose[v].reserve(custCnt);
			for (int w = 0; w <= custCnt; ++w) {
				if (w == v) {
					continue;
				}
				addSTclose[v].push_back(w);
			}

			auto cmp = [&](const int a, const int b) {

				int aLinkv = canLinkNode(a, v);
				int bLinkv = canLinkNode(b, v);
				if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
					return disOf[v][a] + datas[a].SERVICETIME <
						disOf[v][b] + datas[b].SERVICETIME;
				}
				else {
					return aLinkv ? true : false;
				}

			};
			sort(addSTclose[v].begin(), addSTclose[v].end(), cmp);
		}

		addSTJIsxthcloseOf = Vec< Vec<int>>
			(custCnt + 1, Vec<int>(custCnt + 1, -1));

		for (int v = 0; v <= custCnt; ++v) {
			for (std::size_t wpos = 0; wpos < addSTclose[v].size(); ++wpos) {
				int w = addSTclose[v][wpos];
				addSTJIsxthcloseOf[v][w] = wpos;
			}
		}

		auto info = getInsData(example);
		sintefRecRN = info.minRN;
		dimacsRecRL = info.minRL * disMul;
		sintefRecRL = getSintefRL(example)* disMul;
		isOptRL = info.isOpt;

		Log(Log::Level::Warning) <<"sintefRecRN: " << sintefRecRN << endl;
		Log(Log::Level::Warning) <<"sintefRecRL: " << sintefRecRL << endl;
		Log(Log::Level::Warning) <<"dimacsRecRL: " << dimacsRecRL << endl;
		Log(Log::Level::Warning) <<"isOptRL: " << isOptRL << endl;
		Log(Log::Level::Warning) <<"Q: " << Q << endl;
			
		return true;
	}

	bool readDimacsInstance(string& instanciaPath) {

		//debug(instanciaPath.c_str());
		FILE* file = fopen(instanciaPath.c_str(), "r");

		if (!file) {
			std::cout << instanciaPath << "ERROR: Instance path wrong." << std::endl;
			exit(EXIT_FAILURE);
		}

		char name[64];
		this->datas = Vec<Data>(1000 * 3 + 3);

		fscanf(file, "%s\n", name);
		this->example = std::string(name);
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%d %d\n", &this->vehicleCnt, &this->Q);
		fscanf(file, "%*[^\n]\n");
		fscanf(file, "%*[^\n]\n");
		
		this->Q *= disMul;
		string line = "";
		size_t len = 0;
		long read = 0;
		 
		int index = 0;
		int id = -1, coordx = -1, coordy = -1, demand = -1;
		int ready_time = -1, due_date = -1, service_time = -1;
		int readArgNum = 0;
		while ( (readArgNum = fscanf(file, "%d %d %d %d %d %d %d\n", &id, &coordx, &coordy, &demand, &ready_time, &due_date, &service_time)) == 7) {
			this->datas[index].CUSTNO = id;
			this->datas[index].XCOORD = coordx* disMul;
			this->datas[index].YCOORD = coordy * disMul;
			this->datas[index].DEMAND = demand * disMul;
			this->datas[index].READYTIME = ready_time * disMul;
			this->datas[index].DUEDATE = due_date * disMul;
			this->datas[index].SERVICETIME = service_time * disMul;
			
			if (index > 0) {
				auto& dt = datas[index];
				dt.polarAngle = CircleSector::positive_mod
				(32768. * atan2(dt.YCOORD - datas[0].YCOORD, dt.XCOORD - datas[0].XCOORD) / PI);
			}
			++index;
		}
		custCnt = index-1;
		fclose(file);
	}
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
	Vec<bool> HArr;

};

bool solveCommandLine(int argc, char* argv[], Configuration& cfg,Environment& env) {
	
	MyString ms;
	if (argc >= 2) {
		cfg.breakRecord = ms.str_int(argv[1]);
	}

	if (argc >= 3) {
		string inpath = argv[2];
		env.setInputPath(inpath);
	}

	if (argc >= 4) {
		cfg.runTimer = ms.str_int(argv[3]);
	}

	if (argc >= 5) {
		env.seed = ms.str_int(argv[4]);
	}
	return true;
}

bool saveSln(Input& input, Output& output,Configuration& cfg,Environment& env) {

	DateTime d(time(0));
	MyString ms;
	// 输出 tm 结构的各个组成部分
	//string day = /*ms.LL_str(d.year) + */ms.LL_str(d.month) + ms.LL_str(d.day);
	string day = __DATE__;
	for (auto& c : day) {
		if (c == ' ') {
			c = '_';
		}
	}
	string pwe0 = ms.int_str(cfg.Pwei0);
	string pwe1 = ms.int_str(cfg.Pwei1);
	string minKmax = ms.int_str(cfg.minKmax);
	string maxKmax = ms.int_str(cfg.maxKmax);

	string type = output.rts.size() < input.sintefRecRN ? "br" : "Ej";

	std::ofstream rgbData;
	string wrPath = env.outputPath 
		+ type + "_" + day
		/*+ "PW" + pwe0 + pwe1
		+ "KM" + minKmax + maxKmax
		+ "SW" + EPPerturbStep*/
		//+input.example 
		+".csv";

	rgbData.open(wrPath, std::ios::app | std::ios::out);

	if (!rgbData) {
		Log(Log::Level::Warning) << "output file open errno" << endl;
		return false;
	}
	
	// instance,sintefRecRN,sinrn,lyhrn,dimacsRecRL，sinrl,lyhrl,time,epsize,minep,ptw,pc,rts,seeds

	rgbData << input.example << ",";
	//rgbData << input.sumQToQ << ",";
	rgbData << input.sintefRecRN << ",";
	rgbData << output.rts.size() << ",";
	
	rgbData << output.state << ",";

	rgbData << input.dimacsRecRL << ",";
	rgbData << double((double)(output.state - input.dimacsRecRL)/ input.dimacsRecRL)*100 << ",";
	rgbData << input.sintefRecRL << ",";
	rgbData << double((double)(output.state - input.sintefRecRL) / input.sintefRecRL) * 100 << ",";
	
	rgbData << output.runTime << ",";
	rgbData << output.EP.size() << ",";

	rgbData << output.minEP << ",";
	rgbData << output.PtwNoWei << ",";
	rgbData << output.Pc << ",";
	for (std::size_t i = 0; i < output.rts.size(); ++i) {
		rgbData << "Route  " << i + 1 << " : ";
		for (std::size_t j = 0; j < output.rts[i].size(); ++j) {
			rgbData << output.rts[i][j] << " ";
		}
		rgbData << "| ";
	}

	rgbData << ",";
	rgbData << env.seed;

	rgbData << endl;
	rgbData.close();

	return true;
}

}


#endif // !vrptwNew_PROBLEM_H

