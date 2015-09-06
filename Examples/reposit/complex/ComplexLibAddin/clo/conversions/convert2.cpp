
#include <clo/conversions/convert2.hpp>
#include <clo/conversions/cnv_tmpl.hpp>

namespace reposit {

    template<>
    ComplexLib::Grade convert2<ComplexLib::Grade, property_t>(const property_t& p) {
        return convertGrade(p);
    }

//    template<>
//    ComplexLib::Long convert2<ComplexLib::Long, property_t>(const property_t& c) {
//        return convertLong(c);
//    }
}

