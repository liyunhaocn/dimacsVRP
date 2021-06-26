#ifndef vrptwNew_PROBLEM_H
#define vrptwNew_PROBLEM_H

using namespace std;

#include <algorithm>
#include <fstream>
#include "./Configuration.h"
#include "./Flag.h"

namespace vrptwNew {

struct CircleSector
{
	int start;
	int end;

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

	LL CUSTNO;
	LL XCOORD;
	LL YCOORD;
	LL DEMAND;
	LL READYTIME;
	LL DUEDATE;
	LL SERVICETIME;
	LL polarAngle;
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
		id = -1;
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

	map<string, int> bestRec;
	map<string, int> nagataRecs;
	map<string, double> sintefMinlen;

	Configuration cfg;

	string example = "";
	int custCnt = 0;
	DisType Q = 0;
	int vehicleCnt = 0;
	vector<Data> datas;
	int sintefRecRN = 0;
	DisType sintefRecRL = 0;
	int sumQToQ = 0;
	int nagataRec = 0;

#if DISDOUBLE
	int mul = 1;
#else
	int mul = 100000;

#endif // DISDOUBLE

	vector<vector<DisType>> disOf;

	// disOf[v][w] 表示w和v之间的距离
	vector<vector<int>> allCloseOf;
	// input.allCloseOf[v][wpos] 表示v的地理位置第wpos近的点
	vector<vector<int>> addSTclose;
	// input.addSTclose[v][wpos] 表示v的地理位置加上v的服务时间第wpos近的点
	vector<vector<int>> addSTJIsxthcloseOf;
	//表示v的地理位置加上v的服务时间作为排序依据 input.addSTJIsxthcloseOf[v][w],w是v的第几近

	vector<vector<int>> jIsxthcloseOf;
	//表示v的地理位置距离作为排序依据 jIsxthcloseOf[v][w],w是v的第几近

	vector<vector<int>> iInNeicloseOf;

	vector<vector<vector<int>>> iInNeicloseOfUnionNeiCloseOfI;

	int polarIndex = 0;
	int polarCaseNum = 8;
	vector<int> arrAngleNum = { 8,8,8,8,8,8,8,8 }; // = 8;
	vector<int> arrRingNum = { 3,3,3,3,3,3,3,3 };  // = 4;
	vector<double> arrInitAngles = { 0,PI_32,PI_32 * 2,PI_32 * 3,PI_32 * 4
		,PI_32 * 5,PI_32 * 6,PI_32 * 7 };
	vector<vector<vector<int>>> sectorGroups;
	vector<vector<int>> vInsectorGroups;

	struct PolarCo { // 极坐标
		int P;
		double theta;
	};

	vector<PolarCo> vPos;

	Input(Environment& env,Configuration& cfg):cfg(cfg) {

		getSintefBestRec(env);
		getNatagaRec();

		initInput(env);
		//initPolar();
	}

	double cartesianToPolar(LL x, LL y) {

		double angle = atan2(y, x);
		if (angle < 0) {
			angle += 2 * PI;
		}
		return angle;
	}

	bool initPolar() {

		/*sectorGroups = vector<vector<vector<LL>>>(polarCaseNum);
		vInsectorGroups = vector<vector<LL>>(polarCaseNum);*/

		vPos = vector<PolarCo>(custCnt + 1);
		for (int v = 1; v <= custCnt; v++) {
			vPos[v].P = disOf[0][v];
			vPos[v].theta = cartesianToPolar(datas[v].XCOORD - datas[0].XCOORD, datas[v].YCOORD - datas[0].YCOORD);
		}


		for (int c = 0; c < polarCaseNum; c++) {
			int angleNum = arrAngleNum[c];
			int ringNum = arrRingNum[c];
			double initAngle = arrInitAngles[c];

			vector<vector<int>> sectorGroup(angleNum * ringNum);
			vector<int> vInsectorGroup(custCnt + 1);

			for (int v = 1; v <= custCnt; v++) {
				vPos[v].P = disOf[0][v];
				int angleNO = min((double)angleNum - 1, (vPos[v].theta - initAngle) / (2 * PI / angleNum));
				int PNO = min(ringNum - 1, jIsxthcloseOf[0][v] / (custCnt / ringNum));

				sectorGroup[PNO * angleNum + angleNO].push_back(v);
				vInsectorGroup[v] = PNO * angleNum + angleNO;
			}
			/*LL cnt = 0;
			for (int i = 0; i < sectorGroup.size(); i++) {
				cnt += sectorGroup[i].size();
				for (int j = 0; j < sectorGroup[i].size(); j++) {
					cout << sectorGroup[i][j] << " ";
				}
				cout << endl;
			}
			debug(cnt)*/
			sectorGroups.push_back(sectorGroup);
			vInsectorGroups.push_back(vInsectorGroup);
		}

		return true;
	}

