
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_CONVERSIONS

void testConversions() {
    std::cout << std::endl;
    std::cout << "Testing conversions" << std::endl;
    std::cout << std::endl;

    std::cout << ComplexLibAddinCpp::clShowGrade(75L) << std::endl;
    try {
        std::cout << ComplexLibAddinCpp::clShowGrade(75.) << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }
}

#endif

