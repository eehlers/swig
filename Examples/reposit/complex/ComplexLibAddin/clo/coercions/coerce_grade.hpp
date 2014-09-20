
#ifndef clo_conversions_coerce_grade_hpp
#define clo_conversions_coerce_grade_hpp

#include <oh/property.hpp>
#include <oh/conversions/coerce.hpp>
#include <oh/exception.hpp>
#include <cl/coercions.hpp>
#include <sstream>

namespace ObjectHandler {

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
    class CoerceImpl : public ObjectHandler::Coerce<
        TypeIn, TypeOut> {
        typename ObjectHandler::Coerce<TypeIn, TypeOut>::Conversion *getConversions() {
            static typename ObjectHandler::Coerce<TypeIn, TypeOut>::Conversion conversions[] = {
                doubleToGrade2,
                stringToGrade2,
                0
            };
            return conversions; 
        };
    };
}

#endif

