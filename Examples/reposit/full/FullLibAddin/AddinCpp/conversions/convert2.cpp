
#include "convert2.hpp"
#include "flo/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<> 
    FullLib::Long convert2<FullLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

