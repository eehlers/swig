
#include "convert2.hpp"
#include "AddinObjects/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

