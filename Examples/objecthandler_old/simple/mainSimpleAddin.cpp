
#include <iostream>
#include "SimpleAddin.hpp"

int main() {
	std::cout << "hi" << std::endl;
    SimpleAddin::f2();
    SimpleAddin::ohFoo("foo");
    SimpleAddin::ohFoof("foo");
	std::cout << "bye" << std::endl;
    return 0;
}

