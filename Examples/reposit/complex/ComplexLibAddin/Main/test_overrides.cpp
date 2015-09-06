
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_OVERRIDES

void testOverrides() {
    std::cout << std::endl;
    std::cout << "Testing overrides" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clTest2(trigger, "my_test2", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clTest2F(trigger, "my_test2") << std::endl;
}

#endif

