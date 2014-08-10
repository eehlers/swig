
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

void testObjects() {
    std::cout << std::endl;
    std::cout << "Testing objects" << std::endl;
    std::cout << std::endl;

    ComplexLibAddinCpp::clTest("my_test", 42);
    std::cout << ComplexLibAddinCpp::clTestGetInput("my_test") << std::endl;
}

