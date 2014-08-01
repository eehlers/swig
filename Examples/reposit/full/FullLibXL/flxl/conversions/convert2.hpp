
#ifndef addinxl_convert2_hpp
#define addinxl_convert2_hpp

#include <boost/lexical_cast.hpp>
#include <ohxl/convert_oper.hpp>
#include <fl/adder2.hpp>

namespace ObjectHandler {

    template<> 
    FullLib::Long convert2<FullLib::Long, ConvertOper>(const ConvertOper& c);
}

#endif


