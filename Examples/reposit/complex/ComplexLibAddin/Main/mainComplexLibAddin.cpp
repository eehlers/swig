
#include <iostream>
#include "AddinCpp/init.hpp"
#include "AddinCpp/add_functions.hpp"
#include "AddinCpp/add_echo.hpp"
#include "oh/addin.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        ComplexLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << ObjectHandler::ohVersion() << std::endl;

        std::cout << ComplexLibAddinCpp::clFunc1() << std::endl;

        ComplexLibAddinCpp::clEchoVoid();
        std::cout << ComplexLibAddinCpp::clEchoBool(true) << std::endl;
        std::cout << ComplexLibAddinCpp::clEchoLong(42) << std::endl;
        std::cout << ComplexLibAddinCpp::clEchoDouble(123.456) << std::endl;
        std::cout << ComplexLibAddinCpp::clEchoCharStar("hello") << std::endl;
        std::cout << ComplexLibAddinCpp::clEchoString("test") << std::endl;

        ComplexLibAddinCpp::closeAddin();
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    } catch(...) {
        std::cout << "error" << std::endl;
    }
}

