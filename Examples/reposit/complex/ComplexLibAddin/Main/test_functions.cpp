
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include <rp/addin.hpp>
#include "test_all.hpp"

#ifdef TEST_FUNCTIONS

void testFunctions() {
    std::cout << std::endl;
    std::cout << "Testing functions" << std::endl;
    std::cout << std::endl;

    std::cout << ComplexLibAddinCpp::clFunc1() << std::endl;
    ComplexLibAddinCpp::clFunc2(123L);
    ComplexLibAddinCpp::clFunc2(reposit::property_t());

//    ComplexLibAddinCpp::clEchoVoid();
//    std::cout << ComplexLibAddinCpp::clEchoBool(true) << std::endl;
//    std::cout << ComplexLibAddinCpp::clEchoLong(42) << std::endl;
//    std::cout << ComplexLibAddinCpp::clEchoDouble(123.456) << std::endl;
//    std::cout << ComplexLibAddinCpp::clEchoCharStar("hello") << std::endl;
//    std::cout << ComplexLibAddinCpp::clEchoString("test") << std::endl;
}

#endif

