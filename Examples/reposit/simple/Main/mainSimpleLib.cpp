
#include <iostream>
#include "Library/adder.hpp"

int main() {
    std::cout << "hi" << std::endl;
    std::cout << SimpleLib::func() << std::endl;
    SimpleLib::Adder a(1);
    std::cout << "1 + 2 = " << a.add(2) << std::endl;
    std::cout << "bye" << std::endl;
    return 0;
}
