
#include <string>
#include <istream>

#include "Problem.h"
#include "Utility.h"

namespace hust {

Input::Input() {
	initInput();
}

bool Input::initInput() {

	readDimacsInstance(globalCfg->inputPath);
	//readDimacsBKS();

	P = Vec<int>(custCnt + 1, 1);
	for (int i = custCnt + 1; i < datas.size(); ++i) {
		datas[i] = datas[0];
		datas[i].CUSTNO = i;
	}

	disOf = Vec< Vec<DisType> >
		(custCnt + 1, Vec<DisType>(custCnt + 1, 0));

	/*
		sqrt函数有三种形式
		double sqrt(double x);
		float sqrtf(float x);
		long double sqrtl(long double x);
		float精度最低，double较高，long double精度最高
	*/

	for (int i = 0; i <= custCnt + 1; ++i) {
		for (int j = i + 1; j <= custCnt; ++j) {
			Data& d1 = datas[i];
			Data& d2 = datas[j];
			double dis = sqrt((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
				+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));
			//TODO[lyh][000]:dimacs的距离计算不同！！！！
			//disOf[j][i] = disOf[i][j] = dis;
			#if DIMACSGO
			//disOf[j][i] = disOf[i][j] = round(dis);
			disOf[j][i] = disOf[i][j] = dis;
			#else
			disOf[j][i] = disOf[i][j] = round(dis);
			//disOf[j][i] = disOf[i][j] = ceil(dis);
			#endif // DIMACSGO
		}
	}

	auto canlinkDir = [&](int v, int w) ->bool {

		DisType av = disOf[0][v];
		DisType aw = av + datas[v].SERVICETIME + disOf[v][w];
		DisType ptw = std::max<DisType>(0, aw - datas[w].DUEDATE);
		DisType an = aw + datas[w].SERVICETIME + disOf[w][0];
		ptw += std::max<DisType>(0, an - datas[0].DUEDATE);
		return ptw == 0;
	};

	auto canLinkNode = [&](int v, int w) ->bool {

		if (!canlinkDir(v, w) && !canlinkDir(w, v)) {
			return false;
		}
		return true;
	};

	sectorClose = Vec< Vec<int>>(custCnt + 1, Vec<int>(0));
	for (int v = 0; v <= custCnt; ++v) {
		Vec<int> nums;
		nums.reserve(custCnt + 1);
		for (int pos = 0; pos <= custCnt; ++pos) {
			if (pos != v) {
				nums.push_back(pos);
			}
		}
		auto cmp = [&](const int a, const int b) {
			return abs(datas[a].polarAngle - datas[v].polarAngle)
				< abs(datas[b].polarAngle - datas[v].polarAngle);
		};
		std::sort(nums.begin(), nums.end(), cmp);
		sectorClose[v] = std::move(nums);
	}

	allCloseOf = Vec< Vec<int>>(custCnt + 1, Vec<int>(0));

	for (int v = 0; v <= custCnt; ++v) {

		Vec<int> nums;
		nums.reserve(custCnt + 1);

		for (int pos = 0; pos <= custCnt; ++pos) {
			if (pos != v) {
				nums.push_back(pos);
			}
		}

		auto cmp = [&](const int a, const int b) {

			return disOf[a][v] < disOf[b][v];

			//int aLinkv = canLinkNode(a, v);
			//int bLinkv = canLinkNode(b, v);
			//if ( (aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
			//	return disOf[a][v] < disOf[b][v];
			//}
			//else {
			//	return aLinkv ? true : false;
			//}
			//return false;
		};

		std::sort(nums.begin(), nums.end(), cmp);

		allCloseOf[v] = std::move(nums);
	}

	Vec<Vec<int>> jIsxthcloseOf;
	//表示v的地理位置距离作为排序依据 jIsxthcloseOf[v][w],w是v的第几近

	Vec<Vec<int>> iInNeicloseOf;

	jIsxthcloseOf = Vec< Vec<int> >
		(custCnt + 1, Vec<int>(custCnt + 1, -1));

	for (int i = 0; i <= custCnt; ++i) {
		for (int j = 0; j < custCnt; ++j) {
			jIsxthcloseOf[i][allCloseOf[i][j]] = j;
		}
	}

	iInNeicloseOfUnionNeiCloseOfI = Vec< Vec<int> >(custCnt + 1);

	int deNeiSize = globalCfg->outNeiSize;
	deNeiSize = std::min(custCnt - 1, deNeiSize);

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
		std::sort(addSTclose[v].begin(), addSTclose[v].end(), cmp);
	}

	addSTJIsxthcloseOf = Vec< Vec<int>>
		(custCnt + 1, Vec<int>(custCnt + 1, -1));

	for (int v = 0; v <= custCnt; ++v) {
		for (std::size_t wpos = 0; wpos < addSTclose[v].size(); ++wpos) {
			int w = addSTclose[v][wpos];
			addSTJIsxthcloseOf[v][w] = wpos;
		}
	}

	return true;
}

bool Input::readDimacsInstance(std::string& instanciaPath) {

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
	fscanf(file, "%d %lld\n", &this->vehicleCnt, &this->Q);
	fscanf(file, "%*[^\n]\n");
	fscanf(file, "%*[^\n]\n");

	this->Q *= disMul;
	std::string line = "";
	size_t len = 0;
	long read = 0;

	int index = 0;
	int id = -1, coordx = -1, coordy = -1, demand = -1;
	int ready_time = -1, due_date = -1, service_time = -1;
	int readArgNum = 0;
	while ((readArgNum = fscanf(file, "%d %d %d %d %d %d %d\n", &id, &coordx, &coordy, &demand, &ready_time, &due_date, &service_time)) == 7) {

		this->datas[index].CUSTNO = id;
		this->datas[index].XCOORD = coordx * disMul;
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
	custCnt = index - 1;
	fclose(file);
	return true;
}

}
