
#ifndef clo_conversions_coerce_grade_hpp
#define clo_conversions_coerce_grade_hpp

#include <rp/property.hpp>
#include <rp/conversions/coerce.hpp>
#include <rp/exception.hpp>
#include <cl/coercions.hpp>
#include <sstream>

namespace reposit {

    template <class T>
    inline bool doubleToGrade2(
        T in,
        ComplexLib::Grade2 &out) {

        if(in.type() == typeid(double)) {
            out = ComplexLib::Grade2(in.operator double());
            return true;
        } else {
            return false;
        }
    }

    template <class T>
    inline bool stringToGrade2(
        T in,
        ComplexLib::Grade2 &out) {

        if(in.type() == typeid(std::string)) {
            std::istringstream ss(in.operator std::string());  
            long x;
            ss >> x;  
            if (ss.fail())
                return false;
            out = ComplexLib::Grade2(x);
            return true;
        } else {
            return false;
        }
    }

    template <class TypeIn, class TypeOut>
    class CoerceImpl : public reposit::Coerce<
        TypeIn, TypeOut> {
        typename reposit::Coerce<TypeIn, TypeOut>::Conversion *getConversions() {
            static typename reposit::Coerce<TypeIn, TypeOut>::Conversion conversions[] = {
                doubleToGrade2,
                stringToGrade2,
                0
            };
            return conversions; 
        };
    };
}

#endif

