
#ifndef addincpp_convert2_hpp
#define addincpp_convert2_hpp

#include <oh/property.hpp>
#include <Library/adder2.hpp>

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, property_t>(const property_t& c);
}

#endif

