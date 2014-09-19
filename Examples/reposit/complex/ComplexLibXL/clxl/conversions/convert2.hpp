
#ifndef addinxll_convert2_hpp
#define addinxll_convert2_hpp

#include <oh/property.hpp>
#include <cl/conversions.hpp>

namespace ObjectHandler {

    class ConvertOper;

    template<> 
    ComplexLib::Grade convert2<ComplexLib::Grade, ConvertOper>(const ConvertOper& p);
}

#endif
