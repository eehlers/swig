
#include <cl/docstrings.hpp>
#include <iostream>

long ComplexLib::Doc1::sum(long a, long b) const {
    return a+b;
}

long ComplexLib::Doc2::sum(long a, long b) const {
    return a+b;
}

void ComplexLib::docFunc1(const std::string &s) {
    std::cout << "docFunc1() " << s << std::endl;
}

void ComplexLib::docFunc2(const std::string &s) {
    std::cout << "docFunc2() " << s << std::endl;
}

