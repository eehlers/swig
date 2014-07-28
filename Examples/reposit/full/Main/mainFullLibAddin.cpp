
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/cpp_adder.hpp"
#include "AddinCpp/cpp_adder2.hpp"
#include "AddinCpp/cpp_color.hpp"
#include "AddinCpp/cpp_foo.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        FullLibAddinCpp::initializeAddin();
        std::cout << FullLibAddinCpp::slFunc() << std::endl;
        FullLibAddinCpp::slAdder("adder", 1);
        std::cout << "1 + 2 = " << FullLibAddinCpp::slAdderAdd("adder", 2) << std::endl;
        FullLibAddinCpp::slFoo("foo", 42);
        std::cout << FullLibAddinCpp::slFooF("foo") << std::endl;

        FullLibAddinCpp::slAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << FullLibAddinCpp::slAdder2Add("adder2", "4") << std::endl;

        try {
            FullLibAddinCpp::slAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << FullLibAddinCpp::slAccountTypeToString("Current") << std::endl;
        std::cout << FullLibAddinCpp::slAccountTypeToString("Savings") << std::endl;
        try {
            std::cout << FullLibAddinCpp::slAccountTypeToString("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << FullLibAddinCpp::slGetColorName("Red") << std::endl;
        std::cout << FullLibAddinCpp::slGetColorName("Blue") << std::endl;
        try {
            std::cout << FullLibAddinCpp::slGetColorName("wazoo") << std::endl;
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

