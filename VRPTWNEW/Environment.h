#ifndef vrptwNew_ENVIROMENT_H
#define vrptwNew_ENVIROMENT_H

#include <iostream>
#include "./Flag.h"
#include "./Utility.h"

using namespace std;

namespace hust {

class Environment
{
public:

	unsigned seed = 0;
	string inputPath = "";
	string outputPath = "../Results/";

	Environment(const string& inputp) {
		this->inputPath = inputp;
	};

	bool setInputPath(string& inPath) {
		this->inputPath = inPath;
		return true;
	}

	void show() {
		debug(inputPath);
		debug(seed);
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
