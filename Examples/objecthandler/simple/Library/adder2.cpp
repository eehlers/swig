
#include "adder2.hpp"
#include <iostream>
#include <cstdlib>

SimpleLib::Long::Long(long x) : x_(x) {
}

SimpleLib::Long::Long(std::string x) {
    x_ = atoi(x.c_str());
}

long SimpleLib::Long::x() {
    return x_;
}

SimpleLib::Adder2::Adder2(long x) : x_(x/*.x()*/) {
}

long SimpleLib::Adder2::add(long y) {
    return x_ + y;//.x();
}

