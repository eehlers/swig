
//#include <clxl/conversions/convert2.hpp>
#include "conversions/convert2.hpp"//FIXME
#include <clo/conversions/cnv_tmpl.hpp>
#include <ohxl/convert_oper.hpp>

namespace ObjectHandler {

    template<>
    ComplexLib::Grade convert2<ComplexLib::Grade, ConvertOper>(const ConvertOper& p) {
        return convertGrade(p);
    }
}
