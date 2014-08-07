
#ifndef clo_conversions_coerce_grade_hpp
#define clo_conversions_coerce_grade_hpp

#include <oh/conversions/coerce.hpp>
#include <oh/exception.hpp>
#include <cl/coercions.hpp>
#include <sstream>

namespace ObjectHandler {

    bool longToGrade2(
        const ObjectHandler::property_t &in,
        ComplexLib::Grade2 &out) {

        if(in.type() == typeid(long)) {
            out = ComplexLib::Grade2(in.operator long());
            return true;
        } else {
            return false;
        }
    }

    bool stringToGrade2(
        const ObjectHandler::property_t &in,
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
        TypeIn, TypeOut> {};

    template <>
    class CoerceImpl<const ObjectHandler::property_t &, 
        ComplexLib::Grade2> : public ObjectHandler::Coerce<
        const ObjectHandler::property_t &, 
        ComplexLib::Grade2> {

        Conversion *getConversions() {
            static Conversion conversions[] = {
                longToGrade2,
                stringToGrade2,
                0
            };
            return conversions; 
        };
    };
}

#endif

