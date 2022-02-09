
#include <string>
#include <istream>
#include <functional>

#include "Problem.h"
#include "Utility.h"

namespace hust {

Input::Input() {
	initInput();
}

void Input::initDetail() {

	for (int v = 0; v <= custCnt; ++v) {

		auto cmp = [&](const int a, const int b) {
			return abs(datas[a].polarAngle - datas[v].polarAngle)
				< abs(datas[b].polarAngle - datas[v].polarAngle);
		};
		//TODO[-1]:这里的排序比较浪费时间，去掉可以节省一般的初始化时间
		auto& nums = sectorClose[v];
		std::sort(nums.begin(), nums.end(), cmp);
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

	for (int v = 0; v <= custCnt; ++v) {

		auto cmp = [&](const int a, const int b) {

			//TODO[-1] 这里的比较方式进行了修改
			//return disOf[v][a] < disOf[v][b];
			//return disOf[v][a] + datas[a].SERVICETIME <
			//	disOf[v][b] + datas[b].SERVICETIME;

			if (disOf[a][v] == disOf[b][v]) {
				return datas[a].DUEDATE < datas[b].DUEDATE;
			}
			else {
				return disOf[a][v] < disOf[b][v];
			}
			return true;

			//int aLinkv = canLinkNode(a, v);
			//int bLinkv = canLinkNode(b, v);
			//if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
			//	return disOf[v][a] + datas[a].SERVICETIME <
			//		disOf[v][b] + datas[b].SERVICETIME;
			//}
			//else {
			//	return aLinkv ? true : false;
			//}

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

	iInNeicloseOfUnionNeiCloseOfI = Vec< Vec<int> >(custCnt + 1);
	int deNeiSize = globalCfg->outNeiSize;
	deNeiSize = std::min(custCnt - 1, deNeiSize);

	auto iInNeicloseOf = Vec< Vec<int> >
		(custCnt + 1, Vec<int>());
	for (int i = 0; i < custCnt + 1; ++i) {
		iInNeicloseOf[i].reserve(custCnt);
	}
	for (int v = 0; v <= custCnt; ++v) {
		for (int wpos = 0; wpos < deNeiSize; ++wpos) {
			int w = addSTclose[v][wpos];
			iInNeicloseOf[w].push_back(v);
		}
	}

	auto jIsxthcloseOf = Vec< Vec<int> >
		(custCnt + 1, Vec<int>(custCnt + 1, -1));

	for (int i = 0; i <= custCnt; ++i) {
		for (int j = 0; j < custCnt; ++j) {
			jIsxthcloseOf[i][addSTclose[i][j]] = j;
		}
	}

	iInNeicloseOfUnionNeiCloseOfI
		= Vec<Vec<int>>(custCnt + 1);
	for (std::size_t v = 0; v <= custCnt; ++v) {

		iInNeicloseOfUnionNeiCloseOfI[v] = Vec<int>
			(addSTclose[v].begin(), addSTclose[v].begin() + deNeiSize);

		for (std::size_t wpos = 0; wpos < iInNeicloseOf[v].size(); ++wpos) {

			int w = iInNeicloseOf[v][wpos];
			if (jIsxthcloseOf[v][w] >= deNeiSize) {
				iInNeicloseOfUnionNeiCloseOfI[v].push_back(w);
			}
		}
	}
}

bool Input::initInput() {

	readDimacsInstance(globalCfg->inputPath);
	//readDimacsBKS();

	P = Vec<int>(custCnt + 1, 1);
	if (datas.size() < custCnt * 3 + 3) {
		datas.resize(custCnt * 3 + 3);
	}
	for (int i = custCnt + 1; i < datas.size(); ++i) {
		datas[i] = datas[0];
		datas[i].CUSTNO = i;
	}

	disOf = Vec< Vec<DisType> >
		(custCnt + 1, Vec<DisType>(custCnt + 1, 0));


	double sumq = 0;
	for (int i = 1; i <= custCnt; ++i) {
		sumq += datas[i].DEMAND;
	}
	Qbound = ceil(double(sumq) / Q);
	Qbound = std::max<int>(Qbound, 2);

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

			disOf[j][i] = disOf[i][j] = dis;
		}
	}

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
		//TODO[-1]:这里的排序比较浪费时间，去掉可以节省一般的初始化时间
		//std::sort(nums.begin(), nums.end(), cmp);
		sectorClose[v] = std::move(nums);
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

			//TODO[-1] 这里的比较方式进行了修改
			//return disOf[v][a] < disOf[v][b];
			//return disOf[v][a] + datas[a].SERVICETIME <
			//	disOf[v][b] + datas[b].SERVICETIME;

			if (disOf[a][v] == disOf[b][v]) {
				return datas[a].DUEDATE < datas[b].DUEDATE;
			}
			else {
				return disOf[a][v] < disOf[b][v];
			}
			return true;

			//int aLinkv = canLinkNode(a, v);
			//int bLinkv = canLinkNode(b, v);
			//if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
			//	return disOf[v][a] + datas[a].SERVICETIME <
			//		disOf[v][b] + datas[b].SERVICETIME;
			//}
			//else {
			//	return aLinkv ? true : false;
			//}

		};
		//std::sort(addSTclose[v].begin(), addSTclose[v].end(), cmp);

		getTopKmin(addSTclose[v],globalCfg->mRLLocalSearchRange[1],cmp);
		std::sort(addSTclose[v].begin(), addSTclose[v].begin() + globalCfg->mRLLocalSearchRange[1], cmp);

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
	

	fscanf(file, "%s\n", name);
	this->example = std::string(name);
	fscanf(file, "%*[^\n]\n");
	fscanf(file, "%*[^\n]\n");
	fscanf(file, "%d %lld\n", &this->vehicleCnt, &this->Q);
	fscanf(file, "%*[^\n]\n");
	fscanf(file, "%*[^\n]\n");

	this->Q *= disMul;
	std::string line = "";

	this->datas = Vec<Data>(303);

	int index = 0;
	int id = -1, coordx = -1, coordy = -1, demand = -1;
	int ready_time = -1, due_date = -1, service_time = -1;
	int readArgNum = 0;
	while ((readArgNum = fscanf(file, "%d %d %d %d %d %d %d\n", &id, &coordx, &coordy, &demand, &ready_time, &due_date, &service_time)) == 7) {

		if (index >= datas.size()) {
			int newSize = datas.size() + datas.size() / 2;
			datas.resize(newSize);
		}

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

int Input::partition(Vec<int>& arr, int start, int end, std::function<bool(int, int)>cmp) {
	//int index = ( [start, end] (void)  //我试图利用随机法，但是这不是快排，外部输入不能保证end-start!=0，所以可能发生除零异常
	//              {return random()%(end-start)+start;} )(); 
	//std::swap(arr[start], arr[end]);

	int small = start - 1;
	for (int index = start; index < end; ++index) {
		if (cmp(arr[index] , arr[end])) {
			++small;
			if (small != index)
				std::swap(arr[small], arr[index]);
		}
	}
	++small;
	std::swap(arr[small], arr[end]);
	return small;
}

void Input::getTopKmin(Vec<int>& input, int k,std::function<bool(int,int)>cmp) {

	const int size = input.size();

	if (size == 0 || k <= 0 || k > size)
		return;
	if (k == size)
		return;

	int start = 0;
	int end = size - 1;
	int index = partition(input, start, end, cmp);
	while (index != k - 1) {
		if (index > k - 1)
			end = index - 1;
		else
			start = index + 1;
		index = partition(input, start, end, cmp);
	}
	return;
}

}
