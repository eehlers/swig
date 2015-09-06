
#include <clxl/conversions/convert2.hpp>
#include <clo/conversions/cnv_tmpl.hpp>
#include <rpxl/convert_oper.hpp>

namespace reposit {

    template<>
    ComplexLib::Grade convert2<ComplexLib::Grade, ConvertOper>(const ConvertOper& p) {
        return convertGrade(p);
    }
}
