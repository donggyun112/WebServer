//Client::setRequest() 함수 try 블록 에 들어가는 코드. content-type 이 chunked 이면 read_done 플래그를 다르게 먹어야 한다.

//if (HEADER_DONE  이고, getHeader["Content-Type"] == "Chunked 어쩌구" 이면 이 함수를 실행.
#include <sstream>
#include <iostream>
#include <string>
#include "../request/Client.hpp"

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