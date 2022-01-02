
#include "Goal.h"
#include "EAX.h"

namespace hust {

//TODO[0]:这个要更新成最新的了
static InsData getInsData(std::string& ins) {
	static std::unordered_map<std::string, InsData> dataBase = {
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

//TODO[0]:下面的也加上路径数量，都弄成InsData的形式
static double getSintefRL(std::string& ins) {

	static std::unordered_map<std::string, double> sintefMinlen = {
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

//TODO[0]:下面的也加上路径数量，都弄成InsData的形式
static double getNagataRL(std::string ins) {

	static std::unordered_map<std::string, double> nagataRL = {
	{ "R1_2_1", 4784.11 },
	{ "R2_2_1", 4483.16 },
	{ "C1_2_1", 2704.57 },
	{ "C2_2_1", 1931.44 },
	{ "RC1_2_1", 3618.05 },
	{ "RC2_2_1", 3099.53 },
	{ "R1_2_2", 4041.71 },
	{ "R2_2_2", 3621.20 },
	{ "C1_2_2", 2917.89 },
	{ "C2_2_2", 1863.16 },
	{ "RC1_2_2", 3249.34 },
	{ "RC2_2_2", 2825.24 },
	{ "R1_2_3", 3381.96 },
	{ "R2_2_3", 2880.62 },
	{ "C1_2_3", 2707.35 },
	{ "C2_2_3", 1775.08 },
	{ "RC1_2_3", 3008.33 },
	{ "RC2_2_3", 2601.87 },
	{ "R1_2_4", 3057.81 },
	{ "R2_2_4", 1981.30 },
	{ "C1_2_4", 2643.31 },
	{ "C2_2_4", 1703.43 },
	{ "RC1_2_4", 2851.68 },
	{ "RC2_2_4", 2038.56 },
	{ "R1_2_5", 4107.86 },
	{ "R2_2_5", 3366.79 },
	{ "C1_2_5", 2702.05 },
	{ "C2_2_5", 1879.31 },
	{ "RC1_2_5", 3372.88 },
	{ "RC2_2_5", 2911.46 },
	{ "R1_2_6", 3583.77 },
	{ "R2_2_6", 2913.03 },
	{ "C1_2_6", 2701.04 },
	{ "C2_2_6", 1857.35 },
	{ "RC1_2_6", 3328.91 },
	{ "RC2_2_6", 2873.12 },
	{ "R1_2_7", 3150.11 },
	{ "R2_2_7", 2451.14 },
	{ "C1_2_7", 2701.04 },
	{ "C2_2_7", 1849.46 },
	{ "RC1_2_7", 3189.32 },
	{ "RC2_2_7", 2525.83 },
	{ "R1_2_8", 2951.99 },
	{ "R2_2_8", 1849.87 },
	{ "C1_2_8", 2775.48 },
	{ "C2_2_8", 1820.53 },
	{ "RC1_2_8", 3083.93 },
	{ "RC2_2_8", 2295.97 },
	{ "R1_2_9", 3762.57 },
	{ "R2_2_9", 3092.04 },
	{ "C1_2_9", 2687.83 },
	{ "C2_2_9", 1830.05 },
	{ "RC1_2_9", 3081.13 },
	{ "RC2_2_9", 2175.04 },
	{ "R1_2_10", 3301.18 },
	{ "R2_2_10", 2654.97 },
	{ "C1_2_10", 2643.55 },
	{ "C2_2_10", 1806.58 },
	{ "RC1_2_10", 3002.97 },
	{ "RC2_2_10", 2015.61 },
	{ "R1_4_1", 10396.72 },
	{ "R2_4_1", 9210.15 },
	{ "C1_4_1", 7152.06 },
	{ "C2_4_1", 4116.14 },
	{ "RC1_4_1", 8749.98 },
	{ "RC2_4_1", 6682.37 },
	{ "R1_4_2", 8962.13 },
	{ "R2_4_2", 7606.75 },
	{ "C1_4_2", 7695.55 },
	{ "C2_4_2", 3930.05 },
	{ "RC1_4_2", 7925.66 },
	{ "RC2_4_2", 6197.42 },
	{ "R1_4_3", 7836.18 },
	{ "R2_4_3", 5911.50 },
	{ "C1_4_3", 7069.24 },
	{ "C2_4_3", 4018.74 },
	{ "RC1_4_3", 7554.95 },
	{ "RC2_4_3", 4930.84 },
	{ "R1_4_4", 7301.61 },
	{ "R2_4_4", 4241.47 },
	{ "C1_4_4", 6803.41 },
	{ "C2_4_4", 3707.38 },
	{ "RC1_4_4", 7323.08 },
	{ "RC2_4_4", 3631.01 },
	{ "R1_4_5", 9294.34 },
	{ "R2_4_5", 7132.14 },
	{ "C1_4_5", 7152.06 },
	{ "C2_4_5", 3938.69 },
	{ "RC1_4_5", 8230.39 },
	{ "RC2_4_5", 6711.72 },
	{ "R1_4_6", 8408.02 },
	{ "R2_4_6", 6127.60 },
	{ "C1_4_6", 7153.45 },
	{ "C2_4_6", 3875.94 },
	{ "RC1_4_6", 8195.99 },
	{ "RC2_4_6", 5766.61 },
	{ "R1_4_7", 7649.48 },
	{ "R2_4_7", 5028.33 },
	{ "C1_4_7", 7421.56 },
	{ "C2_4_7", 3894.16 },
	{ "RC1_4_7", 7987.79 },
	{ "RC2_4_7", 5336.58 },
	{ "R1_4_8", 7293.21 },
	{ "R2_4_8", 4015.60 },
	{ "C1_4_8", 7364.31 },
	{ "C2_4_8", 3792.76 },
	{ "RC1_4_8", 7801.11 },
	{ "RC2_4_8", 4795.72 },
	{ "R1_4_9", 8758.61 },
	{ "R2_4_9", 6404.83 },
	{ "C1_4_9", 7076.02 },
	{ "C2_4_9", 3865.65 },
	{ "RC1_4_9", 7777.79 },
	{ "RC2_4_9", 4551.80 },
	{ "R1_4_10", 8128.01 },
	{ "R2_4_10", 5791.70 },
	{ "C1_4_10", 6860.63 },
	{ "C2_4_10", 3835.85 },
	{ "RC1_4_10", 7625.34 },
	{ "RC2_4_10", 4285.05 },
	{ "R1_6_1", 21426.76 },
	{ "R2_6_1", 18214.90 },
	{ "C1_6_1", 14103.87 },
	{ "C2_6_1", 7774.16 },
	{ "RC1_6_1", 17405.31 },
	{ "RC2_6_1", 13360.73 },
	{ "R1_6_2", 19014.13 },
	{ "R2_6_2", 14779.26 },
	{ "C1_6_2", 14163.31 },
	{ "C2_6_2", 8290.76 },
	{ "RC1_6_2", 16091.11 },
	{ "RC2_6_2", 11555.76 },
	{ "R1_6_3", 17191.34 },
	{ "R2_6_3", 11191.01 },
	{ "C1_6_3", 13778.75 },
	{ "C2_6_3", 7528.76 },
	{ "RC1_6_3", 15322.93 },
	{ "RC2_6_3", 9447.97 },
	{ "R1_6_4", 15957.34 },
	{ "R2_6_4", 8032.28 },
	{ "C1_6_4", 13561.11 },
	{ "C2_6_4", 6910.62 },
	{ "RC1_6_4", 14891.50 },
	{ "RC2_6_4", 7092.90 },
	{ "R1_6_5", 20018.41 },
	{ "R2_6_5", 15107.97 },
	{ "C1_6_5", 14086.23 },
	{ "C2_6_5", 7575.20 },
	{ "RC1_6_5", 16856.18 },
	{ "RC2_6_5", 13000.84 },
	{ "R1_6_6", 18211.84 },
	{ "R2_6_6", 12503.26 },
	{ "C1_6_6", 14089.66 },
	{ "C2_6_6", 7479.04 },
	{ "RC1_6_6", 16778.82 },
	{ "RC2_6_6", 11973.95 },
	{ "R1_6_7", 16793.65 },
	{ "R2_6_7", 10147.52 },
	{ "C1_6_7", 14855.32 },
	{ "C2_6_7", 7512.22 },
	{ "RC1_6_7", 16322.95 },
	{ "RC2_6_7", 10724.85 },
	{ "R1_6_8", 15766.32 },
	{ "R2_6_8", 7574.39 },
	{ "C1_6_8", 14559.16 },
	{ "C2_6_8", 7601.94 },
	{ "RC1_6_8", 16136.38 },
	{ "RC2_6_8", 10007.82 },
	{ "R1_6_9", 19153.71 },
	{ "R2_6_9", 13377.56 },
	{ "C1_6_9", 13694.94 },
	{ "C2_6_9", 8036.01 },
	{ "RC1_6_9", 15996.82 },
	{ "RC2_6_9", 9580.87 },
	{ "R1_6_10", 18047.99 },
	{ "R2_6_10", 12265.83 },
	{ "C1_6_10", 13637.34 },
	{ "C2_6_10", 7274.04 },
	{ "RC1_6_10", 15795.75 },
	{ "RC2_6_10", 9069.41 },
	{ "R1_8_1", 36852.06 },
	{ "R2_8_1", 28125.99 },
	{ "C1_8_1", 25191.94 },
	{ "C2_8_1", 11662.08 },
	{ "RC1_8_1", 35467.41 },
	{ "RC2_8_1", 21018.41 },
	{ "R1_8_2", 32790.69 },
	{ "R2_8_2", 22795.79 },
	{ "C1_8_2", 27058.99 },
	{ "C2_8_2", 12376.66 },
	{ "RC1_8_2", 30095.13 },
	{ "RC2_8_2", 18181.14 },
	{ "R1_8_3", 29708.38 },
	{ "R2_8_3", 17714.05 },
	{ "C1_8_3", 24362.72 },
	{ "C2_8_3", 11411.53 },
	{ "RC1_8_3", 28520.62 },
	{ "RC2_8_3", 14442.40 },
	{ "R1_8_4", 28078.94 },
	{ "R2_8_4", 13206.10 },
	{ "C1_8_4", 23848.03 },
	{ "C2_8_4", 10752.87 },
	{ "RC1_8_4", 27311.32 },
	{ "RC2_8_4", 11019.70 },
	{ "R1_8_5", 34342.50 },
	{ "R2_8_5", 24345.47 },
	{ "C1_8_5", 25171.39 },
	{ "C2_8_5", 11425.23 },
	{ "RC1_8_5", 32978.66 },
	{ "RC2_8_5", 19137.47 },
	{ "R1_8_6", 31341.06 },
	{ "R2_8_6", 20479.46 },
	{ "C1_8_6", 25169.13 },
	{ "C2_8_6", 11353.61 },
	{ "RC1_8_6", 33181.53 },
	{ "RC2_8_6", 18146.03 },
	{ "R1_8_7", 29195.72 },
	{ "R2_8_7", 16677.31 },
	{ "C1_8_7", 25906.47 },
	{ "C2_8_7", 11379.87 },
	{ "RC1_8_7", 30768.36 },
	{ "RC2_8_7", 16843.96 },
	{ "R1_8_8", 27960.28 },
	{ "R2_8_8", 12645.63 },
	{ "C1_8_8", 25401.38 },
	{ "C2_8_8", 11309.98 },
	{ "RC1_8_8", 30694.45 },
	{ "RC2_8_8", 15799.03 },
	{ "R1_8_9", 32791.14 },
	{ "R2_8_9", 22335.51 },
	{ "C1_8_9", 24726.74 },
	{ "C2_8_9", 11721.31 },
	{ "RC1_8_9", 30299.38 },
	{ "RC2_8_9", 15361.73 },
	{ "R1_8_10", 31474.59 },
	{ "R2_8_10", 20401.47 },
	{ "C1_8_10", 24197.40 },
	{ "C2_8_10", 10990.64 },
	{ "RC1_8_10", 29765.69 },
	{ "RC2_8_10", 14439.14 },
	{ "R1_10_1", 53501.39 },
	{ "R2_10_1", 42294.31 },
	{ "C1_10_1", 42482.61 },
	{ "C2_10_1", 16879.24 },
	{ "RC1_10_1", 47188.44 },
	{ "RC2_10_1", 30289.39 },
	{ "R1_10_2", 49951.00 },
	{ "R2_10_2", 33459.32 },
	{ "C1_10_2", 43023.01 },
	{ "C2_10_2", 17126.39 },
	{ "RC1_10_2", 44620.38 },
	{ "RC2_10_2", 25425.65 },
	{ "R1_10_3", 45707.48 },
	{ "R2_10_3", 24938.95 },
	{ "C1_10_3", 40339.01 },
	{ "C2_10_3", 16322.02 },
	{ "RC1_10_3", 42786.69 },
	{ "RC2_10_3", 20043.04 },
	{ "R1_10_4", 43262.82 },
	{ "R2_10_4", 17880.11 },
	{ "C1_10_4", 39568.77 },
	{ "C2_10_4", 15696.68 },
	{ "RC1_10_4", 41892.10 },
	{ "RC2_10_4", 15741.56 },
	{ "R1_10_5", 53289.48 },
	{ "R2_10_5", 36258.34 },
	{ "C1_10_5", 42478.21 },
	{ "C2_10_5", 16572.60 },
	{ "RC1_10_5", 46078.09 },
	{ "RC2_10_5", 27140.77 },
	{ "R1_10_6", 48424.28 },
	{ "R2_10_6", 30073.60 },
	{ "C1_10_6", 42472.31 },
	{ "C2_10_6", 17109.56 },
	{ "RC1_10_6", 45932.63 },
	{ "RC2_10_6", 26877.69 },
	{ "R1_10_7", 44975.94 },
	{ "R2_10_7", 23253.89 },
	{ "C1_10_7", 42844.23 },
	{ "C2_10_7", 16428.11 },
	{ "RC1_10_7", 45405.13 },
	{ "RC2_10_7", 25161.74 },
	{ "R1_10_8", 42960.69 },
	{ "R2_10_8", 17509.69 },
	{ "C1_10_8", 42436.10 },
	{ "C2_10_8", 16191.70 },
	{ "RC1_10_8", 44628.29 },
	{ "RC2_10_8", 23740.40 },
	{ "R1_10_9", 51615.61 },
	{ "R2_10_9", 33068.74 },
	{ "C1_10_9", 40983.55 },
	{ "C2_10_9", 16372.09 },
	{ "RC1_10_9", 44634.17 },
	{ "RC2_10_9", 23065.64 },
	{ "R1_10_10", 48979.36 },
	{ "R2_10_10", 30312.50 },
	{ "C1_10_10", 40205.01 },
	{ "C2_10_10", 16077.57 },
	{ "RC1_10_10", 44265.86 },
	{ "RC2_10_10", 21910.33 }

	};

	if (nagataRL.count(ins) > 0) {
		auto ret = nagataRL[ins];
		return ret;
	}
	else {
		return 0.0;
	}
	return 0.0;
}

Goal::Goal():gloalTimer(0){

	eaxYearTable = Vec<Vec<LL>>
		(globalCfg->popSize, Vec<LL>(globalCfg->popSize));

	auto info = getInsData(globalInput->example);
	sintefRecRN = info.minRN;
	sintefRecRL = getSintefRL(globalInput->example) * disMul;
	naRecRL = getNagataRL(globalInput->example) * disMul;

	Solver dimBKS;
	dimBKS.initSolution(5);

	dimacsRecRN = dimBKS.rts.cnt;
	dimacsRecRL = dimBKS.RoutesCost;

	println("sintefRecRN:", sintefRecRN);
	println("sintefRecRL:", sintefRecRL);
	println("naRecRL:", naRecRL);
	println("dimacsRecRN:", dimacsRecRN);
	println("dimacsRecRL:", dimacsRecRL);

}

#if 0
Vec<int> Goal::getpairOfPaPb() {

	int paIndex = myRand->pick(globalCfg->popSize);
	int pbIndex = (paIndex + 1) % globalCfg->popSize;
	int sameNum = 1;
	int retPbIndex = pbIndex;
	for (int i = 1; i + 1 < globalCfg->popSize; ++i) {
		int newPbIndex = (pbIndex + i) % globalCfg->popSize;

		if (eaxYearTable[paIndex][newPbIndex]
			< eaxYearTable[paIndex][pbIndex]) {
			retPbIndex = newPbIndex;
			sameNum = 1;
		}
		if (eaxYearTable[paIndex][newPbIndex]
			== eaxYearTable[paIndex][pbIndex]) {
			++sameNum;
			if (myRand->pick(sameNum) == 0) {
				retPbIndex = newPbIndex;
			}
		}
	}
	pbIndex = retPbIndex;
	return { paIndex,pbIndex };
}
#endif // 0

int Goal::doTwoKindEAX(Solver& pa, Solver& pb, int kind) {

	int retState = 0; // 0 表示没有成功更新最优解，1表示更新了最优解 -1表示这两个解无法进行eax

	EAX eax(pa, pb);
	eax.generateCycles();

	if (eax.abCycleSet.size() == 0) {
		return -1;
	}

	Solver paBest = pa;

	//static int genSol = 0;
	//static int repSol = 0;

	int contiNotRepair = 1;

	for (int ch = 1; ch <= globalCfg->naEaxCh; ++ch) {

		Solver pc = pa;
		int eaxState = 0;
		if (kind == 0) {
			eaxState = eax.doNaEAX(pa, pb, pc);
		}
		else {
			eaxState = eax.doPrEAX(pa, pb, pc);
		}

		if (eaxState == -1) {
			eax.generateCycles();
			continue;
		}

		//++genSol;
		if (eax.repairSolNum == 0) {
			++contiNotRepair;
			if (contiNotRepair >= 3) {
				eax.generateCycles();
			}
			continue;
		}
		else {
			contiNotRepair = 1;
		}

		//++repSol;
		auto newCus = EAX::getDiffCusofPb(pa, pc);

		if (newCus.size() > 0) {
			pc.mRLLocalSearch(1, newCus);
			auto cus1 = EAX::getDiffCusofPb(pa, pc);
			if (cus1.size() == 0) {
				//debug("pa is same as pa");
			}
		}
		else {
			//debug("pa is same as pa");
		}

		if (pc.RoutesCost < paBest.RoutesCost) {
			paBest = pc;
			
			bool isup = bks->updateBKS(pc, "time:" + std::to_string(gloalTimer.getRunTime()) + " eax ls, kind:" + std::to_string(kind));
			if (isup) {
				ch = 1;
				retState = 1;
				//println("bestSolFound cost:", bestSolFound.RoutesCost, ", kind, "choosecyIndex:", eax.choosecyIndex, "chooseuId:", eax.unionIndex);
			}
		}

	}

	if (paBest.RoutesCost < pa.RoutesCost) {
		if (paBest.RoutesCost < pb.RoutesCost) {
			auto diffwithPa = EAX::getDiffCusofPb(paBest, pa);
			auto diffwithPb = EAX::getDiffCusofPb(paBest, pb);
			if (diffwithPa.size() <= diffwithPb.size()) {
				pa = paBest;
			}
			else {
				//println("replace with pb,kind:", kind);
				pb = paBest;
			}
		}
		else {
			pa = paBest;
		}
	}
	return retState;
}

bool Goal::perturbThePop() {

	auto pertuOrder = Vec<int>(pool.size(), 0);
	std::iota(pertuOrder.begin(), pertuOrder.end(), 0);
	std::sort(pertuOrder.begin(), pertuOrder.end(), [&](int x, int y) {return
		pool[x].RoutesCost < pool[y].RoutesCost;
		});
	//unsigned shuseed = myRand->pickRandSeed();
	//std::shuffle(pertuOrder.begin(), pertuOrder.end(), std::default_random_engine(shuseed));

	#if 0
	for (int peri = 0; peri < pool.size(); ++peri) {
		int pi = pertuOrder[peri];
		auto& sol = pool[pi];
		auto before = sol.RoutesCost;
		Solver sclone = sol;
		sclone.resetSol();
		int initKind = myRand->pick(5);
		sclone.initSolution(initKind);
		sclone.adjustRN();
		sclone.mRLLocalSearch(0, {});
		//sclone.ruinLocalSearch(1);
		sol = sclone;

		auto after = sol.RoutesCost;
		println("before:", before, "after:", after, "initKind:", initKind);
	}
	#else	

	for (int peri = 0; peri < pool.size(); ++peri) {
		int pi = pertuOrder[peri];
		auto& sol = pool[pi];
		auto before = sol.RoutesCost;
		Solver sclone = sol;
		//sclone.saveOutAsSintefFile();
		for (int i = 0; i < 10; ++i) {
			int kind = myRand->pick(4);
			if (kind < 3) {
				int clearEPkind = myRand->pick(6);
				int ruinCusNum = std::min<int>(globalInput->custCnt/2, 4 * globalCfg->ruinC_);
				sclone.perturbBaseRuin(kind, ruinCusNum, clearEPkind);
			}
			else {

				int step = myRand->pick(sclone.input.custCnt * 0.2, sclone.input.custCnt);
				sclone.patternAdjustment(step);
			}

			auto diff = EAX::getDiffCusofPb(sol, sclone);
			if (diff.size() > sol.input.custCnt * 0.2) {
				sclone.mRLLocalSearch(1, diff);
				sol = sclone;
				break;
			}
			else {
				sclone = sol;
			}

		}
	}
	#endif // 0

	return true;
}

int Goal::naMA() { // 1 代表更新了最优解 0表示没有

	int popSize = globalCfg->popSize;
	Vec<int> papborder(popSize, 0);
	std::iota(papborder.begin(), papborder.end(), 0);

	myRand->shuffleVec(papborder);

	int updateState = 0;

	for (int orderIndex = 0; orderIndex < popSize; ++orderIndex) {
	//for (int orderIndex = 0; orderIndex < 1; ++orderIndex) {
		int paIndex = papborder[orderIndex];
		int pbIndex = papborder[(orderIndex + 1) % popSize];
		Solver& pa = pool[paIndex];
		Solver& pb = pool[pbIndex];
		int before = EAX::getabCyNum(pa, pb);
		if (before <= 1) {
			perturbThePop();
			int after = EAX::getabCyNum(pa, pb);
			//println("before:", before, "after:", after, "abCyNum <= 1");
		}
		int isabState = doTwoKindEAX(pa, pb, 0);
		if (isabState == 1) {
			updateState = 1;
		}
	}
	//if (updateState == 1) {
	//	return updateState;
	//}
	for (int orderIndex = 0; orderIndex < popSize; ++orderIndex) {
		int paIndex = papborder[orderIndex];
		int pbIndex = papborder[(orderIndex + 1) % popSize];
		Solver& pa = pool[paIndex];
		Solver& pb = pool[pbIndex];
		int before = EAX::getabCyNum(pa, pb);
		if (before <= 1) {
			perturbThePop();
			int after = EAX::getabCyNum(pa, pb);
			//println("before:", before, "after:", after, "abCyNum <= 1");
		}
		int isabState = doTwoKindEAX(pa, pb, 1);
		if (isabState == 1) {
			updateState = 1;
		}
	}
	return updateState;
}

bool Goal::initPopulation() {

	//for (int initKind = 0; initKind < 5; ++initKind) {
	//
	//	Solver st;
	//	st.initSolution(initKind);
	//	println("initKind:", initKind);
	//	char a[20];
	//	snprintf(a, 20, "k%d", initKind);
	//	st.saveOutAsSintefFile(a);
	//}

	pool.reserve(globalCfg->popSize);

	//Vec<int> kset = { 1,4 };
	Vec<int> kset = { 0,1,2,3,4 };
	myRand->shuffleVec(kset);

	int ourTarget = 0;

	#if DIMACSGO
	ourTarget = dimacsRecRN;
	#else
	if (globalCfg->breakRecord) {
		ourTarget = globalInput->sintefRecRN - 1;
	}
	else {
		ourTarget = globalInput->sintefRecRN;
	}
	#endif // DIMACSGO

	Solver s0;
	s0.initSolution(kset[0]);
	s0.adjustRN(ourTarget);
	ourTarget = s0.rts.cnt;
	s0.mRLLocalSearch(0, {});
	s0.ruinLocalSearch(globalCfg->ruinC_);
	bks->updateBKS(s0);
	pool.push_back(s0);
	println("s0 inint kind:", kset[0]);

	int produceNum = globalCfg->popSize * 2;
	int i = 1;
	while (++i <= produceNum && pool.size() < globalCfg->popSize) {
		Solver st;
		st.initSolution(kset[i % kset.size()]);
		st.adjustRN(ourTarget);
		if (st.rts.cnt == ourTarget) {
			st.mRLLocalSearch(0, {});
			st.ruinLocalSearch(globalCfg->ruinC_);
			bks->updateBKS(st);
			println("s inint kind:", kset[i % kset.size()]);
			pool.push_back(st);
		}
	}

	if (pool.size() < globalCfg->popSize) {
		println("dont get enough population");
		int poolCurSize = pool.size();
		int i = 0;
		while (pool.size() < globalCfg->popSize) {
			Solver sclone = pool[i++];
			sclone.patternAdjustment(globalInput->custCnt);
			sclone.reCalRtsCostAndPen();
			pool.push_back(sclone);
		}
	}
	return true;
}

bool Goal::TwoAlgCombine() {

	initPopulation();
	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

	int contiNotDown = 1;
	//TODO[0]:先使用哪个
	int whichAlg = 0; // 0代表局部搜搜 1 代表交叉
	int popSize = globalCfg->popSize;


	#if DIMACSGO
	while (!gloalTimer.isTimeOut()) {
	#else
	while (bks->bestSolFound.RoutesCost > sintefRecRL && !gloalTimer.isTimeOut()) {
	#endif // DIMACSGO

		if (globalCfg->cmdIsopt == 1) {
			if (bks->bestSolFound.RoutesCost == dimacsRecRL) {
				break;
			}
		}

		//println("squIter:", squIter);
		if (squIter *10 > IntInf) {
			squIter = 1;
			for (auto& i:(*yearTable)) {
				for (auto& j : i) {
					j = 1;
				}
			}
		}

		int upState = 0;

		if (whichAlg == 0) {

			auto bestClone = bks->bestSolFound;
			bestClone.ruinLocalSearch(globalCfg->ruinC_);
			//bestClone.ruinLocalSearch(1);
			
			if (bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime())+" ruin ls")) {
				upState = 1;
			}
			for (auto& better : pool) {
				//better.ruinLocalSearch(globalCfg->ruinC_);
				better.ruinLocalSearch(1);
				if (bks->updateBKS(bestClone, "time:" + std::to_string(gloalTimer.getRunTime()) + " ruin ls")) {
					upState = 1;
				}
			}
		}
		else {

			upState = naMA();
		}

		if (upState == 1) {
			contiNotDown = 1;
		}
		else {
			++contiNotDown;
		}

		if(contiNotDown % 2 == 0) {
			++whichAlg;
			whichAlg %= 2;
		}

		if (contiNotDown == 100) {
			perturbThePop();
			contiNotDown = 1;
		}
	}

	saveSlnFile();
	gloalTimer.disp();

	return true;
}

bool Goal::justLocalSearch() {

	gloalTimer.setLenUseSecond(globalCfg->runTimer);
	gloalTimer.reStart();

	Solver st;
	//st.initSolution(myRand->pick(2));
	st.initSolution(0);

	st.adjustRN(dimacsRecRN);
	//st.saveOutAsSintefFile("minR");
	st.mRLLocalSearch(0, {});

	//std::fill(globalInput->P.begin(), globalInput->P.end(), 1);
	auto pBest = st;

	int contiNotDown = 1;

	Vec<int> runSize = { 1,globalCfg->ruinC_,2 * globalCfg->ruinC_ };
	int index = 0;

	//ProbControl pc(2*globalCfg->ruinC_);
	#if DIMACSGO
	while (!gloalTimer.isTimeOut()) {
	#else
	while (bks->bestSolFound.RoutesCost > sintefRecRL && !gloalTimer.isTimeOut()) {
	#endif // DIMACSGO

		//st.ruinLocalSearch(c_ + 1);
		//st.ruinLocalSearch(globalCfg->ruinC_);
		st.ruinLocalSearch(runSize[index]);

		bks->updateBKS(st);

		if (st.RoutesCost < pBest.RoutesCost) {
			//++pc.data[c_];
			pBest = st;
			contiNotDown = 1;
			index = 0;
		}
		else {
			st = pBest;
			++contiNotDown;
		}

		if (contiNotDown > 50) {
			++index;
			println("index:",index);
			if (index == runSize.size()) {
				st = pBest;
				if (myRand->pick(3) == 0) {
					int step = myRand->pick(st.input.custCnt * 0.2, st.input.custCnt * 0.4);
					st.patternAdjustment(st.input.custCnt);
					int abcyNum = EAX::getabCyNum(st, pBest);
					println("st.patternAdjustment,cyNum", abcyNum);
				}

			}
			index %= runSize.size();
			contiNotDown = 1;
		}
	}
	saveSlnFile();
	return true;
}

bool Goal::saveSlnFile() {

	Input& input = *globalInput;

	MyString ms;
	// 输出 tm 结构的各个组成部分
	//std::string day = /*ms.LL_str(d.year) + */ms.LL_str(d.month) + ms.LL_str(d.day);

	#if DIMACSGO
	std::string type = "[dim]";
	#else
	std::string type = "[sin]";
	#endif // DIMACSGO

	std::string path = type + __DATE__;
	path += std::string(1, '_') + __TIME__;

	for (auto& c : path) {
		if (c == ' ' || c == ':') {
			c = '_';
		}
	}

	std::string pwe0 = ms.int_str(globalCfg->Pwei0);
	std::string pwe1 = ms.int_str(globalCfg->Pwei1);
	std::string minKmax = ms.int_str(globalCfg->minKmax);
	std::string maxKmax = ms.int_str(globalCfg->maxKmax);

	std::ofstream rgbData;
	std::string wrPath = globalCfg->outputPath + "_" + path + ".csv";

	bool isGood = false;
	{
		std::ifstream f(wrPath.c_str());
		isGood = f.good();
	}

	rgbData.open(wrPath, std::ios::app | std::ios::out);

	if (!rgbData) {
		println("output file open errno");
		return false;
	}
	if (!isGood) {
		//rgbData << "ins,lyhrn,sinrn,dimrn,lyhrl,dimRL,rate, sinRL,rate,naRL,rate,time,epsize,minep,ptw,pc,rts,seed" << std::endl;
		rgbData << "ins,isopt,lyhrn,sinrn,dimrn,narn,lyhrl,dimRL,rate,lkhrl,rate, sinRL,rate,naRL,rate,time,epsize,minep,ptw,pc,rts,seed" << std::endl;
	}

	auto& sol = bks->bestSolFound;

	rgbData << input.example << ",";
	rgbData << globalCfg->cmdIsopt << ",";
	rgbData << sol.rts.size() << ",";
	rgbData << sintefRecRN << ",";
	rgbData << dimacsRecRN << ",";
	rgbData << naRecRL << ",";

	auto state = sol.verify();
	rgbData << state << ",";

	rgbData << dimacsRecRL << ",";
	rgbData << double((double)(state - dimacsRecRL) / dimacsRecRL) * 100 << ",";
	
	rgbData << globalCfg->cmdDimacsRL << ",";
	rgbData << double((double)(state - globalCfg->cmdDimacsRL) 
		/ globalCfg->cmdDimacsRL) * 100 << ",";

	rgbData << sintefRecRL << ",";
	rgbData << double((double)(state - sintefRecRL) / sintefRecRL) * 100 << ",";

	rgbData << naRecRL << ",";
	rgbData << double((double)(state - naRecRL) / naRecRL) * 100 << ",";

	//TODO[-1]:timer
	rgbData << gloalTimer.getRunTime() << ",";

	//rgbData << sol.EPr.rCustCnt << ",";
	//rgbData << sol.minEPcus << ",";
	//rgbData << sol.PtwNoWei << ",";
	//rgbData << sol.Pc << ",";

	for (std::size_t i = 0; i < sol.rts.cnt; ++i) {
		rgbData << "Route  " << i + 1 << " : ";
		Route& r = sol.rts[i];
		auto cusArr = sol.rPutCusInve(r);
		for (int c: cusArr) {
			rgbData << c << " ";
		}
		rgbData << "| ";
	}

	rgbData << ",";
	rgbData << globalCfg->seed;

	rgbData << std::endl;
	rgbData.close();

	return true;
}

bool Goal::test() {

	Solver st;
	//st.initSolution(myRand->pick(2));
	st.initSolution(5);

	return true;
}

}
