
// --> NullPtr
class NullPtr {
public:
    NullPtr() {} // public 생성자

    // 임의의 타입으로 변환하는 operator
    template<typename T>
    operator T*() const
    {
        return NULL;
    }

    // 임의의 클래스 멤버 포인터로 변환하는 operator
    template<typename C, typename T>
    operator T C::*() const
    {
        return NULL;
    }

private:
    // NullPtr의 주소를 가져올 수 없도록 하는 private 멤버 함수
    void operator&() const;
};

NullPtr nullPtr; // 전역 객체 정의

typedef int FD;
typedef int Port;

enum Status { SUCCESS=0, FAILURE=-1 };

#include <map>

// --> 가비지 컬렉터
// 사용법
// 	GarbageCollector<Socket> gc;
//  SmartPointer<Socket> tt(new Socket(), gc);
//  Socket *sock = tt.get(); or tt->socket();
//  FD fd = tt->socket();

template <typename T>
class GarbageCollector {
private:
    std::map<T*, int> referenceCount;

public:
    void addReference(T* object) {
        referenceCount[object]++;
    }

    void removeReference(T* object) {
        if (--referenceCount[object] == 0) {
            delete object;
            referenceCount.erase(object);
        }
    }

    void clear() {
        for (typename std::map<T*, int>::iterator it = referenceCount.begin(); it != referenceCount.end(); ++it) {
            delete it->first;
        }
        referenceCount.clear();
    }
};

template <typename T>
class SmartPointer {
private:
    T* object;
    GarbageCollector<T>& gc;

public:
    SmartPointer(T* obj, GarbageCollector<T>& collector) : object(obj), gc(collector) {
        gc.addReference(object);
    }

    SmartPointer(const SmartPointer& other) : object(other.object), gc(other.gc) {
        gc.addReference(object);
    }

    ~SmartPointer() {
        gc.removeReference(object);
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