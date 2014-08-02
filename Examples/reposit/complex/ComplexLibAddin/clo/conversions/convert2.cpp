
#include <clo/conversions/convert2.hpp>
#include <clo/conversions/cnv_tmpl.hpp>

namespace ObjectHandler {

    template<>
    ComplexLib::Long convert2<ComplexLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

