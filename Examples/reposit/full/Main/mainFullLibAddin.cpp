
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/cpp_adder.hpp"
#include "AddinCpp/cpp_adder2.hpp"
#include "AddinCpp/cpp_color.hpp"
#include "AddinCpp/cpp_foo.hpp"
#include "AddinCpp/cpp_inheritance.hpp"
#include "AddinCpp/cpp_noparm.hpp"
#include "AddinCpp/cpp_oh_repository.hpp"
#include "AddinCpp/cpp_oh_utilities.hpp"
#include "AddinCpp/cpp_oh_serialization.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        FullLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << FullLibAddinCpp::ohVersion() << std::endl;

        std::cout << FullLibAddinCpp::flFunc() << std::endl;
        FullLibAddinCpp::flAdder("adder", 1);
        std::cout << "1 + 2 = " << FullLibAddinCpp::flAdderAdd("adder", 2) << std::endl;
        FullLibAddinCpp::flFoo("foo", 42);
        std::cout << FullLibAddinCpp::flFooF("foo") << std::endl;

        FullLibAddinCpp::flAdder2("adder2", 3L);
        std::cout << "3 + 4 = " << FullLibAddinCpp::flAdder2Add("adder2", "4") << std::endl;

        try {
            FullLibAddinCpp::flAdder2("adder2", "wazoo");
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << FullLibAddinCpp::flAccountTypeToString("Current") << std::endl;
        std::cout << FullLibAddinCpp::flAccountTypeToString("Savings") << std::endl;
        try {
            std::cout << FullLibAddinCpp::flAccountTypeToString("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        std::cout << FullLibAddinCpp::flGetColorName("Red") << std::endl;
        std::cout << FullLibAddinCpp::flGetColorName("Blue") << std::endl;
        try {
            std::cout << FullLibAddinCpp::flGetColorName("wazoo") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }

        // Test constructors/members/functions that take no arguments and return void.
        FullLibAddinCpp::flF2();
        FullLibAddinCpp::flNoParm("noparm");
        FullLibAddinCpp::flNoParmF("noparm");

        // Test inheritance
        FullLibAddinCpp::flDerived("derived");
        FullLibAddinCpp::flBaseF("derived");

        // Test serialization
        // Create an object
        FullLibAddinCpp::flAdder("adder100", 100);
        // Test it
        std::cout << "100 + 1 = " << FullLibAddinCpp::flAdderAdd("adder100", 1) << std::endl;
        //Serialize it
        std::string xml = FullLibAddinCpp::ohObjectSaveString("adder100");
        std::cout << "XML = " << std::endl << xml << std::endl;
        // Delete it
        FullLibAddinCpp::ohDeleteObject("adder100");
        try {
            std::cout << "100 + 1 = " << FullLibAddinCpp::flAdderAdd("adder100", 1) << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error - " << e.what() << std::endl;
        }
        // Deserialize it
        FullLibAddinCpp::ohLoadObjectString(xml, false);
        // Test it
        std::cout << "100 + 1 = " << FullLibAddinCpp::flAdderAdd("adder100", 1) << std::endl;

        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

