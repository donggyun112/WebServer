#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include <sys/wait.h>

#include "../request/RequestHandle.hpp"
#include "../request/ResponseHandle.hpp"
#include "../request/Response.hpp"
#include "../Parse/Config.hpp"
#include "../Parse/LocationConfig.hpp"

std::string handleFormData(std::string cgiPath);

std::string getPart(const std::string& body, const std::string& boundary) {
    size_t start = body.find(boundary);
    if (start == std::string::npos) {
        return "";
    }
    start += boundary.length();
    size_t end = body.find(boundary, start);
    if (end == std::string::npos) {
        return "";
    }
    return body.substr(start, end - start);
}

std::string getfileContent(const std::string &body) {
    size_t start = body.find("\r\n\r\n");
    if (start == std::string::npos) {
        return "";
    }
    start += 4;
    return body.substr(start);
}

std::string getBodyHeader(const std::string& part) {
    size_t end = part.find("\r\n\r\n");
    if (end == std::string::npos) {
        return "";
    }
    return part.substr(0, end);
}

std::string getType(const std::string& header) {
    size_t start = header.find("Content-Type: ");
    if (start == std::string::npos) {
        return "";
    }
    start += 14;
    size_t end = header.find("\r\n", start);
    return header.substr(start, end - start);
}

std::string getFileName(const std::string& header) {
    // Content-Disposition 헤더에서 파일 이름 파싱
    size_t start = header.find("filename=\"");
    if (start != std::string::npos) {
        start += 10;
        size_t end = header.find("\"", start);
        return header.substr(start, end - start);
    }
    return "";
}

std::string getBoundary(const std::string& header) {
    size_t start = header.find("boundary=");
    if (start == std::string::npos) {
        return "";
    }
    start += 9;
    return header.substr(start);
}

std::string getContentType(std::string &header)
{
    size_t start = header.find("Content-Type: ");
    if (start == std::string::npos)
        return "";
    start += 14;
    size_t end = header.find(";", start);
    return header.substr(start, end - start);
}

Response ResponseHandle::handlePostRequest() {
	Response response;
    
    // Response redirectResponse = handleRedirect(_loc);
	// if (redirectResponse.getStatusCode() != OK_200) {
	// 	return redirectResponse;
	// }

    std::string contentType = getHeader("Content-Type");
    if (contentType.find("multipart/form-data") != std::string::npos) {
        
        const std::string part = getPart(_body, getBoundary(getHeader("Content-Type")));
        const std::string bodyHeader = getBodyHeader(part);

        const std::string fileContent = getfileContent(part);
        if (fileContent.empty())
            return createErrorResponse(BadRequest_400, "Bad Request");
        const std::streamsize maxFileSize = 10 * 1024 * 1024;
        if (fileContent.size() > maxFileSize)
            return createErrorResponse(UriTooLong_414, "Body too large");

        if (!fileContent.empty()) { //content가 있을 시
            std::string responseBody = handleFormData(_filePath);
            if (responseBody.empty()) {
                return createErrorResponse(InternalServerError_500, "Internal Server Error");
            }

            std::string fileName = getFileName(bodyHeader);
            if (fileName.empty()) {
                return createErrorResponse(InternalServerError_500, "Internal Server Error");
            }
            std::ofstream file(fileName);
            if (!file) {
                return createErrorResponse(InternalServerError_500, "Internal Server Error");
            }

            response.setStatusCode(Created_201);
            response.setHeader("Date", ResponseUtils::getCurTime());
            response.setHeader("Content-Type", "text/plain");
            response.setHeader("Content-Length", "0");
            response.setHeader("connection", "keep-alive");
            response.setBody(responseBody);
        }
    } else {
        return createErrorResponse(BadRequest_400, "Bad Request");
    }
    response.setHeader("Server", "42Webserv");
    return response;
}

std::string handleFormData(std::string cgiPath) {
    int cgiInput[2]; // -> pipe가 아니라 file로 바꿔야 했던거같은데 max 크기 정해놔서 괜찮지 않을까?
    pid_t pid;

    if (pipe(cgiInput) < 0)
        throw std::runtime_error("pipe error");
    
    if ((pid = fork()) < 0)
        throw std::runtime_error("fork error");

    if (pid == 0) {
        close(cgiInput[0]);
        dup2(cgiInput[1], STDOUT_FILENO);
        close(cgiInput[1]);

        // std::string pythonPath = "/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/site-packages";
        // std::string pythonEnv = "PYTHONPATH=" + pythonPath;
        // char* envp[] = {(char*)pythonEnv.c_str(), NULL};
        // 혹시 몰라 일단 남겨둠 -> python 실행 해보고 필요해지면 추가 할 예정

        std::string py3 = "/usr/bin/python3";
        char* const arg[] = {(char *)py3.c_str(), (char *)cgiPath.c_str(), NULL};

        if (execve(py3.c_str(), arg, NULL) == -1) {
            perror("execve failed");
            exit(1);
        }
    } else {
        close(cgiInput[1]);
        wait(NULL);

        std::string output;
        char buf[1024];
        ssize_t bytesRead;
        while ((bytesRead = read(cgiInput[0], buf, sizeof(buf))) > 0) {
            output.append(buf, bytesRead);
        }
        close(cgiInput[0]);
        std::cout << output;
        return output;
    }
    return "";
}

int main() {
    std::string requestBody = "..."; // POST 요청 본문

    // handlePostRequest(requestBody);

    return 0;
}

//txt
//multipart/form-data 그냥 받고 (파일 업로드)
//application/x-www-form-urlencoded -> 400(bad request)