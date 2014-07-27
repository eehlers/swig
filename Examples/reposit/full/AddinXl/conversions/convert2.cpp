
#include "convert2.hpp"

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, ConvertOper>(const ConvertOper& c) {
        return convertLong(c);
    }
}

