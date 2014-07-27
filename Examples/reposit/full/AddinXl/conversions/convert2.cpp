
#include "convert2.hpp"
#include "AddinObjects/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, ConvertOper>(const ConvertOper& c) {
        return convertLong(c);
    }
}

