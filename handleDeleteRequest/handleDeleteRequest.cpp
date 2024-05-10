#include "../request/Client.hpp"

// if (HTTP_uri == "/cgi-bin/delete.py") {
    // HTML 파일 href 수정하고 버튼 클릭됐을 때 이동하는 && URL HTTP_uri.compare("fileDirectory") 같은 식으로 처리 계획
    //     std::remove("/Users/junsepar/42/webserv/html/8080/cgi/jackson.txt"); //// SERVER_ROOT + HTTP_uri 이런식으로 해야함
    //     response.setStatusCode(204); // 204 반환하면 신경쓸거 없이 그 페이지에 남아있음 UI변경은 알아보면 될거같음
    //     return response;
    // } 위치 잡아줘야함

// maybe Client.cpp Line: 434 정도에 위치하면 적당할듯 .

std::string generateFileList(std::string HTTP_uri) {
    std::string full;
    full += "        <ul>\n";
    if (HTTP_uri == "delete1") {
        full += "    <li><a href=\"/files/file2.jpg\">file2.jpg</a> <a href=\"delete2\" class=\"delete-link\" data-file=\"file2.jpg\">[삭제]</a></li>\n";
        full += "    <li><a href=\"/files/file3.pdf\">file3.pdf</a> <a href=\"delete3\" class=\"delete-link\" data-file=\"file3.pdf\">[삭제]</a></li>\n";
    } else if (HTTP_uri == "delete2") {
        full += "    <li><a href=\"/files/file1.jpg\">file1.jpg</a> <a href=\"delete1\" class=\"delete-link\" data-file=\"file1.jpg\">[삭제]</a></li>\n";
        full += "    <li><a href=\"/files/file3.pdf\">file3.pdf</a> <a href=\"delete3\" class=\"delete-link\" data-file=\"file3.pdf\">[삭제]</a></li>\n";
    } else {
        full += "    <li><a href=\"/files/file1.jpg\">file1.jpg</a> <a href=\"delete1\" class=\"delete-link\" data-file=\"file1.jpg\">[삭제]</a></li>\n";
        full += "    <li><a href=\"/files/file2.jpg\">file2.jpg</a> <a href=\"delete2\" class=\"delete-link\" data-file=\"file2.jpg\">[삭제]</a></li>\n";
    }
    full += "        </ul>\n";
    return full;
}

int main() {
    std::string HTTP_uri = "delete3";
    std::string fullset;
    std::string mmm = "HTTP/1.1 200 OK\n"
                "Connection: keep-alive\n"
                "Content-Length: 1787\n"
                "Content-Type: text/html; charset=utf-8\n"
                "Date: Thu, 09 May 2024 18:29:37 GMT\n"
                "Server: 42Webserv\n\n"
                "<!DOCTYPE html>\n"
                "<html lang=\"ko\">\n"
                "<head>\n"
                "    <meta charset=\"UTF-8\">\n"
                "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
                "    <title>파일 업로드 및 삭제</title>\n"
                "    <style>\n"
                "        body {\n"
                "            font-family: Arial, sans-serif;\n"
                "            background-image: url(\"ground2.png\");\n"
                "            background-color: #f2f2f2;\n"
                "            display: flex;\n"
                "            justify-content: center;\n"
                "            align-items: center;\n"
                "            height: 100vh;\n"
                "            margin: 0;\n"
                "        }\n"
                "        .container {\n"
                "            background-color: white;\n"
                "            padding: 40px;\n"
                "            border-radius: 10px;\n"
                "            box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);\n"
                "            text-align: center;\n"
                "            max-width: 600px;\n"
                "        }\n"
                "        h1 {\n"
                "            color: #333;\n"
                "            font-size: 36px;\n"
                "            margin-bottom: 20px;\n"
                "        }\n"
                "        form {\n"
                "            margin-top: 20px;\n"
                "        }\n"
                "        input[type=\"file\"] {\n"
                "            margin-bottom: 10px;\n"
                "        }\n"
                "        input[type=\"submit\"] {\n"
                "            padding: 12px 24px;\n"
                "            background-color: #333;\n"
                "            color: white;\n"
                "            border: none;\n"
                "            border-radius: 5px;\n"
                "            cursor: pointer;\n"
                "            transition: background-color 0.3s ease;\n"
                "        }\n"
                "        input[type=\"submit\"]:hover {\n"
                "            background-color: #666;\n"
                "        }\n"
                "        ul {\n"
                "            list-style-type: none;\n"
                "            padding: 0;\n"
                "            margin-top: 20px;\n"
                "        }\n"
                "        li {\n"
                "            margin-bottom: 10px;\n"
                "        }\n"
                "        a {\n"
                "            color: #333;\n"
                "            text-decoration: none;\n"
                "        }\n"
                "        a:hover {\n"
                "            text-decoration: underline;\n"
                "        }\n"
                "        .btn {\n"
                "            display: inline-block;\n"
                "            padding: 12px 24px;\n"
                "            background-color: #333;\n"
                "            color: white;\n"
                "            text-decoration: none;\n"
                "            border-radius: 5px;\n"
                "            transition: background-color 0.3s ease;\n"
                "            margin-top: 20px;\n"
                "        }\n"
                "        .btn:hover {\n"
                "            background-color: #666;\n"
                "        }\n"
                "    </style>\n"
                "</head>\n"
                "<body>\n"
                "    <div class=\"container\">\n"
                "        <h1>파일 업로드 및 삭제</h1>\n"
                "        <form action=\"/upload.py\" method=\"post\" enctype=\"multipart/form-data\">\n"
                "            <input type=\"file\" name=\"file\">\n"
                "            <input type=\"submit\" value=\"업로드\">\n"
                "        </form>\n"
                "        <h2>업로드된 파일 목록</h2>\n";
                mmm += generateFileList(HTTP_uri);

            mmm +=    
                "    </div>\n"
                "</body>\n"
                "</html>\n";

    // std::cout << mmm << std::endl;

    return 0;
}

