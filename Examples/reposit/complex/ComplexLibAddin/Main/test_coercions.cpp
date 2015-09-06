
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include <rp/addin.hpp>
#include "test_all.hpp"

#ifdef TEST_COERCIONS

void testCoercions() {
    std::cout << std::endl;
    std::cout << "Testing coercions" << std::endl;
    std::cout << std::endl;

    std::cout << ComplexLibAddinCpp::clShowGrade2(75.) << std::endl;
    std::cout << ComplexLibAddinCpp::clShowGrade2("90") << std::endl;
    try {
        std::cout << ComplexLibAddinCpp::clShowGrade2(75L) << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }
}

#endif

