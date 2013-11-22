
#include <iostream>
#include "Library/simplelib.hpp"

int main() {
    std::cout << "hi" << std::endl;
    SimpleLib::func();
    SimpleLib::Adder a(1);
    std::cout << "1 + 2 = " << a.add(2) << std::endl;
    std::cout << "bye" << std::endl;
    return 0;
}

