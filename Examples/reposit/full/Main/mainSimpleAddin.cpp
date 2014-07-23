
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/cpp_adder.hpp"
#include "AddinCpp/cpp_adder2.hpp"
#include "AddinCpp/cpp_color.hpp"
#include "AddinCpp/cpp_foo.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        SimpleLibAddinCpp::initializeAddin();
        std::cout << SimpleLibAddinCpp::slFunc() << std::endl;
        SimpleLibAddinCpp::slAdder("adder", 1);
        std::cout << "1 + 2 = " << SimpleLibAddinCpp::slAdderAdd("adder", 2) << std::endl;
        SimpleLibAddinCpp::slFoo("foo", 42);
        std::cout << SimpleLibAddinCpp::slFooF("foo") << std::endl;

        SimpleLibAddinCpp::slAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << SimpleLibAddinCpp::slAdder2Add("adder2", "4") << std::endl;

        try {
            SimpleLibAddinCpp::slAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        //std::cout << SimpleLibAddinCpp::slGetColorName("Red") << std::endl;
        //std::cout << SimpleLibAddinCpp::slGetColorName("x") << std::endl;
        std::cout << SimpleLibAddinCpp::slAccountTypeToString("Current") << std::endl;
        std::cout << SimpleLibAddinCpp::slAccountTypeToString("Savings") << std::endl;
        try {
            std::cout << SimpleLibAddinCpp::slAccountTypeToString("wazoo") << std::endl;
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

