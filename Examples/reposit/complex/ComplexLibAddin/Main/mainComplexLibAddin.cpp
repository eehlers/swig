
#include <iostream>
#include "AddinCpp/all.hpp"
#include "oh/addin.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        ComplexLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << ObjectHandler::ohVersion() << std::endl;

        std::cout << ComplexLibAddinCpp::clFunc1() << std::endl;

        ComplexLibAddinCpp::closeAddin();
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

