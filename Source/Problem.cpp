
#include <string>
#include <istream>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Problem.h"
#include "Util_Common.h"

namespace hust {

struct InstanceData {
	int minRouteNumber = 0;
	double minRouteDistance = 0;
	int isOptimal = 0;
	InstanceData() :minRouteNumber(0), minRouteDistance(0), isOptimal(0) {}
	InstanceData(int routeNumber, double routeDistance, int isOptimal) :
		minRouteNumber(routeNumber), minRouteDistance(routeDistance), isOptimal(isOptimal) {}
};

InstanceData getInstanceDataFromFile(std::string path,std::string instanceName) {

	std::ifstream inStream(path, std::ios::in);
	if (!inStream)
	{
		std::cout << "file to open the file" << path << std::endl;
		exit(1);
	}

	std::string line;

	while (getline(inStream, line)) {
		
		std::stringstream sin(line); 
		std::string item;

		getline(sin, item, ',');
		
		std::string instanceNameInLine = item;
		strtool::trim(instanceName);

		getline(sin, item, ',');
		//std::cout << atoi(item.c_str()) << " ";
		int routeNumber = atoi(item.c_str());

		getline(sin, item, ',');
		//std:: cout << atof(item.c_str()) << " ";
		double routeDistance = atof(item.c_str());

		getline(sin, item); 
		//std::cout << atoi(item.c_str()) << std::endl;
		int isOptimal = atoi(item.c_str());
		if (instanceNameInLine == instanceName) {
			inStream.close();
			return InstanceData{ routeNumber,routeDistance,isOptimal };
		}
		
	}
	inStream.close();

	return InstanceData{};
}

Input::Input(CommandLine* commandLine,AlgorithmParameters* aps,RandomTools* randomTools,Timer*timer):
	commandLine(commandLine),aps(aps),randomTools(randomTools),timer(timer) {

	//std::string example = aps->inputPath;
	//int txtPos = example.find(".txt");
	//example = example.substr(0, txtPos);
	//int slich = example.find("Homberger/");
	//if (slich != -1) {
	//	slich += 9;
	//}
	//else {
	//	slich = example.find("Solomon/");
	//	slich += 7;
	//}
	//example = example.substr(slich + 1);

	initInput();
	initDetail();
}

void Input::initDetail() {

	for (int v = 0; v <= custCnt; ++v) {

		auto cmp = [&](const int a, const int b) {
			return abs(datas[a].polarAngle - datas[v].polarAngle)
				< abs(datas[b].polarAngle - datas[v].polarAngle);
		};
		std::sort(sectorClose[v], sectorClose[v]+ sectorClose.size2(), cmp);
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

			//if (disOf[a][v] == disOf[b][v]) {
			//	return datas[a].DUEDATE < datas[b].DUEDATE;
			//}
			//else {
			//	return disOf[a][v] < disOf[b][v];
			//}
			//return true;

			//int aLinkv = canLinkNode(a, v);
			//int bLinkv = canLinkNode(b, v);
			//if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
			//	return disOf[v][a] + datas[a].SERVICETIME <
			//		disOf[v][b] + datas[b].SERVICETIME;
			//}
			//else {
			//	return aLinkv ? true : false;
			//}
			
			int aLinkv = canLinkNode(a, v);
			int bLinkv = canLinkNode(b, v);
			if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
				return disOf[v][a] < disOf[v][b] ;
			}
			else {
				return aLinkv ? true : false;
			}

		};

		std::sort(addSTclose[v], addSTclose[v]+addSTclose.size2(), cmp);
	}

	addSTJIsxthcloseOf = util::Array2D<int> (custCnt + 1, custCnt + 1, -1);

	for (int v = 0; v <= custCnt; ++v) {
		for (int wpos = 0; wpos < addSTclose.size2(); ++wpos) {
			int w = addSTclose[v][wpos];
			addSTJIsxthcloseOf[v][w] = wpos;
		}
	}
	
	int deNeiSize = aps->outNeiSize;
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

	iInNeicloseOfUnionNeiCloseOfI = Vec< Vec<int> >(custCnt + 1);

	for (int v = 0; v <= custCnt; ++v) {

		iInNeicloseOfUnionNeiCloseOfI[v] = Vec<int>
			(addSTclose[v], addSTclose[v] + deNeiSize);

		for (int w : iInNeicloseOf[v]) {
			if (addSTJIsxthcloseOf[v][w] >= deNeiSize) {
				iInNeicloseOfUnionNeiCloseOfI[v].push_back(w);
			}
		}
	}

	const auto& d15 = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromDIMACSDay15.csv",example);
	const auto& lkh = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromDIMACSLKH.csv",example);
	const auto& nagata = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromNagata.csv",example);
	const auto& sintef = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromSINTEF.csv",example);

	aps->sintefRecRN = sintef.minRouteNumber;
	aps->sintefRecRL = static_cast<DisType>(sintef.minRouteDistance * disMul);
	aps->naRecRN = nagata.minRouteNumber;
	aps->naRecRL = static_cast<DisType>(nagata.minRouteDistance * disMul);

	aps->d15RecRN = d15.minRouteNumber;
	aps->d15RecRL = static_cast<DisType>(d15.minRouteDistance);

	aps->lkhRN = lkh.minRouteNumber;
	aps->lkhRL = static_cast<DisType>(lkh.minRouteDistance);
	aps->cmdIsopt = lkh.isOptimal;

}