// Response ResponseHandle::handleDeleteRequest() {
//     Response response;

// 	Response redirectResponse = handleRedirect(_loc);
// 	if (redirectResponse.getStatusCode() != OK_200) {
// 		return redirectResponse;
// 	}

//     // 인덱스 파일 설정
//     std::string index = _loc.getIndex();
//     // std::cout << __LINE__ << "     " <<  _loc.getIndex() << std::endl;
//     if (ResponseUtils::isDirectory(_filePath) && !index.empty()) {
//         _filePath += "/" + index;
//     }

//     // 파일 확장자 추출
//     std::string extension = ResponseUtils::getFileExtension(_filePath);
//     // std::cout << "Extension: " << extension << std::endl;
//     // 파일 읽기
//     std::ifstream file(_filePath.c_str(), std::ios::binary);
// 	// std::cout << "File Path: " << _filePath << std::endl;
//     if (file.is_open() && file.good()) {
//         // 파일 크기 확인
//         std::streamsize fileSize = ResponseUtils::getFileSize(file);

//         // 파일 크기 제한 설정
//         const std::streamsize maxFileSize = 10 * 1024 * 1024;
//         if (fileSize > maxFileSize) {
// 			return createErrorResponse(PayloadTooLarge_413, "The requested file is too large.");
//         }
//         // 파일 내용 읽기
//         std::string body = ResponseUtils::readFileContent(file, fileSize);
//         std::string body1 = "<!DOCTYPE html>\n"
//                 "<html lang=\"ko\">\n"
//                 "<head>\n"
//                 "    <meta charset=\"UTF-8\">\n"
//                 "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
//                 "    <title>파일 업로드 및 삭제</title>\n"
//                 "    <style>\n"
//                 "        body {\n"
//                 "            font-family: Arial, sans-serif;\n"
//                 "            background-image: url(\"ground2.png\");\n"
//                 "            background-color: #f2f2f2;\n"
//                 "            display: flex;\n"
//                 "            justify-content: center;\n"
//                 "            align-items: center;\n"
//                 "            height: 100vh;\n"
//                 "            margin: 0;\n"
//                 "        }\n"
//                 "        .container {\n"
//                 "            background-color: white;\n"
//                 "            padding: 40px;\n"
//                 "            border-radius: 10px;\n"
//                 "            box-shadow: 0 0 20px rgba(0, 0, 0, 0.1);\n"
//                 "            text-align: center;\n"
//                 "            max-width: 600px;\n"
//                 "        }\n"
//                 "        h1 {\n"
//                 "            color: #333;\n"
//                 "            font-size: 36px;\n"
//                 "            margin-bottom: 20px;\n"
//                 "        }\n"
//                 "        form {\n"
//                 "            margin-top: 20px;\n"
//                 "        }\n"
//                 "        input[type=\"file\"] {\n"
//                 "            margin-bottom: 10px;\n"
//                 "        }\n"
//                 "        input[type=\"submit\"] {\n"
//                 "            padding: 12px 24px;\n"
//                 "            background-color: #333;\n"
//                 "            color: white;\n"
//                 "            border: none;\n"
//                 "            border-radius: 5px;\n"
//                 "            cursor: pointer;\n"
//                 "            transition: background-color 0.3s ease;\n"
//                 "        }\n"
//                 "        input[type=\"submit\"]:hover {\n"
//                 "            background-color: #666;\n"
//                 "        }\n"
//                 "        ul {\n"
//                 "            list-style-type: none;\n"
//                 "            padding: 0;\n"
//                 "            margin-top: 20px;\n"
//                 "        }\n"
//                 "        li {\n"
//                 "            margin-bottom: 10px;\n"
//                 "        }\n"
//                 "        a {\n"
//                 "            color: #333;\n"
//                 "            text-decoration: none;\n"
//                 "        }\n"
//                 "        a:hover {\n"
//                 "            text-decoration: underline;\n"
//                 "        }\n"
//                 "        .btn {\n"
//                 "            display: inline-block;\n"
//                 "            padding: 12px 24px;\n"
//                 "            background-color: #333;\n"
//                 "            color: white;\n"
//                 "            text-decoration: none;\n"
//                 "            border-radius: 5px;\n"
//                 "            transition: background-color 0.3s ease;\n"
//                 "            margin-top: 20px;\n"
//                 "        }\n"
//                 "        .btn:hover {\n"
//                 "            background-color: #666;\n"
//                 "        }\n"
//                 "    </style>\n"
//                 "</head>\n"
//                 "<body>\n"
//                 "    <div class=\"container\">\n"
//                 "        <h1>파일 업로드 및 삭제</h1>\n"
//                 "        <form action=\"/upload.py\" method=\"post\" enctype=\"multipart/form-data\">\n"
//                 "            <input type=\"file\" name=\"file\">\n"
//                 "            <input type=\"submit\" value=\"업로드\">\n"
//                 "        </form>\n"
//                 "        <h2>업로드된 파일 목록</h2>\n "
//                 "    <li><a href=\"/files/file1.jpg\">file1.jpg</a> <a href=\"delete1\" class=\"delete-link\" data-file=\"file1.jpg\">[삭제]</a></li>\n"
//                 "    <li><a href=\"/files/file2.jpg\">file2.jpg</a> <a href=\"delete2\" class=\"delete-link\" data-file=\"file2.jpg\">[삭제]</a></li>\n"
//                 "    <li><a href=\"/files/file3.pdf\">file3.pdf</a> <a href=\"delete3\" class=\"delete-link\" data-file=\"file3.pdf\">[삭제]</a></li>\n"
//                 "    </div>\n"
//                 "</body>\n"
//                 "</html>\n";
//         std::remove("/Users/parkjunseo/42/webserv/html/8080/files/file1.jpg");
//         file.close();

//         response.setStatusCode(OK_200);
//         response.setHeader("Date", ResponseUtils::getCurTime());
//         response.setHeader("Content-Type", ResponseUtils::getContentType(extension));
//         response.setBody(body1);
//         response.setHeader("Content-Length", web::toString(body.length()));
//         response.setHeader("Connection", "keep-alive");

//     } else {
//         if (ResponseUtils::isDirectory(_filePath)) {
// 			if (_loc.getAutoindex() == true) {
//             	handleAutoIndex(response, _filePath);
// 			} else {
// 				return createErrorResponse(Forbidden_403, "Directory listing not allowed.");
// 			}
//         } else {
// 			if (_loc.getAutoindex() == false) {
// 				return createErrorResponse(NotFound_404, "The requested file was not found.");
// 			} else {
// 				handleAutoIndex(response, _filePath.substr(0, _filePath.find_last_of('/')));
// 			}
// 		}
//     }
//     response.setHeader("Server", "42Webserv");
//     std::cout << "ResponseDDDDDDDDDDDDDDDDDDDEEEEEEELLLLLLEEEEETTTTEEEE: " << response.getResponses() << std::endl;
//     return response;
// }
