
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include <rp/addin.hpp>
#include "test_all.hpp"

#ifdef TEST_ENUMERATED_TYPES

void testEnumeratedTypes() {
    std::cout << std::endl;
    std::cout << "Testing enumerated types" << std::endl;
    std::cout << std::endl;

    std::cout << "Interest rate current = " << ComplexLibAddinCpp::clGetInterestRate("Current") << std::endl;
    std::cout << "Interest rate savings = " << ComplexLibAddinCpp::clGetInterestRate("SAVINGS") << std::endl;
    try {
        std::cout << "Interest rate xyz = " << ComplexLibAddinCpp::clGetInterestRate("xyz") << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }

    std::cout << "Interest rate current = " << ComplexLibAddinCpp::clGetInterestRate2("Current2") << std::endl;
    std::cout << "Interest rate savings = " << ComplexLibAddinCpp::clGetInterestRate2("SAVINGS2") << std::endl;
    try {
        std::cout << "Interest rate xyz = " << ComplexLibAddinCpp::clGetInterestRate2("xyz") << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }
}

#endif