bool Input::initInput() {

	readDimacsInstance(commandLine->instancePath);
	//readDimacsBKS();

	P = Vec<int>(custCnt + 1, 1);
	if (datas.size() < custCnt * 3 + 3) {
		datas.resize(custCnt * 3 + 3);
	}
	for (int i = custCnt + 1; i < datas.size(); ++i) {
		datas[i] = datas[0];
		datas[i].CUSTNO = i;
	}

	disOf = util::Array2D <DisType>(custCnt + 1, custCnt + 1,DisType(0));

	double sumq = 0;
	for (int i = 1; i <= custCnt; ++i) {
		sumq += datas[i].DEMAND;
	}
	Qbound = static_cast<int>(ceil(double(sumq) / Q));
	Qbound = std::max<int>(Qbound, 2);

	/*
		sqrt函数有三种形式
		double sqrt(double x);
		float sqrtf(float x);
		long double sqrtl(long double x);
		float精度最低，double较高，long double精度最高
	*/

	for (int i = 0; i <= custCnt ; ++i) {
		for (int j = i + 1; j <= custCnt; ++j) {
			Data& d1 = datas[i];
			Data& d2 = datas[j];
			double dis = sqrt((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
				+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));

			//disOf[j][i] = disOf[i][j] = dis+0.5;
			disOf[j][i] = disOf[i][j] = static_cast<DisType>(ceil(dis));
		}
	}

	sectorClose = util::Array2D<int>(custCnt + 1, custCnt,0);

	for (int v = 0; v <= custCnt; ++v) {

		int idx = 0;

		for (int pos = 0; pos <= custCnt; ++pos) {
			if (pos != v) {
				sectorClose[v][idx] = pos;
				++idx;
			}
		}
		auto cmp = [&](const int a, const int b) {
			return abs(datas[a].polarAngle - datas[v].polarAngle)
				< abs(datas[b].polarAngle - datas[v].polarAngle);
		};
		//TODO[-1]:这里的排序比较浪费时间，去掉可以节省一般的初始化时间
		std::sort(sectorClose[v], sectorClose[v] + sectorClose.size2(), cmp);
	}

	addSTclose = util::Array2D<int> (custCnt + 1, custCnt,0);

	for (int v = 0; v <= custCnt; ++v) {

		int idx = 0;
		for (int w = 0; w <= custCnt; ++w) {
			if (w != v) {
				addSTclose[v][idx] = w;
				++idx;
			}
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
		
		getTopKmin(addSTclose[v], addSTclose.size2(), aps->mRLLocalSearchRange[1],cmp);
		std::sort(addSTclose[v], addSTclose[v] + aps->mRLLocalSearchRange[1], cmp);

	}

	return true;
}

bool Input::readDimacsInstance(std::string& instanciaPath) {

	//debug(instanciaPath.c_str());
	FILE* file = fopen(instanciaPath.c_str(), "r");

	if (!file) {
		std::cout << instanciaPath << "Logger::ERROR: Instance path wrong." << std::endl;
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
			int newSize = static_cast<int>(datas.size()) + static_cast<int>(datas.size()) / 2;
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
			auto polar = CircleSector::positive_mod
			(32768. * atan2(dt.YCOORD - datas[0].YCOORD, dt.XCOORD - datas[0].XCOORD) / PI);
			dt.polarAngle = static_cast<int>(polar);
		}
		++index;
	}
	custCnt = index - 1;
	fclose(file);
	return true;
}

int Input::partition(int* arr, int start, int end, std::function<bool(int, int)>cmp) {
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

void Input::getTopKmin(int* input, int n, int k,std::function<bool(int,int)>cmp) {

	const int size = n;

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
