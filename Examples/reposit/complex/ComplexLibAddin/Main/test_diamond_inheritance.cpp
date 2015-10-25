
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_DIAMOND_INHERITANCE

void testDiamondInheritance() {
    std::cout << std::endl;
    std::cout << "Testing diamond inheritance" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clFoo2(trigger, "foo2", overwrite, permanent);
    ComplexLibAddinCpp::clBar2(trigger, "bar2", overwrite, permanent);
    ComplexLibAddinCpp::clFooBar2(trigger, "foobar2", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clFoo2F(trigger, "foo2") << std::endl;
    std::cout << ComplexLibAddinCpp::clFoo2F(trigger, "foobar2") << std::endl;
    std::cout << ComplexLibAddinCpp::clBar2F(trigger, "bar2") << std::endl;
    std::cout << ComplexLibAddinCpp::clBar2F(trigger, "foobar2") << std::endl;
    std::cout << ComplexLibAddinCpp::clFunctionUsingFoo2("foo2") << std::endl;
    std::cout << ComplexLibAddinCpp::clFunctionUsingFoo2("foobar2") << std::endl;
    std::cout << ComplexLibAddinCpp::clFunctionUsingBar2("bar2") << std::endl;
    std::cout << ComplexLibAddinCpp::clFunctionUsingBar2("foobar2") << std::endl;
}

#endif

