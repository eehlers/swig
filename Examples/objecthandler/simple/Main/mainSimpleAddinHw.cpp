
#include <iostream>
#include "AddinCpp/sla_hw.hpp"

int main() {
	std::cout << "hi" << std::endl;
    SimpleLibAddin::slFunc();
    SimpleLibAddin::slAdder("adder", 1);
	std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;
	std::cout << "bye" << std::endl;
    return 0;
}

