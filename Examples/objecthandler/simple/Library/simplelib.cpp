
#include "simplelib.hpp"
#include <iostream>

std::string SimpleLib::func() {
    return "f()";
}

SimpleLib::Adder::Adder(int x) : x_(x) {}

int SimpleLib::Adder::add(int y) {
    return x_ + y;
}

