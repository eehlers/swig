
#include "sla_hw.hpp"
#include "ValueObjects/vo_hw.hpp"
#include "AddinObjects/obj_hw.hpp"
#include <boost/shared_ptr.hpp>
#include <oh/repository.hpp>

static ObjectHandler::Repository repository;

std::string SimpleLibAddin::slFunc() {
    return SimpleLib::func();
}

std::string SimpleLibAddin::slAdder(const std::string &objectID, int x) {
    boost::shared_ptr<ObjectHandler::ValueObject> valueObject(
        new ValueObjects::slAdder(
            objectID, false, x));
    boost::shared_ptr<ObjectHandler::Object> object(
        new SimpleLibAddin::Adder(
            valueObject, false, x));
    std::string returnValue =
        ObjectHandler::Repository::instance().storeObject(
            objectID, object, false, valueObject);
    return returnValue;
}

int SimpleLibAddin::slAdderAdd(const std::string &objectID, int y) {
    OH_GET_REFERENCE(x, objectID, SimpleLibAddin::Adder, SimpleLib::Adder);
    return x->add(y);
}

