#include "utils.hpp"
#include "Error.hpp"

std::string Utils::toString(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}


Utils::Utils() {
}

Utils::~Utils() {
}
