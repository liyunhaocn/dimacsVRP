#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#include "Individual.h"
#include "Problem.h"
#include "Util_Common.h"

void Individual::evaluateCompleteCost()
{
	// Create an object to store all information regarding solution costs
	myCostSol = CostSol();
	// Loop over all routes that are not empty
	for (int r = 0; r < input->vehicleCnt; r++)
	{
		if (!chromR[r].empty())
		{
			
			// Get the distance, load, SERVICETIME and time associated with the vehicle traveling from the depot to the first client
			// Assume depot has service time 0 and READYTIME 0
			int distance = input->getDisof2(0, chromR[r][0]);
			int load = input->datas[chromR[r][0]].DEMAND;
			int service = input->datas[chromR[r][0]].SERVICETIME;
			// Running time excludes service of current node. This is the time that runs with the vehicle traveling
			// We start the route at the latest release time (or later but then we can just wait and there is no penalty for waiting)
			int waitTime = 0;
			int timeWarp = 0;
			int time = distance;
			// Add possible waiting time
			if (time < input->datas[chromR[r][0]].READYTIME)
			{
				// Don't add wait time since we can start route later 
				// (doesn't really matter since there is no penalty anyway)
				// waitTime += input->datas[chromR[r][0]].READYTIME - time;
				time = input->datas[chromR[r][0]].READYTIME;
			}
			// Add possible time warp
			else if (time > input->datas[chromR[r][0]].DUEDATE)
			{
				timeWarp += time - input->datas[chromR[r][0]].DUEDATE;
				time = input->datas[chromR[r][0]].DUEDATE;
			}
			predecessors[chromR[r][0]] = 0;

			// Loop over all clients for this vehicle
			for (int i = 1; i < static_cast<int>(chromR[r].size()); i++)
			{
				// Sum the distance, load, SERVICETIME and time associated with the vehicle traveling from the depot to the next client
				distance += input->getDisof2(chromR[r][i - 1], chromR[r][i]);
				load += input->datas[chromR[r][i]].DEMAND;
				service += input->datas[chromR[r][i]].SERVICETIME;
				time = time + input->datas[chromR[r][i - 1]].SERVICETIME + input->getDisof2(chromR[r][i - 1], chromR[r][i]);

				// Add possible waiting time
				if (time < input->datas[chromR[r][i]].READYTIME)
				{
					waitTime += input->datas[chromR[r][i]].READYTIME - time;
					time = input->datas[chromR[r][i]].READYTIME;
				}
				// Add possible time warp
				else if (time > input->datas[chromR[r][i]].DUEDATE)
				{
					timeWarp += time - input->datas[chromR[r][i]].DUEDATE;
					time = input->datas[chromR[r][i]].DUEDATE;
				}
				
				// Update predecessors and successors
				predecessors[chromR[r][i]] = chromR[r][i - 1];
				successors[chromR[r][i - 1]] = chromR[r][i];
			}

			// For the last client, the successors is the depot. Also update the distance and time
			successors[chromR[r][chromR[r].size() - 1]] = 0;
			distance += input->getDisof2(chromR[r][chromR[r].size() - 1], 0);
			time = time + input->datas[chromR[r][chromR[r].size() - 1]].SERVICETIME + input->getDisof2(chromR[r][chromR[r].size() - 1], 0);
			
			// For the depot, we only need to check the end of the time window (add possible time warp)
			if (time > input->datas[0].DUEDATE)
			{
				timeWarp += time - input->datas[0].DUEDATE;
				time = input->datas[0].DUEDATE;
			}
			// Update variables that track stats on the whole solution (all vehicles combined)
			myCostSol.distance += distance;
			myCostSol.waitTime += waitTime;
			myCostSol.timeWarp += timeWarp;
			myCostSol.nbRoutes++;
			if (load > input->Q)
			{
				myCostSol.capacityExcess += load - input->Q;
			}
		}
	}

	// When all vehicles are dealt with, calculated total penalized cost and check if the solution is feasible. (Wait time does not affect feasibility)
	//myCostSol.penalizedCost = myCostSol.distance + myCostSol.capacityExcess * input->penaltyCapacity + myCostSol.timeWarp * input->penaltyTimeWarp + myCostSol.waitTime * input->penaltyWaitTime;
	myCostSol.penalizedCost = myCostSol.distance + myCostSol.capacityExcess + myCostSol.timeWarp + myCostSol.waitTime;
	//isFeasible = (myCostSol.capacityExcess < MY_EPSILON && myCostSol.timeWarp < MY_EPSILON);
	isFeasible = (myCostSol.capacityExcess == 0 && myCostSol.timeWarp ==0);
}

