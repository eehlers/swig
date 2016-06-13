
#include <iostream>
#include <string>
#include "AddinCpp/add_all.hpp"
#include "rp/addin.hpp"
#include "test_all.hpp"

#ifdef TEST_DOCSTRINGS

void testDocstrings() {
    std::cout << std::endl;
    std::cout << "Testing docstrings" << std::endl;
    std::cout << std::endl;

    reposit::property_t trigger;
    bool overwrite = true;
    bool permanent = true;

    ComplexLibAddinCpp::clDoc1(trigger, "my_doc1", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clDoc1Sum(trigger, "my_doc1", 1, 2) << std::endl;

    ComplexLibAddinCpp::clDocFunc1("echo");

    ComplexLibAddinCpp::clDoc2(trigger, "my_doc2", overwrite, permanent);
    std::cout << ComplexLibAddinCpp::clDoc2Sum(trigger, "my_doc2", 1, 2) << std::endl;

    ComplexLibAddinCpp::clDocFunc2("echo");
}

#endif

