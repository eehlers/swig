
#ifndef addinobjects_conversions_cnv_tmpl_hpp
#define addinobjects_conversions_cnv_tmpl_hpp

// This file contains some templates common to AddinObjects and AddinXl.
// This file should be #included only by AddinXxx/conversions/xxx.cpp.

#include <cl/conversions.hpp>
#include <boost/lexical_cast.hpp>

namespace ObjectHandler {

    template<class container_t>
    ComplexLib::Grade convertGrade(const container_t& c) {
        if(c.type() == typeid(long))
            return ComplexLib::Grade(c.operator long());
        else
            OH_FAIL("unable to convert type '" << c.type().name() << "' to type 'ComplexLib::Grade'");
    }

//    inline bool is_numeric(const std::string &s, long &l) {
//        try {
//            l = boost::lexical_cast<long>(s);
//            return true;
//        } catch(...) {
//            return false;
//        }
//    }
//
//    template<class container_t>
//    ComplexLib::Long convertLong(const container_t& c) {
//        if(c.type() == typeid(long))
//            return ComplexLib::Long(c.operator long());
//        else if(c.type() == typeid(std::string)) {
//            std::string s = c.operator std::string();
//            long l;
//            if (is_numeric(s, l))
//                return ComplexLib::Long(l);
//            else
//                OH_FAIL("unable to convert string '" << s << "' to type 'ComplexLib::Long'");
//        }
//        OH_FAIL("unable to convert type '" << c.type().name() << "' to type 'ComplexLib::Long'");
//    }
}

#endif

