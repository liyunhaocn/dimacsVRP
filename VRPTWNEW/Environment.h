#ifndef vrptwNew_ENVIROMENT_H
#define vrptwNew_ENVIROMENT_H

#include <iostream>
#include "./Flag.h"
#include "./Utility.h"

using namespace std;

namespace vrptwNew {

class Environment
{
public:

	LL seed = -1;
	string inputPath;
	string sinRecPath = "../Data/bestRec.txt";
	string basePath = "../Data/homberger_";
	string outputPath = "../Results/";
	string sinBKSPath = "../Data/";
	string linBKSPath = "../Data/";
	string example;

	Environment(string ex) {

		this->example = ex;
		
		MyString ms;
		vector<string> mssp = ms.split(this->example, "_");
		string customersNum = mssp[1] + "00";
		 
		this->inputPath = basePath + customersNum + "_customer_instances/" 
			+ this->example + ".TXT";

	};

	Environment() {

		this->example = "C1_8_2";
		MyString ms;
		vector<string> mssp = ms.split(this->example, "_");
		string customersNum = mssp[1] + "00";
		this->inputPath = basePath + customersNum + "_customer_instances/"
			+ this->example + ".TXT";

	};

	bool setExample(string ex) {

		this->example = ex;

		MyString ms;
		vector<string> mssp = ms.split(this->example, "_");
		string customersNum = mssp[1] + "00";

		this->inputPath = basePath + customersNum + "_customer_instances/"
			+ this->example + ".TXT";
		return true;
	}

	~Environment() {};

private:

};

}

//Environment::Environment()
//{
//}
//
//Environment::~Environment()
//{
//}


#endif // !vrptwNew_ENVIROMENT_H
