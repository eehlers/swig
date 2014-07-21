
#include <iostream>
#include "AddinCpp/cpp_hw_adder.hpp"
#include "AddinCpp/cpp_hw_adder2.hpp"

int main() {
	std::cout << "hi" << std::endl;
    std::cout << SimpleLibAddin::slFunc() << std::endl;
    SimpleLibAddin::slAdder("adder", 1);
	std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;

    SimpleLibAddin::slAdder2("adder2", 1L);

	std::cout << "bye" << std::endl;
    return 0;
}

