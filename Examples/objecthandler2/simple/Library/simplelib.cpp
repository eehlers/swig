
#include "simplelib.hpp"
#include <iostream>

void SimpleLib::func() {
    std::cout << "f()" << std::endl;
}

SimpleLib::Adder::Adder(int x) : x_(x) {}

int SimpleLib::Adder::add(int y) {
    return x_ + y;
}

