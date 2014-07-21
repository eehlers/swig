
#include "cpp_hw_adder2.hpp"
#include "ValueObjects/vo_hw_adder2.hpp"
#include "AddinObjects/obj_hw_adder2.hpp"
#include <boost/shared_ptr.hpp>
#include <oh/repository.hpp>
#include <oh/conversions/convert2.hpp>

static ObjectHandler::Repository repository;

namespace ObjectHandler {

    inline bool is_numeric(const std::string &s, long &l) {
        try {
            l = boost::lexical_cast<long>(s);
            return true;
        } catch(...) {
            return false;
        }
    }

    template<class container_t>
    SimpleLib::Long convertLong(const container_t& c) {
        if(c.type() == typeid(long))
            return SimpleLib::Long(c.operator long());
        else if(c.type() == typeid(std::string)) {
            std::string s = c.operator std::string();
            long l;
            if (is_numeric(s, l))
                return SimpleLib::Long(l);
        }
        OH_FAIL("unable to convert type '" << c.type().name() << "' to type 'SimpleLib::Long'");
    }

    template<> 
    SimpleLib::Long convert2<SimpleLib::Long, property_t>(const property_t& c) {
        return convertLong(c);
    }
}

//std::string SimpleLibAddin::slLong(const std::string &objectID, long x) {
//    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
//        new ValueObjects::slLong(
//            objectID, false, x));
//    boost::shared_ptr<ObjectHandler::Object> object(
//        new SimpleLibAddin::Long(
//            valueObject, false, x));
//    std::string returnValue =
//        ObjectHandler::Repository::instance().storeObject(
//            objectID, object, false, valueObject);
//    return returnValue;
//}

std::string SimpleLibAddin::slAdder2(const std::string &objectID, const ObjectHandler::property_t& x) {
    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
        new ValueObjects::slAdder2(
            objectID, false, 0));
    SimpleLib::Long x2=ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>(x);
    boost::shared_ptr<ObjectHandler::Object> object(
        new SimpleLibAddin::Adder2(
            valueObject, false, x2));
    std::string returnValue =
        ObjectHandler::Repository::instance().storeObject(
            objectID, object, false, valueObject);
    return returnValue;
}

//long SimpleLibAddin::slAdder2Add(const std::string &objectID, long y) {
//    OH_GET_REFERENCE(x, objectID, SimpleLibAddin::Adder2, SimpleLib::Adder2);
//    return x->add(y);
//}

