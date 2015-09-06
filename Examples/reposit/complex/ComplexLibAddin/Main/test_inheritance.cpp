
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_INHERITANCE

void testInheritance() {
    std::cout << std::endl;
    std::cout << "Testing inheritance" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clBase(trigger, "base", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clBaseF(trigger, "base") << std::endl;
    ComplexLibAddinCpp::clDerived(trigger, "derived", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clBaseF(trigger, "derived") << std::endl;
    std::cout << ComplexLibAddinCpp::clDerivedF(trigger, "derived") << std::endl;
    try {
        std::cout << ComplexLibAddinCpp::clDerivedF(trigger, "base") << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }

    ComplexLibAddinCpp::clC(trigger, "c", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clAF0(trigger, "c") << std::endl;
    std::cout << ComplexLibAddinCpp::clBF1(trigger, "c") << std::endl;
}

#endif

