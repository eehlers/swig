
#include <iostream>
#include "AddinCpp/cpp_hw_adder.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        std::cout << SimpleLibAddin::slFunc() << std::endl;
        SimpleLibAddin::slAdder("adder", 1);
        std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

