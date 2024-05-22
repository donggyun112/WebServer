#include "../request/ResponseHandle.hpp"
#include "../request/NResponseUtils.hpp"

std::string ResponseHandle::handleDeleteRequest() {
    Response tmpResponse;
    std::string response;

    std::ifstream file(_filePath.c_str(), std::ios::binary);
    if (file.is_open() && file.good()) {
        std::streamsize fileSize = Manager::responseUtils.getFileSize(file);

        const std::streamsize maxFileSize = 10 * 1024 * 1024;
        if (fileSize > maxFileSize) {
			throw PayloadTooLarge_413;
        }
        std::remove(_filePath.c_str());
        file.close();

        tmpResponse.setStatusCode(204);
    } else {
        if (Manager::responseUtils.isDirectory(_filePath)) {
			if (_loc.getAutoindex() == true) {
            	handleAutoIndex(tmpResponse, _filePath);
			} else {
				throw Forbidden_403;
			}
        } else {
			if (_loc.getAutoindex() == false) {
                throw NotFound_404;
			} else {
				handleAutoIndex(tmpResponse, _filePath.substr(0, _filePath.find_last_of('/')));
			}
		}
    }
    tmpResponse.setHeader("Server", "42Webserv");
    response = tmpResponse.getResponses();
    return response;
}
