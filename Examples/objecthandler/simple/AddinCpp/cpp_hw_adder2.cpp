
#include "cpp_hw_adder2.hpp"
#include "ValueObjects/vo_hw_adder2.hpp"
#include "AddinObjects/obj_hw_adder2.hpp"
#include <boost/shared_ptr.hpp>
#include <oh/repository.hpp>

static ObjectHandler::Repository repository;

std::string SimpleLibAddin::slLong(const std::string &objectID, long x) {
    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
        new ValueObjects::slLong(
            objectID, false, x));
    boost::shared_ptr<ObjectHandler::Object> object(
        new SimpleLibAddin::Long(
            valueObject, false, x));
    std::string returnValue =
        ObjectHandler::Repository::instance().storeObject(
            objectID, object, false, valueObject);
    return returnValue;
}

std::string SimpleLibAddin::slAdder2(const std::string &objectID, long x) {
    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
        new ValueObjects::slAdder2(
            objectID, false, x));
    boost::shared_ptr<ObjectHandler::Object> object(
        new SimpleLibAddin::Adder2(
            valueObject, false, x));
    std::string returnValue =
        ObjectHandler::Repository::instance().storeObject(
            objectID, object, false, valueObject);
    return returnValue;
}

long SimpleLibAddin::slAdder2Add(const std::string &objectID, long y) {
    OH_GET_REFERENCE(x, objectID, SimpleLibAddin::Adder2, SimpleLib::Adder2);
    return x->add(y);
}

