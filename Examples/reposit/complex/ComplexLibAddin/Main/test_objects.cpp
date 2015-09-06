
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_OBJECTS

void testObjects() {
    std::cout << std::endl;
    std::cout << "Testing objects" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clTest(trigger, "my_test", overwrite, permanent, 42);
    std::cout << ComplexLibAddinCpp::clTestGetInput(trigger, "my_test") << std::endl;
}

#endif

