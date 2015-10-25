
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_MULTIPLE_INHERITANCE

void testMultipleInheritance() {
    std::cout << std::endl;
    std::cout << "Testing multiple inheritance" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clFooBar(trigger, "foobar", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clFunctionUsingFoo("foobar") << std::endl;
    std::cout << ComplexLibAddinCpp::clFunctionUsingBar("foobar") << std::endl;
}

#endif

