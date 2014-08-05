
#include <iostream>
#include "AddinCpp/all.hpp"
#include "oh/addin.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        ComplexLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << ObjectHandler::ohVersion() << std::endl;

        std::cout << ComplexLibAddinCpp::clFunc() << std::endl;
        ComplexLibAddinCpp::clAdder("adder", 1);
        std::cout << "1 + 2 = " << ComplexLibAddinCpp::clAdderAdd("adder", 2) << std::endl;
        ComplexLibAddinCpp::clFoo("foo", 42);
        std::cout << ComplexLibAddinCpp::clFooF("foo") << std::endl;

        ComplexLibAddinCpp::clAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << ComplexLibAddinCpp::clAdder2Add("adder2", "4") << std::endl;

        try {
            ComplexLibAddinCpp::clAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << ComplexLibAddinCpp::clAccountTypeToString("Current") << std::endl;
        std::cout << ComplexLibAddinCpp::clAccountTypeToString("Savings") << std::endl;
        try {
            std::cout << ComplexLibAddinCpp::clAccountTypeToString("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << ComplexLibAddinCpp::clGetColorName("Red") << std::endl;
        std::cout << ComplexLibAddinCpp::clGetColorName("Blue") << std::endl;
        try {
            std::cout << ComplexLibAddinCpp::clGetColorName("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        // Test constructors/members/functions that take no arguments and return void.
        ComplexLibAddinCpp::clFunc2();
        ComplexLibAddinCpp::clNoParm("noparm");
        ComplexLibAddinCpp::clNoParmF("noparm");

        // Test inheritance
        ComplexLibAddinCpp::clDerived("derived");
        ComplexLibAddinCpp::clBaseF("derived");

        // Test serialization
        // Create an object
        ComplexLibAddinCpp::clAdder("adder100", 100);
        // Test it
        std::cout << "100 + 1 = " << ComplexLibAddinCpp::clAdderAdd("adder100", 1) << std::endl;
        //Serialize it
        std::string xml = ObjectHandler::ohObjectSaveString("adder100");
        std::cout << "XML = " << std::endl << xml << std::endl;
        // Delete it
        ObjectHandler::ohDeleteObject("adder100");
        try {
            std::cout << "100 + 1 = " << ComplexLibAddinCpp::clAdderAdd("adder100", 1) << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }
        // Deserialize it
        ObjectHandler::ohLoadObjectString(xml, false);
        // Test it
        std::cout << "100 + 1 = " << ComplexLibAddinCpp::clAdderAdd("adder100", 1) << std::endl;

        ComplexLibAddinCpp::closeAddin();
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

