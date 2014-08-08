
#include <iostream>
#include "AddinCpp/add_all.hpp"
#include "oh/addin.hpp"

int main() {
    try {
        std::cout << "hi" << std::endl;
        ComplexLibAddinCpp::initializeAddin();
        std::cout << "ObjectHandler version = " << ObjectHandler::ohVersion() << std::endl;

//        std::cout << ComplexLibAddinCpp::clFunc1() << std::endl;
//
//        ComplexLibAddinCpp::clEchoVoid();
//        std::cout << ComplexLibAddinCpp::clEchoBool(true) << std::endl;
//        std::cout << ComplexLibAddinCpp::clEchoLong(42) << std::endl;
//        std::cout << ComplexLibAddinCpp::clEchoDouble(123.456) << std::endl;
//        std::cout << ComplexLibAddinCpp::clEchoCharStar("hello") << std::endl;
//        std::cout << ComplexLibAddinCpp::clEchoString("test") << std::endl;

//        ComplexLibAddinCpp::clTest("my_test", 42);
//        std::cout << ComplexLibAddinCpp::clTestGetInput("my_test") << std::endl;

//        // Test inheritance
//
//        ComplexLibAddinCpp::clBase("base");
//        std::cout << ComplexLibAddinCpp::clBaseF("base") << std::endl;
//        ComplexLibAddinCpp::clDerived("derived");
//        std::cout << ComplexLibAddinCpp::clBaseF("derived") << std::endl;
//        std::cout << ComplexLibAddinCpp::clDerivedF("derived") << std::endl;
//        try {
//            std::cout << ComplexLibAddinCpp::clDerivedF("base") << std::endl;
//        } catch(const std::exception &e) {
//            std::cout << "Error : " << e.what() << std::endl;
//        }
//
//        ComplexLibAddinCpp::clC("c");
//        std::cout << ComplexLibAddinCpp::clAF0("c") << std::endl;
//        std::cout << ComplexLibAddinCpp::clBF1("c") << std::endl;

//        // Test typedefs
//        std::cout << ComplexLibAddinCpp::clDoubleToString(123.456) << std::endl;

//        // Test conversions
//        std::cout << ComplexLibAddinCpp::clShowGrade(75L) << std::endl;
//        try {
//            std::cout << ComplexLibAddinCpp::clShowGrade(75.) << std::endl;
//        } catch(const std::exception &e) {
//            std::cout << "Error : " << e.what() << std::endl;
//        }

//        // Test coercions
//        std::cout << ComplexLibAddinCpp::clShowGrade2(75L) << std::endl;
//        std::cout << ComplexLibAddinCpp::clShowGrade2("90") << std::endl;
//        try {
//            std::cout << ComplexLibAddinCpp::clShowGrade2(75.) << std::endl;
//        } catch(const std::exception &e) {
//            std::cout << "Error : " << e.what() << std::endl;
//        }

        // Test enumerated types
        std::cout << "Interest rate current = " << ComplexLibAddinCpp::clGetInterestRate("Current") << std::endl;
        std::cout << "Interest rate savings = " << ComplexLibAddinCpp::clGetInterestRate("SAVINGS") << std::endl;
        try {
            std::cout << "Interest rate xyz = " << ComplexLibAddinCpp::clGetInterestRate("xyz") << std::endl;
        } catch(const std::exception &e) {
            std::cout << "Error : " << e.what() << std::endl;
        }

        ComplexLibAddinCpp::closeAddin();
        std::cout << "bye" << std::endl;
        return 0;
    } catch(const std::exception &e) {
        std::cout << "Error : " << e.what() << std::endl;
    } catch(...) {
        std::cout << "Error" << std::endl;
    }
}

