
#include <cl/adder2.hpp>
//#include <iostream>
#include <cstdlib>

ComplexLib::Long::Long(long x) : x_(x) {
}

ComplexLib::Long::Long(std::string x) {
    x_ = atoi(x.c_str());
}

long ComplexLib::Long::x() {
    return x_;
}

ComplexLib::Adder2::Adder2(Long x) : x_(x.x()) {
}

long ComplexLib::Adder2::add(Long y) {
    return x_ + y.x();
}

