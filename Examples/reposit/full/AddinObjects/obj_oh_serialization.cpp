
#include "obj_oh_serialization.hpp"
#include <oh/repository.hpp>
#include <oh/serializationfactory.hpp>

std::string FullLibAddin::objectSaveString(std::string const &objectId) {
    OH_GET_OBJECT(x, objectId, ObjectHandler::Object)
    return ObjectHandler::SerializationFactory::instance().saveObjectString(x);
}

void FullLibAddin::loadObjectString(std::string const &xml, bool overwriteExisting) {
    ObjectHandler::SerializationFactory::instance().loadObjectString(xml, overwriteExisting);
}

