
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_TYPEDEFS

void testTypedefs() {
    std::cout << std::endl;
    std::cout << "Testing typedefs" << std::endl;
    std::cout << std::endl;

    std::cout << ComplexLibAddinCpp::clDoubleToString(123.456) << std::endl;
    std::cout << ComplexLibAddinCpp::clLongDoubleToString(123.456) << std::endl;
}

#endif

