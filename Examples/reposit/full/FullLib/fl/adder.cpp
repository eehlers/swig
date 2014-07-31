
#include <fl/adder.hpp>
#include <iostream>

std::string FullLib::func() {
    return "f()";
}

FullLib::Adder::Adder(long x) : x_(x) {}

long FullLib::Adder::add(long y) {
    return x_ + y;
}

