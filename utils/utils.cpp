#include "utils.hpp"


namespace web {
	std::string toString(int num) {
		std::stringstream ss;
		ss << num;
		return ss.str();
	}
}

std::string Error::errorHandler(const ServerConfig &Serv, int num) {
		std::string fileName;
		std::string testPath;
		Response response;

		fileName = "/" + web::toString(num) + ".html";
		testPath = Serv.getPath() + Serv.getErrorPagePath() + fileName; 
		std::ifstream file(testPath.c_str());
		
		if (file.is_open() && file.good()) {
        	// 파일 크기 확인
        	std::streamsize fileSize = ResponseUtils::getFileSize(file);

	        // 파일 크기 제한 설정
	        const std::streamsize maxFileSize = 10 * 1024 * 1024;
	        if (fileSize > maxFileSize) {
				throw PayloadTooLarge_413;
	        }
	        // 파일 내용 읽기
	        std::string body = ResponseUtils::readFileContent(file, fileSize);
	        file.close();

	        response.setStatusCode(OK_200);
	        response.setHeader("Date", ResponseUtils::getCurTime());
	        response.setHeader("Content-Type", ResponseUtils::getContentType("text/html; charset=utf-8"));
	        response.setBody(body);
	        response.setHeader("Content-Length", web::toString(body.length()));
	        response.setHeader("Connection", "keep-alive");
			return response.getResponses();
		}
		else 
			return Error::createErrorResponse(num);
	}
