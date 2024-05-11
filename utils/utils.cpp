#include "utils.hpp"


namespace web {
	std::string toString(int num) {
		std::stringstream ss;
		ss << num;
		return ss.str();
	}
}