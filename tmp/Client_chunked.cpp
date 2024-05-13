//Client::setRequest() 함수 try 블록 에 들어가는 코드. content-type 이 chunked 이면 read_done 플래그를 다르게 먹어야 한다.

//if (HEADER_DONE  이고, getHeader["Content-Type"] == "Chunked 어쩌구" 이면 이 함수를 실행.
#include <sstream>
#include <iostream>
#include <string>
#include "../request/Client.hpp"

void    RequestHandle::handleChunkedMessage(std::string &chunkedBody) {
    std::stringstream oss(chunkedBody);
    std::string line;
    std::string hex = "0123456789abcdef";
    int length;

    while (true) {
        std::getline(oss, line);
        length = hex[atoi(line.c_str()) % 16];
        if (length == 0) _readStatus = READ_DONE;

        std::getline(oss, line);
        _request._body += line.substr(0, length);
    }
}