
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_OVERRIDES

void testOverrides() {
    std::cout << std::endl;
    std::cout << "Testing overrides" << std::endl;
    std::cout << std::endl;

    ComplexLibAddinCpp::clTest2("my_test2");
    std::cout << ComplexLibAddinCpp::clTest2F("my_test2") << std::endl;
}

#endif

