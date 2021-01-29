#ifndef vrptwNew_ENVIROMENT_H
#define vrptwNew_ENVIROMENT_H

#include <iostream>
#include "./Flag.h"

using namespace std;

namespace vrptwNew {

class Environment
{
public:

	LL seed = -1;
	string inputPath;
	string outputPath;
	string example;

	Environment() {
		
	};
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
