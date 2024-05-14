#include "utils.hpp"
#include "Error.hpp"

class Response;

namespace web {
	std::string toString(int num) {
		std::stringstream ss;
		ss << num;
		return ss.str();
	}
};


