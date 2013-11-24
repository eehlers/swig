
#include "simplelib.hpp"
#include <iostream>

std::string SimpleLib::func() {
    return "f()";
}

SimpleLib::Adder::Adder(long x) : x_(x) {}

long SimpleLib::Adder::add(long y) {
    return x_ + y;
}

