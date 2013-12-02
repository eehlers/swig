
#include <iostream>
#include "AddinCpp/cpp_adder.hpp"
#include "AddinCpp/cpp_foo.hpp"

int main() {
	std::cout << "hi" << std::endl;
    std::cout << SimpleLibAddin::slFunc() << std::endl;
    SimpleLibAddin::slAdder("adder", 1);
	std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;
    SimpleLibAddin::slFoo("foo", 42);
	std::cout << SimpleLibAddin::slFooF("foo") << std::endl;
	std::cout << "bye" << std::endl;
    return 0;
}

