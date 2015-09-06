
#ifndef addinobjects_convert2_hpp
#define addinobjects_convert2_hpp

#include <rp/property.hpp>
#include <cl/conversions.hpp>

namespace reposit {

    template<> 
    ComplexLib::Grade convert2<ComplexLib::Grade, property_t>(const property_t& p);

//    template<> 
//    ComplexLib::Long convert2<ComplexLib::Long, property_t>(const property_t& c);
}

#endif