void Individual::shuffleChromT()
{
	// Initialize the chromT with values from 1 to nbClients
	for (int i = 0; i < input->custCnt; i++)
	{
		chromT[i] = i + 1;
	}
	// Do a random shuffle chromT from begin to end
	std::shuffle(chromT.begin(), chromT.end(), input->random->rgen);
}

void Individual::removeProximity(Individual* indiv)
{
	// Get the first individual in indivsPerProximity
	auto it = indivsPerProximity.begin();
	// Loop over all individuals in indivsPerProximity until indiv is found
	while (it->second != indiv)
	{
		++it;
	}
	// Remove indiv from indivsPerProximity
	indivsPerProximity.erase(it);
}

double Individual::brokenPairsDistance(Individual* indiv2)
{
	// Initialize the difference to zero. Then loop over all clients of this individual
	int differences = 0;
	for (int j = 1; j <= input->custCnt; j++)
	{
		// Increase the difference if the successor of j in this individual is not directly linked to j in indiv2
		if (successors[j] != indiv2->successors[j] && successors[j] != indiv2->predecessors[j])
		{
			differences++;
		}
		// Last loop covers all but the first arc. Increase the difference if the predecessor of j in this individual is not directly linked to j in indiv2
		if (predecessors[j] == 0 && indiv2->predecessors[j] != 0 && indiv2->successors[j] != 0)
		{
			differences++;
		}
	}
	return static_cast<double>(differences) / input->custCnt;
}

double Individual::averageBrokenPairsDistanceClosest(int nbClosest)
{
	double result = 0;
	int maxSize = std::min(nbClosest, static_cast<int>(indivsPerProximity.size()));
	auto it = indivsPerProximity.begin();
	for (int i = 0; i < maxSize; i++)
	{
		result += it->first;
		++it;
	}
	return result / maxSize;
}

bool Individual::readCVRPLibFormat(std::string fileName, std::vector<std::vector<int>>& readSolution, double& readCost)
{
	readSolution.clear();
	std::ifstream inputFile(fileName);
	if (inputFile.is_open())
	{
		std::string inputString;
		inputFile >> inputString;
		// Loops as long as the first line keyword is "Route"
		for (int r = 0; inputString == "Route"; r++)
		{
			readSolution.push_back(std::vector<int>());
			inputFile >> inputString;
			getline(inputFile, inputString);
			std::stringstream ss(inputString);
			int inputCustomer;
			// Loops as long as there is an integer to read
			while (ss >> inputCustomer)
			{
				readSolution[r].push_back(inputCustomer);
			}
			inputFile >> inputString;
		}
		if (inputString == "Cost")
		{
			inputFile >> readCost;
			return true;
		}
		else std::cout << "----- UNEXPECTED WORD IN SOLUTION FORMAT: " << inputString << std::endl;
	}
	else std::cout << "----- IMPOSSIBLE TO OPEN: " << fileName << std::endl;
	return false;
}

Individual::Individual(hust::Input* input, bool initializeChromTAndShuffle) : input(input), isFeasible(false), biasedFitness(0)
{
	successors = std::vector<int>(input->custCnt + 1);
	predecessors = std::vector<int>(input->custCnt + 1);
	chromR = std::vector<std::vector<int>>(input->vehicleCnt);
	chromT = std::vector<int>(input->custCnt);
	if (initializeChromTAndShuffle)
	{
		shuffleChromT();
	}
}

Individual::Individual(hust::Input* input, std::string solutionStr) : input(input), isFeasible(false), biasedFitness(0)
{
	successors = std::vector<int>(input->custCnt + 1);
	predecessors = std::vector<int>(input->custCnt + 1);
	chromR = std::vector<std::vector<int>>(input->vehicleCnt);
	chromT = std::vector<int>(input->custCnt);

	std::stringstream ss(solutionStr);
	int inputCustomer;
	// Loops as long as there is an integer to read
	int pos = 0;
	int route = 0;
	while (ss >> inputCustomer)
	{
		if (inputCustomer == 0)
		{
			// Depot
			route++;
			assert(route < input->vehicleCnt);
		}
		else
		{
			chromR[route].push_back(inputCustomer);
			chromT[pos] = inputCustomer;
			pos++;
		}
	}
	assert (pos == input->custCnt);
	evaluateCompleteCost();
}

Individual::Individual(): input(nullptr), isFeasible(false), biasedFitness(0)
{
	myCostSol.penalizedCost = 1.e30;
}
