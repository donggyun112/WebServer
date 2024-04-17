

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