
#include <iostream>
#include "SimpleAddin.hpp"

int main() {
	std::cout << "hi" << std::endl;
    std::cout << SimpleAddin::f2() << std::endl;
    SimpleAddin::ohFoo("foo");
    SimpleAddin::ohFoof("foo");
	std::cout << "bye" << std::endl;
    return 0;
}

