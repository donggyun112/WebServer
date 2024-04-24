#ifndef UTILS_HPP
#define UTILS_HPP
#include <iostream>





typedef int FD;
typedef unsigned int Port;
typedef int Status;

enum STATE { SUCCESS=0, FAILURE=-1 };

#include <map>

// --> 가비지 컬렉터
// 사용법
// 	GarbageCollector<Socket> gc;
//  SmartPointer<Socket> tt(new Socket(), gc);
//  Socket *sock = tt.get(); or tt->socket();
//  FD fd = tt->socket();

#endif