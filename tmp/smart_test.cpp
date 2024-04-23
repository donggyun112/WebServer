#include "../socket/socket.hpp"

class SmartPointerBase {
public:
	std::vector<SmartPointer<Socket> > smartPointers;
	SmartPointerBase() {}
	~SmartPointerBase() {}
	void makeSocket() {
		for (int i = 0; i < 10; i++) {
			SmartPointer<Socket> tt(new Socket());
			tt.get()->autoActivate();
			smartPointers.push_back(tt);
		}
	}
};

int main() {
	SmartPointerBase spb;
	spb.makeSocket();
	
	return 0;
}
