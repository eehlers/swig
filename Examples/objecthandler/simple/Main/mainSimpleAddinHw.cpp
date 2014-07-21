
#include <iostream>
#include "AddinCpp/cpp_hw_adder.hpp"
#include "AddinCpp/cpp_hw_adder2.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        std::cout << SimpleLibAddin::slFunc() << std::endl;
        SimpleLibAddin::slAdder("adder", 1);
        std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;

        SimpleLibAddin::slAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << SimpleLibAddin::slAdder2Add("adder2", "4") << std::endl;

        try {
            SimpleLibAddin::slAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << e.what() << std::endl;
        }

        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

