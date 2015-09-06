
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include <rp/addin.hpp>
#include "test_all.hpp"

#ifdef TEST_ENUMERATED_CLASSES

void testEnumeratedClasses() {
    std::cout << std::endl;
    std::cout << "Testing enumerated classes" << std::endl;
    std::cout << std::endl;

    std::cout << "The current time in New York is " << ComplexLibAddinCpp::clTimeString("EST") << std::endl;
    std::cout << "The current time in Brussels is " << ComplexLibAddinCpp::clTimeString("UTC") << std::endl;
    std::cout << "The current time in Beijing is " << ComplexLibAddinCpp::clTimeString("CST") << std::endl;
    try {
        std::cout << "The current time in Wazoo is " << ComplexLibAddinCpp::clTimeString("WAZOO") << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }
}

#endif