	bool initInput(Environment env) {

		if (env.example == "") {
			this->example = "C1_2_1";
		}
		else {
			this->example = env.example;
		}

		MyString ms;
		vector<string> mssp = ms.split(example, "_");
		string customersNum = mssp[1] + "00";

		this->custCnt = ms.str_LL(customersNum);
		//10 11 20
		
		ifstream fin(env.inputPath);

		if (!fin.good()) {
			Log(Log::Level::Error) << "input file open error!" << endl;
			Log(Log::Level::Error) << env.inputPath << endl;
			return false;
		}

		string str;
		LL line = 0;

		datas = vector<Data>(custCnt + 401);

		line = 0;
		while (getline(fin, str)) {

			line++;
			istringstream is(str);

			if (line == 5) {
				is >> vehicleCnt;
				is >> Q;
				Q *= mul;
			}

			if (line < 10) {
				continue;
			}

			Data data0;
			
			is >> data0.CUSTNO;
			is >> data0.XCOORD;
			is >> data0.YCOORD;
			is >> data0.DEMAND;
			is >> data0.READYTIME;
			is >> data0.DUEDATE;
			is >> data0.SERVICETIME;

			data0.XCOORD *= mul;
			data0.YCOORD *= mul;
			data0.DEMAND *= mul;
			data0.READYTIME *= mul;
			data0.DUEDATE *= mul;
			data0.SERVICETIME *= mul;

			if (line == 10) {
				
				datas[line - 10] = data0;
				for (int i = custCnt + 1; i <= custCnt + 400; i++) {
					data0.CUSTNO = i;
					datas[i] = data0;
				}
			}
			else {
				datas[line - 10] = data0;
				auto& dt = datas[line - 10];
				datas[line - 10].polarAngle = CircleSector::positive_mod
				(32768. * atan2(dt.YCOORD - datas[0].YCOORD, dt.XCOORD - datas[0].XCOORD) / PI);
			}
		}

		fin.clear();
		fin.close();

		disOf = vector< vector<DisType> >
			(custCnt + 2, vector<DisType>(custCnt + 2, 0));

		/*
			sqrt函数有三种形式
			double sqrt(double x);
			float sqrtf(float x);
			long double sqrtl(long double x);
			float精度最低，double较高，long double精度最高
		*/

		for (int i = 0; i <= custCnt + 1; i++) {
			for (int j = i + 1; j <= custCnt + 1; j++) {
				Data& d1 = datas[i];
				Data& d2 = datas[j];
				double dis = sqrtl((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
					+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));

#if DISDOUBLE
				disOf[j][i] = disOf[i][j] = dis;
#else
				disOf[j][i] = disOf[i][j] = ceil(dis);
#endif // DISDOUBLE

			}
		}

		allCloseOf = vector< vector<int>>(custCnt + 1, vector<int>(0));

		auto canlinkDir = [&](int v, int w) {

			DisType av = disOf[0][v];
			DisType aw = av + datas[v].SERVICETIME + disOf[v][w];
			DisType ptw = max(0, aw - datas[w].DUEDATE);
			DisType an = aw + datas[w].SERVICETIME + disOf[w][0];
			ptw += max(0, an - datas[0].DUEDATE);
			return ptw == 0;
		};

		auto canLinkNode = [&](int v, int w) {
			
			if (!canlinkDir(v,w) && !canlinkDir(w,v)) {
				return false;
			}
			return true;
		};

		for (int v = 0; v <= custCnt; v++) {

			vector<int> nums;
			nums.reserve(custCnt + 1);

			for (int pos = 0; pos <= custCnt; pos++) {
				if (pos != v) {
					nums.push_back(pos);
				}
			}

			auto cmp = [&](const LL a, const LL b) {
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
			
			auto cmp1 = [&](const LL a, const LL b) {
				return disOf[a][v] < disOf[b][v];
			};

			/*sort(nums.begin(), nums.end(), cmp1);
			auto cnums = nums;*/

			sort(nums.begin(), nums.end(), cmp);

			/*vector<int> arr;
			for (int i = 0; i < cfg.outNeiSize[0]; ++i) {
				if (!canLinkNode(v, cnums[i])) {
					arr.push_back(i);
				}
			}
			cout<<v<<"： " << arr.size() << ": ";
			for (auto i : arr) {
				cout << i << " ";
			}
			cout << endl;*/

			allCloseOf[v] = nums;
		}

		jIsxthcloseOf = vector< vector<int> >
			(custCnt + 1, vector<int>(custCnt + 1, -1));

		for (int i = 0; i <= custCnt; i++) {
			for (int j = 0; j < custCnt; j++) {
				jIsxthcloseOf[i][allCloseOf[i][j]] = j;
			}
		}

		iInNeicloseOfUnionNeiCloseOfI = vector <vector< vector<int> > >(cfg.outNeiSize.size());

		vector<int> devs = cfg.outNeiSize;

		for (int i = 0; i < devs.size(); i++) {

			LL deNeiSize = devs[i];

			iInNeicloseOf = vector< vector<int> >
				(custCnt + 1, vector<int>());

			for (int i = 0; i < custCnt + 1; i++) {
				iInNeicloseOf[i].reserve(custCnt);
			}

			for (int v = 0; v <= custCnt; v++) {
				for (int wpos = 0; wpos < deNeiSize; wpos++) {
					LL w = allCloseOf[v][wpos];
					iInNeicloseOf[w].push_back(v);
				}
			}

			iInNeicloseOfUnionNeiCloseOfI[i] 
				= vector<vector<int>>(custCnt + 1);

			for (int v = 0; v <= custCnt; v++) {

				iInNeicloseOfUnionNeiCloseOfI[i][v] = vector<int>
					(allCloseOf[v].begin(), allCloseOf[v].begin() + deNeiSize);

				for (int wpos = 0; wpos < iInNeicloseOf[v].size(); wpos++) {

					LL w = iInNeicloseOf[v][wpos];
					if (jIsxthcloseOf[v][w] >= deNeiSize) {
						iInNeicloseOfUnionNeiCloseOfI[i][v].push_back(w);
					}
				}
			}
		}

		addSTclose = vector< vector<int>>
			(custCnt + 1, vector<int>());

		for (int v = 0; v <= custCnt; v++) {
			addSTclose[v].reserve(custCnt);
			for (int w = 0; w <= custCnt; w++) {
				if (w == v) {
					continue;
				}
				addSTclose[v].push_back(w);
			}

			auto cmp = [&](LL a, LL b) {

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

		addSTJIsxthcloseOf = vector< vector<int>>
			(custCnt + 1, vector<int>(custCnt + 1, -1));

		for (int v = 0; v <= custCnt; v++) {
			for (int wpos = 0; wpos < addSTclose[v].size(); wpos++) {
				LL w = addSTclose[v][wpos];
				addSTJIsxthcloseOf[v][w] = wpos;
			}
		}

		sintefRecRN = bestRec[example];

		sintefRecRL = sintefMinlen[example]* mul;

		nagataRec = nagataRecs[mssp[0] + "_" + mssp[1] + "_"];
		sumQToQ = getTheMinRouteNum();

		Log(Log::Level::Warning) <<"path: " << env.inputPath << endl;
		Log(Log::Level::Warning) <<"sintefRecRN: " << sintefRecRN << endl;
		Log(Log::Level::Warning) <<"sintefRecRL: " << sintefRecRL << endl;
		Log(Log::Level::Warning) <<"nagataRec: " << nagataRec << endl;
		Log(Log::Level::Warning) <<"sumQToQ: " << sumQToQ << endl;
		Log(Log::Level::Warning) <<"Q: " << Q << endl;
			
		return true;
	}

	// 得到容量下界：sumQ/Q
	LL getTheMinRouteNum() {
		double sumQ = 0;
		for (int i = 1; i <= custCnt; i++) {
			sumQ += (double)datas[i].DEMAND;
		}
		LL ret = ceil(sumQ / (double)Q);

		return ret;
	}

	bool getNatagaRec() {

		nagataRecs["R1_2_"] = 18;
		nagataRecs["R2_2_"] = 4;
		nagataRecs["RC1_2_"] = 18;
		nagataRecs["RC2_2_"] = 4;
		nagataRecs["C1_2_"] = 18;
		nagataRecs["C2_2_"] = 6;

		nagataRecs["R1_4_"] = 36;
		nagataRecs["R2_4_"] = 8;
		nagataRecs["RC1_4_"] = 36;
		nagataRecs["RC2_4_"] = 8;
		nagataRecs["C1_4_"] = 37;
		nagataRecs["C2_4_"] = 11;

		nagataRecs["R1_6_"] = 54;
		nagataRecs["R2_6_"] = 11;
		nagataRecs["RC1_6_"] = 55;
		nagataRecs["RC2_6_"] = 11;
		nagataRecs["C1_6_"] = 57;
		nagataRecs["C2_6_"] = 17;

		nagataRecs["R1_8_"] = 72;
		nagataRecs["R2_8_"] = 15;
		nagataRecs["RC1_8_"] = 72;
		nagataRecs["RC2_8_"] = 15;
		nagataRecs["C1_8_"] = 74;
		nagataRecs["C2_8_"] = 23;

		nagataRecs["R1_10_"] = 91;
		nagataRecs["R2_10_"] = 19;
		nagataRecs["RC1_10_"] = 90;
		nagataRecs["RC2_10_"] = 18;
		nagataRecs["C1_10_"] = 93;
		nagataRecs["C2_10_"] = 28;

		return true;
	}

	bool getSintefBestRec(Environment& env) {

		string file = env.sinRecPath;

		string ins;
		LL veh;
		double dis;

		ifstream fin(file);
		
		if (!fin.good()) {
			Log(Log::Warning) << file << " is not exist!" << endl;
			return false;
		}

		string str;

		LL mul = 10000;

		do {

			fin >> ins;
			fin >> veh;
			fin >> dis;
			bestRec[ins] = veh;

			sintefMinlen[ins] = dis;

		} while (getline(fin, str));

		fin.clear();
		fin.close();
		return true;
	}

	~Input() {

		disOf.clear();

		allCloseOf.clear();

		addSTclose.clear();

		addSTJIsxthcloseOf.clear();

		jIsxthcloseOf.clear();

		iInNeicloseOfUnionNeiCloseOfI.clear();
	}
};

struct Output
{
	vector<vector<int>> rts;
	vector<int> EP;
	LL PtwNoWei;
	LL Pc;
	int minEP;
	DisType state;
	double runTime;
	vector<bool> HArr;

};

bool solveCommandLine(int argc, char* argv[], Configuration& cfg,Environment& env) {
	
	MyString ms;
	if (argc >= 2) {
		cfg.breakRecord = ms.str_LL(argv[1]);
		debug(cfg.breakRecord)
	}

	if (argc >= 3) {
		string example = argv[2];
		env.setExample(example);
		debug(env.example)
	}

	if (argc >= 4) {
		cfg.runTimer = ms.str_LL(argv[3]);
		debug(cfg.runTimer)
	}

	if (argc >= 5) {
		env.seed = ms.str_LL(argv[4]);
		debug(env.seed)
	}

	debug(cfg.Pwei0);
	debug(cfg.Pwei1);
	debug(cfg.minKmax);
	debug(cfg.maxKmax);

	return true;
}

bool saveSln(Input& input, Output& output,Configuration& cfg,Environment& env) {

	DateTime d(time(0));
	MyString ms;
	// 输出 tm 结构的各个组成部分
	string day = /*ms.LL_str(d.year) + */ms.LL_str(d.month) + ms.LL_str(d.day);
	string pwe0 = ms.LL_str(cfg.Pwei0);
	string pwe1 = ms.LL_str(cfg.Pwei1);
	string minKmax = ms.LL_str(cfg.minKmax);
	string maxKmax = ms.LL_str(cfg.maxKmax);

	string type = output.rts.size() < input.sintefRecRN ? "br" : "Ej";

	std::ofstream rgbData;
	string wrPath = env.outputPath 
		+ type + day
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

	rgbData << input.example << ",";
	rgbData << "b" << input.sumQToQ << ",";
	rgbData << "sinRN " << input.sintefRecRN << ",";
	rgbData << output.rts.size() << ",";
	rgbData << "sinRL " << input.sintefRecRL << ",";
	rgbData << output.state << ",";

	rgbData << output.runTime << "s ,";
	rgbData << "EPs " << output.EP.size() << ": ";

	for (int i = 0; i < output.EP.size(); i++) {
		rgbData << output.EP[i]<< "|";
		
	}
	rgbData << ",";

	rgbData << "minEP " << output.minEP << ",";
	rgbData << "Ptw " << output.PtwNoWei << ",";
	rgbData << "Pc " << output.Pc << ",";
	for (int i = 0; i < output.rts.size(); i++) {
		rgbData << "Route  " << i + 1 << " : ";
		for (int j = 0; j < output.rts[i].size(); j++) {
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

