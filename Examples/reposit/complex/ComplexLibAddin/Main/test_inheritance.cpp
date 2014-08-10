
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_INHERITANCE

void testInheritance() {
    std::cout << std::endl;
    std::cout << "Testing inheritance" << std::endl;
    std::cout << std::endl;

    ComplexLibAddinCpp::clBase("base");
    std::cout << ComplexLibAddinCpp::clBaseF("base") << std::endl;
    ComplexLibAddinCpp::clDerived("derived");
    std::cout << ComplexLibAddinCpp::clBaseF("derived") << std::endl;
    std::cout << ComplexLibAddinCpp::clDerivedF("derived") << std::endl;
    try {
        std::cout << ComplexLibAddinCpp::clDerivedF("base") << std::endl;
    } catch(const std::exception &e) {
        std::cout << "Expected error : " << e.what() << std::endl;
    }

    ComplexLibAddinCpp::clC("c");
    std::cout << ComplexLibAddinCpp::clAF0("c") << std::endl;
    std::cout << ComplexLibAddinCpp::clBF1("c") << std::endl;
}

#endif

