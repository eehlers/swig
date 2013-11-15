
#include <iostream>
#include "simpleAddinTest.hpp"

int main() {
	std::cout << "hi" << std::endl;
    SimpleAddin::rpFoo("foo");
    SimpleAddin::rpFooF("foo");
	std::cout << "bye" << std::endl;
    return 0;
}

