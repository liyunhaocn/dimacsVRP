
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
		sqrt have three kind
		double sqrt(double x);
		float sqrtf(float x);
		long double sqrtl(long double x);
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
		//OPT[init] the sort will take much time, it can be quick when sort in initDetial
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

			//OPT[-1] this can be changed below
			//return disOf[v][a] < disOf[v][b];
//			return disOf[v][a] + datas[a].SERVICETIME <
//				disOf[v][b] + datas[b].SERVICETIME;

			if (disOf[a][v] == disOf[b][v]) {
				return datas[a].DUEDATE < datas[b].DUEDATE;
			}
			else {
				return disOf[a][v] < disOf[b][v];
			}
			return true;

//			int aLinkv = canLinkNode(a, v);
//			int bLinkv = canLinkNode(b, v);
//			if ((aLinkv && bLinkv) || (!aLinkv && !bLinkv)) {
//				return disOf[v][a] + datas[a].SERVICETIME <
//					disOf[v][b] + datas[b].SERVICETIME;
//			}
//			else {
//				return aLinkv ? true : false;
//			}

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

	String content, content2, content3;
	int node = 0;
	DisType totalDemad = 0;

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
			vehicleCapacity *= 10;

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
				datas[customerNumer].DEMAND *= 10;
				datas[customerNumer].READYTIME *= 10;
				datas[customerNumer].DUEDATE *= 10;
				datas[customerNumer].SERVICETIME *= 10;
				DisType deltaY = datas[customerNumer].YCOORD - datas[0].YCOORD;
				DisType deltaX = datas[customerNumer].XCOORD - datas[0].XCOORD;

				datas[customerNumer].polarAngle = 
					CircleSector::positive_mod(static_cast<int>(32768. * 
						atan2(static_cast<double>(deltaY), static_cast<double>(deltaX)) / PI));
				totalDemad += datas[customerNumer].DEMAND;
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

					DisType deltaX = d1.XCOORD - d2.XCOORD;
					DisType deltaY = d1.YCOORD - d2.YCOORD;

					disOf[j][i] = disOf[i][j] = static_cast<DisType>(sqrt(static_cast<double>(deltaX * deltaX + deltaY * deltaY)));
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
				}
				else if (content == "EDGE_WEIGHT_FORMAT")
				{
					std::cin >> content2 >> content3;
				}
				else if (content == "CAPACITY")
				{
					std::cin >> content2 >> vehicleCapacity;
				}
				else if (content == "VEHICLES" || content == "SALESMAN")
				{
					// Set vehicle count from instance only if not specified on CLI.
					std::cin >> content2 >> vehicleNumber;
				}
				// Read the data on the service time (used when the service time is constant for all clients)
				else if (content == "SERVICE_TIME")
				{
					int serviceTimeData = 0;
					std::cin >> content2 >> serviceTimeData;
				}
				// Read the edge weights of an explicit distance matrix
				else if (content == "EDGE_WEIGHT_SECTION")
				{

					disOf = util::Array2D <DisType>(customerNumer + 1, customerNumer + 1, DisType(0));

					for (int i = 0; i <= customerNumer; ++i)
					{
						for (int j = 0; j <= customerNumer; ++j)
						{
							// Keep track of the largest distance between two clients (or the depot)
							DisType cost;
							std::cin >> cost;
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

						DisType deltaY = datas[i].YCOORD - datas[0].YCOORD;
						DisType deltaX = datas[i].XCOORD - datas[0].XCOORD;

						datas[i].polarAngle =
							CircleSector::positive_mod(static_cast<int>(32768. *
								atan2(static_cast<double>(deltaY), static_cast<double>(deltaX)) / PI));
					}
				}
				// Read the DEMAND of each client (including the depot, which should have DEMAND 0)
				else if (content == "DEMAND_SECTION")
				{
					for (int i = 0; i <= customerNumer; i++)
					{
						int clientNr = 0;
						std::cin >> clientNr >> datas[i].DEMAND;
						totalDemad += datas[i].DEMAND;
						// Check if the clients are in order
						if (clientNr != i + 1)
						{
							throw String("Clients are not in order in the list of demands");
						}
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
				else if (content == "CUSTOMER_WEIGHT_SECTION")
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
                else if (content == "MUST_DISPATCH_SECTION")
                {
                    for (int i = 0; i <= customerNumer; i++)
                    {
                        int clientNr = 0;
                        std::cin >> clientNr >> datas[i].must_dispatch;
                        // Check if the clients are in order
                        if (clientNr != i + 1)
                        {
                            throw std::string("Clients are not in order in the list of MUST_DISPATCH");
                        }
                    }
                    // Check if the service duration of the depot is 0
                    if (datas[0].must_dispatch != 1)
                    {
                        throw std::string("must_dispatch depot should be 1");
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
				}
				
				else if (content == "TIME_WINDOW_SECTION")
				{
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

	int approximateNumberOfVehicles = static_cast<int>(totalDemad / vehicleCapacity);

	aps->ruinLmax = customerNumer / approximateNumberOfVehicles;

	if (aps->ruinLmax < 25) {
		//short route
		aps->populationSizeMin = 5;
		aps->populationSizeMax = 10;
		aps->neiborSizeMax = 25;
	}
	else {//long route
		aps->populationSizeMin = 4;
		aps->populationSizeMax = 50;
		aps->neiborSizeMax = 35;
	}
}

int Input::partition(int* arr, int start, int end, std::function<bool(int, int)>cmp) {

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
