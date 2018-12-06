#include <iostream>
#include "rsa/rsa.h"

int main() {
	std::cout << "hello, world" <<std::endl;
	rsa::EncryptDevice<int> d();
	rsa::Exception e("test");
	std::cout << e.what() << std::endl;	
	return 0;
}
