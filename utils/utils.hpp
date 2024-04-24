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

template <typename T>
class SmartPointer {
private:
    T* object;
    static std::map<T*, int> referenceCount;

public:
    SmartPointer(T* obj) : object(obj) {
        referenceCount[object]++;
    }

    SmartPointer(const SmartPointer& other) : object(other.object) {
        referenceCount[object]++;
    }

    ~SmartPointer() {
        if (--referenceCount[object] == 0) {
            delete object;
            referenceCount.erase(object);
        }
    }

    T* operator->() {
        return object;
    }

    T& operator*() {
        return *object;
    }

    T* get() {
        return object;
    }
};

template <typename T>
std::map<T*, int> SmartPointer<T>::referenceCount;

#endif
