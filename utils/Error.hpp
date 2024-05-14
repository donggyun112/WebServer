#ifndef ERROR_HPP
# define ERROR_HPP
# include <map>
# include <string>
# include "utils.hpp"
# include "../request/Response.hpp"
# include "../request/RequestHandle.hpp"
# include "../request/ResponseHandle.hpp"
# include "../request/NResponseUtils.hpp"

class ServerConfig;

class Error {
	public:
	    static std::map<int, std::string> errors;
		static std::map<int, std::string> errorResponseBodyStrings;
		
		Error();
		
		static void 		initializeError();
		static std::string 	errorHandler(const ServerConfig &Serv, int num);
		static std::string 	createErrorResponse(int code);
		
		const std::string& operator[](int status) const;

		static std::string getError(int status);
};
#endif
