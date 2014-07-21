
#include "cpp_hw_adder2.hpp"
#include "ValueObjects/vo_hw_adder2.hpp"
#include "AddinObjects/obj_hw_adder2.hpp"
#include "AddinCpp/conversions.hpp"
#include <boost/shared_ptr.hpp>
#include <oh/repository.hpp>

static ObjectHandler::Repository repository;

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

long SimpleLibAddin::slAdder2Add(const std::string &objectID, const ObjectHandler::property_t& y) {
    OH_GET_REFERENCE(x, objectID, SimpleLibAddin::Adder2, SimpleLib::Adder2);
    SimpleLib::Long y2=ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>(y);
    return x->add(y2);
}

