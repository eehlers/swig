
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/add_adder.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        SimpleLibAddinCpp::initializeAddin();
        // Call function SimpleLib::func()
        std::cout << SimpleLibAddinCpp::slFunc() << std::endl;
        // Construct an instance of class SimpleLib::Adder
        ObjectHandler::property_t trigger;
        bool overwrite = true;
        bool permanent = true;
        SimpleLibAddinCpp::slAdder(trigger, "adder", overwrite, permanent, 1);
        // Call member function SimpleLib::Adder::add()
        std::cout << "1 + 2 = " << SimpleLibAddinCpp::slAdderAdd("adder", 2) << std::endl;
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

