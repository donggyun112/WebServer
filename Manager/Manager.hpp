#ifndef MANAGER_HPP
# define MANAGER_HPP
# include "../request/NResponseUtils.hpp"
# include "../utils/utils.hpp"
# include "../request/Request.hpp"

class Manager {
	public:
		Manager();
		~Manager();
		static ResponseUtils responseUtils;
		static Utils utils;
		static RequestUtils requestUtils;
		static std::vector<std::string> sessionStorage;
		static void addSessionId(const std::string& sid) {
			sessionStorage.push_back(sid);
		}
		static void removeSession(const std::string& session) {
			std::vector<std::string>::iterator it = std::find(sessionStorage.begin(), sessionStorage.end(), session);
			if (it != sessionStorage.end()) {
				sessionStorage.erase(it);
			}
		}
		static std::string findSession(const std::string &session) {
			if (session == "") {
				return "";
			}
			std::vector<std::string>::iterator it = std::find(sessionStorage.begin(), sessionStorage.end(), session);
			if (it != sessionStorage.end()) return *it;
			else return "";
		}
};
#endif
