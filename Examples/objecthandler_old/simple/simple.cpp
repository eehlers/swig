
#include "simple.hpp"
#include <iostream>

void Simple::f2() {
    std::cout << "f()" << std::endl;
}

void Simple::Foo::f() {
    std::cout << "Simple::Foo::f()" << std::endl;
}

