
#ifndef cpp_hw_adder2_hpp
#define cpp_hw_adder2_hpp

#include <string>
#include <oh/property.hpp>

namespace SimpleLibAddin {
    std::string slAdder2(const std::string &objectID, const ObjectHandler::property_t& x);
    long slAdder2Add(const std::string &objectID, const ObjectHandler::property_t& y);
}

#endif

