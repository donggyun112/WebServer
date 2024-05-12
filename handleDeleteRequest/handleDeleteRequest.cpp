#include "../request/ResponseHandle.hpp"

Response ResponseHandle::handleDeleteRequest() {
    Response response;

	// Response redirectResponse = handleRedirect(_loc);
	// if (redirectResponse.getStatusCode() != 300) {
	// 	return redirectResponse;
	// } //이 부분은 redirect 구현 잘 끝나면 거기에서 에러코드 등 핸들링 추가로 합시다.

    // 파일 읽기
    std::ifstream file(_filePath.c_str(), std::ios::binary);
	// std::cout << "File Path: " << _filePath << std::endl;
    if (file.is_open() && file.good()) {
        // 파일 크기 확인
        std::streamsize fileSize = ResponseUtils::getFileSize(file);

        // 파일 크기 제한 설정
        const std::streamsize maxFileSize = 10 * 1024 * 1024;
        if (fileSize > maxFileSize) {
			return createErrorResponse(PayloadTooLarge_413, "The requested file is too large.");
        }
        std::remove(_filePath.c_str());
        file.close();

        response.setStatusCode(204);
    } else {
        if (ResponseUtils::isDirectory(_filePath)) {
			if (_loc.getAutoindex() == true) {
            	handleAutoIndex(response, _filePath);
			} else {
				return createErrorResponse(Forbidden_403, "Directory listing not allowed.");
			}
        } else {
			if (_loc.getAutoindex() == false) {
				return createErrorResponse(NotFound_404, "The requested file was not found.");
			} else {
				handleAutoIndex(response, _filePath.substr(0, _filePath.find_last_of('/')));
			}
		}
    }
    response.setHeader("Server", "42Webserv");
    return response;
}
