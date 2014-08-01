
#ifndef addinobjects_convert2_hpp
#define addinobjects_convert2_hpp

#include <oh/property.hpp>
#include <fl/adder2.hpp>
//#include "flo/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<> 
    FullLib::Long convert2<FullLib::Long, property_t>(const property_t& c);

    //template<> 
    //inline FullLib::Long convert2<FullLib::Long, property_t>(const property_t& c) {
    //    return convertLong(c);
    //}
}

#endif

