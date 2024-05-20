#include "../request/ResponseHandle.hpp"
#include "../request/NResponseUtils.hpp"

std::string ResponseHandle::handleDeleteRequest() {
    Response tmpResponse;
    std::string response;

	// Response redirectResponse = handleRedirect(_loc);
	// if (redirectResponse.getStatusCode() != 300) {
	// 	return redirectResponse;
	// } //이 부분은 redirect 구현 잘 끝나면 거기에서 에러코드 등 핸들링 추가로 합시다.

    // 파일 읽기
    std::ifstream file(_filePath.c_str(), std::ios::binary);
    if (file.is_open() && file.good()) {
        // 파일 크기 확인
        std::streamsize fileSize = Manager::responseUtils.getFileSize(file);

        // 파일 크기 제한 설정
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
