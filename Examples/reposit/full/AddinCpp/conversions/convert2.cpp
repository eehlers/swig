
#include "convert2.hpp"

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

