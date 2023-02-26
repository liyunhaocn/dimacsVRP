
#include <string>
#include <istream>
#include <functional>
#include <sstream>
#include <fstream>
#include <iostream>

#include "Input.h"
#include "Util.h"

namespace hust {

Input::Input(CommandLine* commandLine):
	commandLine(commandLine),
	aps(&commandLine->aps),
	random(commandLine->seed),
	randomx(commandLine->seed),
	timer(commandLine->runTimer) {

	//String example = aps->inputPath;
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

void Input::initDetail() {

	for (int v = 0; v <= customerNumer; ++v) {

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

	for (int v = 0; v <= customerNumer; ++v) {

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

	addSTJIsxthcloseOf = util::Array2D<int> (customerNumer + 1, customerNumer + 1, -1);

	for (int v = 0; v <= customerNumer; ++v) {
		for (int wpos = 0; wpos < addSTclose.size2(); ++wpos) {
			int w = addSTclose[v][wpos];
			addSTJIsxthcloseOf[v][w] = wpos;
		}
	}
	
	int deNeiSize = aps->outNeiborSize;
	deNeiSize = std::min(customerNumer - 1, deNeiSize);

	auto iInNeicloseOf = Vector< Vector<int> >
		(customerNumer + 1, Vector<int>());
	for (int i = 0; i < customerNumer + 1; ++i) {
		iInNeicloseOf[i].reserve(customerNumer);
	}

	for (int v = 0; v <= customerNumer; ++v) {
		for (int wpos = 0; wpos < deNeiSize; ++wpos) {
			int w = addSTclose[v][wpos];
			iInNeicloseOf[w].push_back(v);
		}
	}

	iInNeicloseOfUnionNeiCloseOfI = Vector< Vector<int> >(customerNumer + 1);

	for (int v = 0; v <= customerNumer; ++v) {

		iInNeicloseOfUnionNeiCloseOfI[v] = Vector<int>
			(addSTclose[v], addSTclose[v] + deNeiSize);

		for (int w : iInNeicloseOf[v]) {
			if (addSTJIsxthcloseOf[v][w] >= deNeiSize) {
				iInNeicloseOfUnionNeiCloseOfI[v].push_back(w);
			}
		}
	}
}

void Input::initInput() {

	P = Vector<int>(customerNumer + 1, 1);
	if (static_cast<int>(datas.size()) < customerNumer * 3 + 3) {
		datas.resize(customerNumer * 3 + 3);
	}
	for (int i = customerNumer + 1; i < static_cast<int>(datas.size()); ++i) {
		datas[i] = datas[0];
		datas[i].CUSTNO = i;
	}

	double sumq = 0;
	for (int i = 1; i <= customerNumer; ++i) {
		sumq += static_cast<double>(datas[i].DEMAND);
	}
	Qbound = static_cast<int>(ceil(double(sumq) / static_cast<double>(vehicleCapacity)));
	Qbound = std::max<int>(Qbound, 2);

	/*
		sqrt函数有三种形式
		double sqrt(double x);
		float sqrtf(float x);
		long double sqrtl(long double x);
		float精度最低，double较高，long double精度最高
	*/

	sectorClose = util::Array2D<int>(customerNumer + 1, customerNumer,0);

	for (int v = 0; v <= customerNumer; ++v) {

		int idx = 0;

		for (int pos = 0; pos <= customerNumer; ++pos) {
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

	addSTclose = util::Array2D<int> (customerNumer + 1, customerNumer,0);

	for (int v = 0; v <= customerNumer; ++v) {

		int idx = 0;
		for (int w = 0; w <= customerNumer; ++w) {
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
		
		getTopKmin(addSTclose[v], addSTclose.size2(), aps->neiborRange[1],cmp);
		std::sort(addSTclose[v], addSTclose[v] + aps->neiborRange[1], cmp);

	}
}

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

	this->customerNumer = 0;
	this->vehicleCapacity = INT_MAX;
	mustDispatchNumber = -1;
	DisType totalDemand = 0;

	String content, content2, content3;
	int node = 0;
	durationLimit = INT_MAX;
	int serviceTimeData = 0;
	bool hasServiceTimeSection = false;
	// Read INPUT dataset
	//std::ifstream inputFile(config.pathInstance);
	//Logger::INFO("cl.config.isNpsRun:", cl.config.isNpsRun);

	auto x = freopen(commandLine->instancePath.data(), "r", stdin);
	if (x == nullptr) {
		throw String("x== nullptr return of freopen");
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
			std::cin >> this->vehicleNumber >> vehicleCapacity;

			// Skip the next four lines
			getline(std::cin, content);
			getline(std::cin, content);
			getline(std::cin, content);
			getline(std::cin, content);

			// Create a vector where all information on the Clients can be stored and loop over all information in the file
			datas = Vector<Data>(1001);

			customerNumer = 0;
			while (std::cin >> node)
			{
				// Store all the information of the next client
				datas[customerNumer].CUSTNO = node;
				std::cin >> datas[customerNumer].XCOORD >> datas[customerNumer].YCOORD >> datas[customerNumer].DEMAND >> datas[customerNumer].READYTIME >> datas[customerNumer].DUEDATE >> datas[customerNumer].SERVICETIME;

				// Scale coordinates by factor 10, later the distances will be rounded so we optimize with 1 decimal distances
				datas[customerNumer].XCOORD *= 10;
				datas[customerNumer].YCOORD *= 10;
				datas[customerNumer].READYTIME *= 10;
				datas[customerNumer].DUEDATE *= 10;
				datas[customerNumer].SERVICETIME *= 10;
				datas[customerNumer].polarAngle = CircleSector::positive_mod(static_cast<int>(32768. * atan2(datas[customerNumer].YCOORD - datas[0].YCOORD, datas[customerNumer].XCOORD - datas[0].XCOORD) / PI));

				++customerNumer;
			}

			// Reduce the size of the vector of clients if possible
			datas.resize(customerNumer);

			// Don't count depot as client
			--customerNumer;

			// Check if the required service and the start of the time window of the depot are both zero
			if (datas[0].READYTIME != 0)
			{
				throw String("Time window for depot should start at 0");
			}
			if (datas[0].SERVICETIME != 0)
			{
				throw String("Service duration for depot should be 0");
			}

			disOf = util::Array2D <DisType>(customerNumer + 1, customerNumer + 1, DisType(0));
			for (int i = 0; i <= customerNumer; ++i) {
				for (int j = i + 1; j <= customerNumer; ++j) {
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
					std::cin >> content2 >> customerNumer;
					customerNumer--;
					P.resize(customerNumer + 1, 1);
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
						throw String("EDGE_WEIGHT_FORMAT can only be used with EDGE_WEIGHT_TYPE : EXPLICIT");
					}

					if (content3 != "FULL_MATRIX")
					{
						throw String("EDGE_WEIGHT_FORMAT only supports FULL_MATRIX");
					}
				}
				else if (content == "CAPACITY")
				{
					std::cin >> content2 >> vehicleCapacity;
				}
				else if (content == "VEHICLES" || content == "SALESMAN")
				{
					// Set vehicle count from instance only if not specified on CLI.
					std::cin >> content2;
					if (vehicleNumber == INT_MAX) {
						std::cin >> vehicleNumber;
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
						throw String("EDGE_WEIGHT_SECTION can only be used with EDGE_WEIGHT_TYPE : EXPLICIT");
					}
					maxDist = 0;
					

					disOf = util::Array2D <DisType>(customerNumer + 1, customerNumer + 1, DisType(0));

					for (int i = 0; i <= customerNumer; ++i)
					{
						for (int j = 0; j <= customerNumer; ++j)
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
					datas = Vector<Data>(customerNumer + 1);
					for (int i = 0; i <= customerNumer; i++)
					{
						std::cin >> datas[i].CUSTNO >> datas[i].XCOORD >> datas[i].YCOORD;

						// Check if the clients are in order
						if (datas[i].CUSTNO != i + 1)
						{
							throw String("Clients are not in order in the list of coordinates")
								+ "datas[i].custNum:" + std::to_string(datas[i].CUSTNO)
								+ "i + 1:" + std::to_string(i + 1);
						}
						datas[i].CUSTNO--;
						datas[i].polarAngle = CircleSector::positive_mod(static_cast<int>(32768. * atan2(datas[i].YCOORD - datas[0].YCOORD, datas[i].XCOORD - datas[0].XCOORD) / PI));
					}

					mustDispatchNumber = customerNumer;
				}
				// Read the DEMAND of each client (including the depot, which should have DEMAND 0)
				else if (content == "DEMAND_SECTION")
				{
					for (int i = 0; i <= customerNumer; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].DEMAND;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw String("Clients are not in order in the list of demands");
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
						throw String("Depot DEMAND is not zero, but is instead: " + std::to_string(datas[0].SERVICETIME));
					}
				}
				else if (content == "DEPOT_SECTION")
				{
					std::cin >> content2 >> content3;
					if (content2 != "1")
					{
						throw String("Expected depot index 1 instead of " + content2);
					}
				}
				else if (content == "CUSTOMER_WEIGHT")
				{
					for (int i = 0; i <= customerNumer; ++i)
					{
						int clientNr = 0;
						std::cin >> clientNr >> P[i];
						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw String("Clients are not in order in the list of CUSTOMER_WEIGHT")
								+ "clientNr:" + std::to_string(clientNr)
								+ "i + 1:" + std::to_string(i + 1);
						}
					}
					// Check if the service duration of the depot is 0
					if (P[0] != 0)
					{
						throw String("P[0] should be 0");
					}
				}
				else if (content == "SERVICE_TIME_SECTION")
				{
					for (int i = 0; i <= customerNumer; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].SERVICETIME;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw String("Clients are not in order in the list of service times");
						}
					}
					// Check if the service duration of the depot is 0
					if (datas[0].SERVICETIME != 0)
					{
						throw String("Service duration for depot should be 0");
					}
					hasServiceTimeSection = true;
				}
				//else if (content == "RELEASE_TIME_SECTION")
				//{
				//	for (int i = 0; i <= customerNumer; i++)
				//	{
				//		int clientNr = 0;
				//		std::cin >> clientNr >> datas[i].releaseTime;

				//		// Check if the clients are in order
				//		if (clientNr != i + 1)
				//		{
				//			throw String("Clients are not in order in the list of release times");
				//		}
				//	}
				//	// Check if the service duration of the depot is 0
				//	if (datas[0].releaseTime != 0)
				//	{
				//		throw String("Release time for depot should be 0");
				//	}
				//}
				// Read the time windows of all the clients (the depot should have a time window from 0 to max)
				else if (content == "TIME_WINDOW_SECTION")
				{
					isTimeWindowConstraint = true;
					for (int i = 0; i <= customerNumer; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].READYTIME >> datas[i].DUEDATE;

						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							Logger::ERROR("Clients are not in order in the list of time windows");
							throw String("Clients are not in order in the list of time windows");
						}
					}

					// Check the start of the time window of the depot
					if (datas[0].READYTIME != 0)
					{
						Logger::ERROR("Time window for depot should start at 0");
						throw String("Time window for depot should start at 0");
					}
				}
				else
				{
					Logger::ERROR("Unexpected data in input file: " + content);
					throw String("Unexpected data in input file: " + content);
				}
			}

			if (!hasServiceTimeSection)
			{
				for (int i = 0; i <= customerNumer; i++)
				{
					datas[i].SERVICETIME = (i == 0) ? 0 : serviceTimeData;
				}
			}

			if (customerNumer <= 0)
			{
				throw String("Number of nodes is undefined");
			}
			if (vehicleCapacity == INT_MAX)
			{
				throw String("Vehicle capacity is undefined");
			}
		}

