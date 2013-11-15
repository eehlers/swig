
#include <iostream>
#include "simple.hpp"

int main() {
	std::cout << "hi" << std::endl;
    Simple::f2();
    Simple::Foo f;
    f.f();
	std::cout << "bye" << std::endl;
    return 0;
}

