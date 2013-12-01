
#include <iostream>
#include "AddinCpp/cpp_adder.hpp"

int main() {
	std::cout << "hi" << std::endl;
    std::cout << SimpleLibAddin::slFunc() << std::endl;
    SimpleLibAddin::slAdder("adder", 1);
	std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;
	std::cout << "bye" << std::endl;
    return 0;
}