		if (static_cast<int>(datas.size()) < customerNumer * 3 + 3) {
			datas.resize(customerNumer * 3 + 3);
		}
		for (int i = customerNumer + 1; i < static_cast<int>(datas.size()); ++i) {
			datas[i] = datas[0];
			datas[i].CUSTNO = i;
		}

		//Logger::ERROR("mustDispatchNumber:",mustDispatchNumber);
	}
	else {
		throw std::invalid_argument("Impossible to open instance file: " + commandLine->instancePath);
	}

	std::cin.clear();
	fclose(stdin);

	// Default initialization if the number of vehicles has not been provided by the user
	if (vehicleNumber == INT_MAX)
	{
		// Safety margin: 30% + 3 more vehicles than the trivial bin packing LB
		vehicleNumber = static_cast<int>(std::ceil(1.3 * totalDemand / vehicleCapacity) + 3.);
		Logger::INFO("----- FLEET SIZE WAS NOT SPECIFIED: DEFAULT INITIALIZATION TO ", vehicleNumber, " VEHICLES");
	}
	else if (vehicleNumber == -1)
	{
		vehicleNumber = customerNumer;
		Logger::INFO("----- FLEET SIZE UNLIMITED: SET TO UPPER BOUND OF ", vehicleNumber, " VEHICLES");
	}
	else
	{
		Logger::INFO("----- FLEET SIZE SPECIFIED IN THE COMMANDLINE: SET TO ", vehicleNumber, " VEHICLES");
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
