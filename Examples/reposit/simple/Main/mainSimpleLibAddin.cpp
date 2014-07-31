
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/add_adder.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        SimpleLibAddinCpp::initializeAddin();
        std::cout << SimpleLibAddinCpp::slFunc() << std::endl;
        SimpleLibAddinCpp::slAdder("adder", 1);
        std::cout << "1 + 2 = " << SimpleLibAddinCpp::slAdderAdd("adder", 2) << std::endl;
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

