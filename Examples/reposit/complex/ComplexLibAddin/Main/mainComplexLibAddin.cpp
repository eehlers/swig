
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;

        ComplexLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << ObjectHandler::ohVersion() << std::endl;

        testFunctions();
        testObjects();
        testInheritance();
        testTypedefs();
        testConversions();
        testCoercions();
        testEnumeratedTypes();
        testEnumeratedClasses();

        ComplexLibAddinCpp::closeAddin();

        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << "Error : " << e.what() << std::endl;
    } catch(...) {
        std::cout << "Unhandled error" << std::endl;
    }
}

