
#include "adder2.hpp"
#include <iostream>
#include <cstdlib>

FullLib::Long::Long(long x) : x_(x) {
}

FullLib::Long::Long(std::string x) {
    x_ = atoi(x.c_str());
}

long FullLib::Long::x() {
    return x_;
}

FullLib::Adder2::Adder2(Long x) : x_(x.x()) {
}

long FullLib::Adder2::add(Long y) {
    return x_ + y.x();
}

