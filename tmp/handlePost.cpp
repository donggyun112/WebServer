#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <cstdlib>

#include <sys/wait.h>

#include "../request/Client.hpp"
#include "../request/Response.hpp"
#include "../Parse/Config.hpp"
#include "../Parse/LocationConfig.hpp"


// body길이 넘으면 414 (uri too long)
//버튼 들어오는거 데이터 확인
//succes 201 created


const int MAX_REQUEST_SIZE = 1024 * 1024; // 1MB

std::string parseFileName(const std::string& header) {
    // Content-Disposition 헤더에서 파일 이름 파싱
    size_t start = header.find("filename=\"");
    if (start != std::string::npos) {
        start += 10;
        size_t end = header.find("\"", start);
        return header.substr(start, end - start);
    }
    return "";
}

// std::string parseBody() --> 파일 업로드 처리 1 boundary로 구분


Response Client::handlePostRequest(const Config &Conf) {
	Response response;
	const std::string server_root = Conf[_port].getPath();
	std::string uri = getUri();
	// std::string fileName()
    // 요청 본문 크기 검사
	// std::string 

    if (getBody().length() > MAX_REQUEST_SIZE)
		return createErrorResponse(UriTooLong_414, "Body too large");

	// if (uri.find(".") == std::string::npos) ==> folder 인 경우 index붙여서 뭐 고려해보고
	// {
	// 	//error
	// 	// handleFormData(partHeader, getBody());
	// } else
	if (uri.find(".") != std::string::npos && \
			uri.substr(uri.find_last_of(".") + 1) == "py")
	{	
		LocationConfig loc;
		try {
			loc = Conf[_port].getLocation(uri);
		}
		catch (std::exception &e) {
			loc = Conf[_port].getLocation("/");
		}
		std::string cgipath = server_root + loc.getFastcgiPass() + uri;
		// open x -> 404 calculator.py 존재?
		handleFormData(cgipath);
	}
	//else if (getHeader("Content-Disposition") == "file") {
		// 파일 업로드 처리
		// std::string fileName = parseFileName(getHeader("Content-Disposition"));
		// handleFileUpload(fileName, getBody());
		// std::ofstream file("uploaded_data.txt", std::ios::binary);
		// file << getBody();
		// file.close();
		// response.setStatusCode(Created_201);
		// response.setHeader("Date", Utils::getCurTime());
		// response.setHeader("Content-Type", "text/plain");
		// response.setHeaders("Content-Length", "0");

	// }
    // }

    // 리소스 처리 (예: 파일 생성)
/*
void    Client::handleChunkedMessage(std::string &str) {
    std::stringstream oss(str);
    std::string line;
    int length;
    std::getline(oss, line);
    length = myToString(line);
    if (length == 0) _readStatus = READ_HEADER_DONE;
    else if (length > 0) {
        std::getline(oss, line);
        _request._body += line;
        _readStatus = READ_BODY_DOING;
    } else throw 404;
}
*/
    // 응답 생성
    // std::string message = "HTTP/1.1 201 Created\r\n";
    // message += "Content-Type: text/plain\r\n";
    // message += "Content-Length: 0\r\n";
    // message += "\r\n";

    // std::cout << message << std::endl;

    return response;
}

void handleFormData(std::string cgiPath) {
    int cgiInput[2];
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
    }
}
void handleFileUpload(const std::string& header, const std::string& body) {
    // 파일 업로드 처리 로직
    std::string fileName = parseFileName(header);
    std::ofstream file(fileName.c_str(), std::ios::binary);
    file.write(body.c_str(), body.length());
    file.close();
    std::cout << "File uploaded: " << fileName << std::endl;
}

int main() {
    std::string requestBody = "..."; // POST 요청 본문

    // handlePostRequest(requestBody);

    return 0;
}

//txt
//multipart/form-data 그냥 받고 (파일 업로드)
//application/x-www-form-urlencoded -> 400(bad request)