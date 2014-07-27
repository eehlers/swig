
#ifndef addincpp_convert2_hpp
#define addincpp_convert2_hpp

#include <boost/lexical_cast.hpp>
#include <ohxl/convert_oper.hpp>
#include <Library/adder2.hpp>

namespace ObjectHandler {

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, ConvertOper>(const ConvertOper& c);
}

#endif


