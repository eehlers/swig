
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/cpp_adder.hpp"
#include "AddinCpp/cpp_adder2.hpp"
#include "AddinCpp/cpp_color.hpp"
#include "AddinCpp/cpp_foo.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        SimpleLibAddin::initializeAddin();
        std::cout << SimpleLibAddin::slFunc() << std::endl;
        SimpleLibAddin::slAdder("adder", 1);
        std::cout << "1 + 2 = " << SimpleLibAddin::slAdderAdd("adder", 2) << std::endl;
        SimpleLibAddin::slFoo("foo", 42);
        std::cout << SimpleLibAddin::slFooF("foo") << std::endl;

        SimpleLibAddin::slAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << SimpleLibAddin::slAdder2Add("adder2", "4") << std::endl;

        try {
            SimpleLibAddin::slAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        //std::cout << SimpleLibAddin::slGetColorName("Red") << std::endl;
        //std::cout << SimpleLibAddin::slGetColorName("x") << std::endl;
        std::cout << SimpleLibAddin::slAccountTypeToString("Current") << std::endl;
        std::cout << SimpleLibAddin::slAccountTypeToString("Savings") << std::endl;
        try {
            std::cout << SimpleLibAddin::slAccountTypeToString("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

