#include "./Environment.h"

namespace hust {

Environment::Environment(const std::string& inputp) {
	this->inputPath = inputp;
};

bool Environment::setInputPath(std::string& inPath) {
	this->inputPath = inPath;
	return true;
}

void Environment::show() {
	debug(inputPath);
	debug(seed);
}
Environment::~Environment() {};

}