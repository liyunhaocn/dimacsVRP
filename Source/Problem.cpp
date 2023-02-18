
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
	if (!inStream) {
		Logger::INFO("file to open the file", path);
		return InstanceData{};
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

	readInstanceFormatCVRPLIB();

	initInput();
	initDetail();
}

Input::Input(CommandLine* commandLine, AlgorithmParameters* aps, RandomTools* randomTools, Timer* timer, const DataModel& dm):
	commandLine(commandLine), aps(aps), randomTools(randomTools), timer(timer) {
	if (dm.check() == false) {
		Logger::ERROR("Customized Data Model Error");
		exit(-1);
	}
	setInstanceCustomized(dm);

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

	const auto& d15 = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromDIMACSDay15.csv",instanceName);
	const auto& lkh = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromDIMACSLKH.csv", instanceName);
	const auto& nagata = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromNagata.csv", instanceName);
	const auto& sintef = getInstanceDataFromFile(commandLine->bksDataFileBasePath + "BKSfromSINTEF.csv", instanceName);

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

void Input::initInput() {

	P = Vec<int>(custCnt + 1, 1);
	if (static_cast<int>(datas.size()) < custCnt * 3 + 3) {
		datas.resize(custCnt * 3 + 3);
	}
	for (int i = custCnt + 1; i < static_cast<int>(datas.size()); ++i) {
		datas[i] = datas[0];
		datas[i].CUSTNO = i;
	}

	double sumq = 0;
	for (int i = 1; i <= custCnt; ++i) {
		sumq += static_cast<double>(datas[i].DEMAND);
	}
	Qbound = static_cast<int>(ceil(double(sumq) / static_cast<double>(Q)));
	Qbound = std::max<int>(Qbound, 2);

	/*
		sqrt函数有三种形式
		double sqrt(double x);
		float sqrtf(float x);
		long double sqrtl(long double x);
		float精度最低，double较高，long double精度最高
	*/

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
}


//void Input::readInstanceFormatCVRPLIB() {
//
//	//debug(instanciaPath.c_str());
//	FILE* file = nullptr;
//
//	if (commandLine->readInstanceFromStdin == 1) {
//		;
//	}
//	else {
//		file = fopen(commandLine->instancePath.c_str(), "r");
//	}
//
//	if (!file) {
//		std::cout << commandLine->instancePath << "Logger::ERROR: Instance path wrong." << std::endl;
//		exit(EXIT_FAILURE);
//	}
//
//	char name[64];
//	
//	fscanf(file, "%s\n", name);
//	this->instanceName = std::string(name);
//	fscanf(file, "%*[^\n]\n");
//	fscanf(file, "%*[^\n]\n");
//	fscanf(file, "%d %lld\n", &this->vehicleCnt, &this->Q);
//	fscanf(file, "%*[^\n]\n");
//	fscanf(file, "%*[^\n]\n");
//
//	this->Q *= disMul;
//	std::string line = "";
//
//	this->datas = Vec<Data>(303);
//
//	int index = 0;
//	int id = -1, coordx = -1, coordy = -1, DEMAND = -1;
//	int ready_time = -1, due_date = -1, service_time = -1;
//	int readArgNum = 0;
//	while ((readArgNum = fscanf(file, "%d %d %d %d %d %d %d\n", &id, &coordx, &coordy, &DEMAND, &ready_time, &due_date, &service_time)) == 7) {
//
//		if (index >= static_cast<int>(datas.size())) {
//			int newSize = static_cast<int>(datas.size()) + static_cast<int>(datas.size()) / 2;
//			datas.resize(newSize);
//		}
//
//		this->datas[index].CUSTNO = id;
//		this->datas[index].XCOORD = coordx * disMul;
//		this->datas[index].YCOORD = coordy * disMul;
//		this->datas[index].DEMAND = DEMAND * disMul;
//		this->datas[index].READYTIME = ready_time * disMul;
//		this->datas[index].DUEDATE = due_date * disMul;
//		this->datas[index].SERVICETIME = service_time * disMul;
//
//		if (index > 0) {
//			auto& dt = datas[index];
//			auto polar = CircleSector::positive_mod
//			(32768. * atan2(dt.YCOORD - datas[0].YCOORD, dt.XCOORD - datas[0].XCOORD) / PI);
//			dt.polarAngle = static_cast<int>(polar);
//		}
//		++index;
//	}
//	custCnt = index - 1;
//	disOf = util::Array2D <DisType>(custCnt + 1, custCnt + 1, DisType(0));
//	for (int i = 0; i <= custCnt; ++i) {
//		for (int j = i + 1; j <= custCnt; ++j) {
//			Data& d1 = datas[i];
//			Data& d2 = datas[j];
//			double dis = sqrt((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
//				+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));
//
//			//disOf[j][i] = disOf[i][j] = dis+0.5;
//			disOf[j][i] = disOf[i][j] = static_cast<DisType>(ceil(dis));
//		}
//	}
//	fclose(file);
//}

/*MIT License

Original HGS-CVRP code: Copyright(c) 2020 Thibaut Vidal
Additional contributions: Copyright(c) 2022 ORTEC
Additional contributions: Copyright(c) 2023 liyunhao

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

void Input::readInstanceFormatCVRPLIB() {

	this->custCnt = 0;
	this->Q = INT_MAX;
	mustDispatchNumber = -1;
	DisType totalDemand = 0;

	std::string content, content2, content3;
	int node = 0;
	durationLimit = INT_MAX;
	int serviceTimeData = 0;
	bool hasServiceTimeSection = false;
	// Read INPUT dataset
	//std::ifstream inputFile(config.pathInstance);
	//Logger::INFO("cl.config.isNpsRun:", cl.config.isNpsRun);
	if (commandLine->readInstanceFromStdin == 1) {

	}
	else {
		auto x = freopen(commandLine->instancePath.data(), "r", stdin);
		if (x == nullptr) {
			throw std::string("x== nullptr return of freopen");
		}
	}

	if (true)
	{
		// Read the instance name from the first line and remove \r
		getline(std::cin, content);

		instanceName = content;
		instanceName.erase(std::remove(instanceName.begin(), instanceName.end(), '\r'), instanceName.end());

		// Read the next lines
		getline(std::cin, content);	// "Empty line" or "NAME : {instance_name}"
		getline(std::cin, content);	// VEHICLE or "COMMENT: {}"

		// Check if the next line has "VEHICLE"
		if (content.substr(0, 7) == "VEHICLE")
		{
			// Get the number of vehicles and the capacity of the vehicles
			getline(std::cin, content);  // NUMBER    CAPACITY
			std::cin >> this->vehicleCnt >> Q;

			// Skip the next four lines
			getline(std::cin, content);
			getline(std::cin, content);
			getline(std::cin, content);
			getline(std::cin, content);

			// Create a vector where all information on the Clients can be stored and loop over all information in the file
			datas = std::vector<Data>(1001);

			custCnt = 0;
			while (std::cin >> node)
			{
				// Store all the information of the next client
				datas[custCnt].CUSTNO = node;
				std::cin >> datas[custCnt].XCOORD >> datas[custCnt].YCOORD >> datas[custCnt].DEMAND >> datas[custCnt].READYTIME >> datas[custCnt].DUEDATE >> datas[custCnt].SERVICETIME;

				// Scale coordinates by factor 10, later the distances will be rounded so we optimize with 1 decimal distances
				datas[custCnt].XCOORD *= 10;
				datas[custCnt].YCOORD *= 10;
				datas[custCnt].READYTIME *= 10;
				datas[custCnt].DUEDATE *= 10;
				datas[custCnt].SERVICETIME *= 10;
				datas[custCnt].polarAngle = CircleSector::positive_mod(static_cast<int>(32768. * atan2(datas[custCnt].YCOORD - datas[0].YCOORD, datas[custCnt].XCOORD - datas[0].XCOORD) / PI));

				custCnt++;
			}

			// Reduce the size of the vector of clients if possible
			datas.resize(custCnt);

			// Don't count depot as client
			--custCnt;

			// Check if the required service and the start of the time window of the depot are both zero
			if (datas[0].READYTIME != 0)
			{
				throw std::string("Time window for depot should start at 0");
			}
			if (datas[0].SERVICETIME != 0)
			{
				throw std::string("Service duration for depot should be 0");
			}

			disOf = util::Array2D <DisType>(custCnt + 1, custCnt + 1, DisType(0));
			for (int i = 0; i <= custCnt; ++i) {
				for (int j = i + 1; j <= custCnt; ++j) {
					Data& d1 = datas[i];
					Data& d2 = datas[j];
					double dis = sqrt((d1.XCOORD - d2.XCOORD) * (d1.XCOORD - d2.XCOORD)
						+ (d1.YCOORD - d2.YCOORD) * (d1.YCOORD - d2.YCOORD));
					//disOf[j][i] = disOf[i][j] = dis+0.5;
					disOf[j][i] = disOf[i][j] = static_cast<DisType>(ceil(dis));
				}
			}
		}
		else
		{
			// CVRP or VRPTW according to VRPLib format
			for (std::cin >> content; content != "EOF"; std::cin >> content)
			{
				// Read the dimension of the problem (the number of clients)
				if (content == "DIMENSION")
				{
					// Need to substract the depot from the number of nodes
					std::cin >> content2 >> custCnt;
					custCnt--;
					P.resize(custCnt + 1, 1);
				}
				// Read the type of edge weights
				else if (content == "EDGE_WEIGHT_TYPE")
				{
					std::cin >> content2 >> content3;
					if (content3 == "EXPLICIT")
					{
						//isExplicitDistanceMatrix = true;
					}
				}
				else if (content == "EDGE_WEIGHT_FORMAT")
				{
					std::cin >> content2 >> content3;
					if (!isExplicitDistanceMatrix)
					{
						throw std::string("EDGE_WEIGHT_FORMAT can only be used with EDGE_WEIGHT_TYPE : EXPLICIT");
					}

					if (content3 != "FULL_MATRIX")
					{
						throw std::string("EDGE_WEIGHT_FORMAT only supports FULL_MATRIX");
					}
				}
				else if (content == "CAPACITY")
				{
					std::cin >> content2 >> Q;
				}
				else if (content == "VEHICLES" || content == "SALESMAN")
				{
					// Set vehicle count from instance only if not specified on CLI.
					std::cin >> content2;
					if (vehicleCnt == INT_MAX) {
						std::cin >> vehicleCnt;
					}
					else {
						// Discard vehicle count
						int _;
						std::cin >> _;
					}
				}
				else if (content == "DISTANCE")
				{
					std::cin >> content2 >> durationLimit; isDurationConstraint = true;
				}
				// Read the data on the service time (used when the service time is constant for all clients)
				else if (content == "SERVICE_TIME")
				{
					std::cin >> content2 >> serviceTimeData;
				}
				// Read the edge weights of an explicit distance matrix
				else if (content == "EDGE_WEIGHT_SECTION")
				{
					if (!isExplicitDistanceMatrix)
					{
						throw std::string("EDGE_WEIGHT_SECTION can only be used with EDGE_WEIGHT_TYPE : EXPLICIT");
					}
					maxDist = 0;
					

					disOf = util::Array2D <DisType>(custCnt + 1, custCnt + 1, DisType(0));

					for (int i = 0; i <= custCnt; ++i)
					{
						for (int j = 0; j <= custCnt; ++j)
						{
							// Keep track of the largest distance between two clients (or the depot)
							int cost;
							std::cin >> cost;
							if (cost > maxDist)
							{
								maxDist = cost;
							}
							disOf[i][j] = static_cast<DisType>(cost);
						}
					}
				}
				else if (content == "NODE_COORD_SECTION")
				{
					// Reading client coordinates
					datas = std::vector<Data>(custCnt + 1);
					for (int i = 0; i <= custCnt; i++)
					{
						std::cin >> datas[i].CUSTNO >> datas[i].XCOORD >> datas[i].YCOORD;

						// Check if the clients are in order
						if (datas[i].CUSTNO != i + 1)
						{
							throw std::string("Clients are not in order in the list of coordinates")
								+ "datas[i].custNum:" + std::to_string(datas[i].CUSTNO)
								+ "i + 1:" + std::to_string(i + 1);
						}
						datas[i].CUSTNO--;
						datas[i].polarAngle = CircleSector::positive_mod(static_cast<int>(32768. * atan2(datas[i].YCOORD - datas[0].YCOORD, datas[i].XCOORD - datas[0].XCOORD) / PI));
					}
					// 将所有的顾客都置为必须配送
					for (int i = 0; i <= custCnt; i++) {
						datas[i].must_dispatch = 1;
					}
					mustDispatchNumber = custCnt;
				}
				// Read the DEMAND of each client (including the depot, which should have DEMAND 0)
				else if (content == "DEMAND_SECTION")
				{
					for (int i = 0; i <= custCnt; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].DEMAND;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw std::string("Clients are not in order in the list of demands");
						}

						// Keep track of the max and total DEMAND
						if (datas[i].DEMAND > maxDemand)
						{
							maxDemand = datas[i].DEMAND;
						}
						totalDemand += datas[i].DEMAND;
					}
					// Check if the depot has DEMAND 0
					if (datas[0].DEMAND != 0)
					{
						throw std::string("Depot DEMAND is not zero, but is instead: " + std::to_string(datas[0].SERVICETIME));
					}
				}
				else if (content == "DEPOT_SECTION")
				{
					std::cin >> content2 >> content3;
					if (content2 != "1")
					{
						throw std::string("Expected depot index 1 instead of " + content2);
					}
				}
				else if (content == "CUSTOMER_WEIGHT")
				{
					for (int i = 0; i <= custCnt; ++i)
					{
						int clientNr = 0;
						std::cin >> clientNr >> P[i];
						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw std::string("Clients are not in order in the list of CUSTOMER_WEIGHT")
								+ "clientNr:" + std::to_string(clientNr)
								+ "i + 1:" + std::to_string(i + 1);
						}
					}
					// Check if the service duration of the depot is 0
					if (P[0] != 0)
					{
						throw std::string("P[0] should be 0");
					}
				}
				else if (content == "MUST_DISPATCH")
				{
					mustDispatchNumber = 0;
					for (int i = 0; i <= custCnt; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].must_dispatch;
						if (datas[i].must_dispatch == 1) {
							++mustDispatchNumber;
						}
						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw std::string("Clients are not in order in the list of MUST_DISPATCH");
						}
					}
					// Check if the service duration of the depot is 0
					if (datas[0].must_dispatch != 0)
					{
						throw std::string("must_dispatch depot should be 0");
					}
				}
				else if (content == "SERVICE_TIME_SECTION")
				{
					for (int i = 0; i <= custCnt; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].SERVICETIME;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw std::string("Clients are not in order in the list of service times");
						}
					}
					// Check if the service duration of the depot is 0
					if (datas[0].SERVICETIME != 0)
					{
						throw std::string("Service duration for depot should be 0");
					}
					hasServiceTimeSection = true;
				}
				//else if (content == "RELEASE_TIME_SECTION")
				//{
				//	for (int i = 0; i <= custCnt; i++)
				//	{
				//		int clientNr = 0;
				//		std::cin >> clientNr >> datas[i].releaseTime;

				//		// Check if the clients are in order
				//		if (clientNr != i + 1)
				//		{
				//			throw std::string("Clients are not in order in the list of release times");
				//		}
				//	}
				//	// Check if the service duration of the depot is 0
				//	if (datas[0].releaseTime != 0)
				//	{
				//		throw std::string("Release time for depot should be 0");
				//	}
				//}
				// Read the time windows of all the clients (the depot should have a time window from 0 to max)
				else if (content == "TIME_WINDOW_SECTION")
				{
					isTimeWindowConstraint = true;
					for (int i = 0; i <= custCnt; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].READYTIME >> datas[i].DUEDATE;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							Logger::ERROR("Clients are not in order in the list of time windows");
							throw std::string("Clients are not in order in the list of time windows");
						}
					}

					// Check the start of the time window of the depot
					if (datas[0].READYTIME != 0)
					{
						Logger::ERROR("Time window for depot should start at 0");
						throw std::string("Time window for depot should start at 0");
					}
				}
				else
				{
					Logger::ERROR("Unexpected data in input file: " + content);
					throw std::string("Unexpected data in input file: " + content);
				}
			}

			if (!hasServiceTimeSection)
			{
				for (int i = 0; i <= custCnt; i++)
				{
					datas[i].SERVICETIME = (i == 0) ? 0 : serviceTimeData;
				}
			}

			if (custCnt <= 0)
			{
				throw std::string("Number of nodes is undefined");
			}
			if (Q == INT_MAX)
			{
				throw std::string("Vehicle capacity is undefined");
			}
		}

		if (static_cast<int>(datas.size()) < custCnt * 3 + 3) {
			datas.resize(custCnt * 3 + 3);
		}
		for (int i = custCnt + 1; i < static_cast<int>(datas.size()); ++i) {
			datas[i] = datas[0];
			datas[i].CUSTNO = i;
		}

		//Logger::ERROR("mustDispatchNumber:",mustDispatchNumber);
	}
	else {
		throw std::invalid_argument("Impossible to open instance file: " + commandLine->instancePath);
	}

	// Default initialization if the number of vehicles has not been provided by the user
	if (vehicleCnt == INT_MAX)
	{
		// Safety margin: 30% + 3 more vehicles than the trivial bin packing LB
		vehicleCnt = static_cast<int>(std::ceil(1.3 * totalDemand / Q) + 3.);
		Logger::INFO("----- FLEET SIZE WAS NOT SPECIFIED: DEFAULT INITIALIZATION TO ", vehicleCnt, " VEHICLES");
	}
	else if (vehicleCnt == -1)
	{
		vehicleCnt = custCnt;
		Logger::INFO("----- FLEET SIZE UNLIMITED: SET TO UPPER BOUND OF ", vehicleCnt, " VEHICLES");
	}
	else
	{
		Logger::INFO("----- FLEET SIZE SPECIFIED IN THE COMMANDLINE: SET TO ", vehicleCnt, " VEHICLES");
	}
}

void Input::setInstanceCustomized(const DataModel& dm) {

	char name[64];

	this->instanceName = dm.instanceName;
	this->vehicleCnt = dm.num_vehicles;
	this->Q = dm.vehicles_capacity;

	this->datas = Vec<Data>(dm.num_customers + 1);
	this->custCnt = dm.num_customers;

	for (int i = 0; i <= dm.num_customers; ++i) {
		this->datas[i].CUSTNO = i;
		this->datas[i].XCOORD = dm.coordinates[i].first;
		this->datas[i].YCOORD = dm.coordinates[i].second;
		this->datas[i].DEMAND = dm.demands[i];
		this->datas[i].READYTIME = dm.time_windows[i].first;
		this->datas[i].DUEDATE = dm.time_windows[i].second;
		this->datas[i].SERVICETIME = dm.service_times[i];

		if (i > 0) {
			auto& dt = datas[i];
			auto polar = CircleSector::positive_mod
			(32768. * atan2(dt.YCOORD - datas[0].YCOORD, dt.XCOORD - datas[0].XCOORD) / PI);
			dt.polarAngle = static_cast<int>(polar);
		}
	}
	
	disOf = util::Array2D <DisType>(custCnt + 1, custCnt + 1, DisType(0));
	for (int i = 0; i <= custCnt; ++i) {
		for (int j = 0 ; j <= custCnt; ++j) {
			disOf[j][i] = dm.time_matrix[i][j];
		}
	}
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
