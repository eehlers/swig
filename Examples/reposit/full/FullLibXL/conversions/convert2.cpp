
#include "convert2.hpp"
#include "AddinObjects/conversions/cnv_tmpl.hpp"

namespace ObjectHandler {

    template<> 
    FullLib::Long convert2<FullLib::Long, ConvertOper>(const ConvertOper& c) {
        return convertLong(c);
    }
}

