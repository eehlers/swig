
#include "obj_oh_repository.hpp"
#include <oh/repository.hpp>

void FullLibAddin::deleteObject(std::string const &objectID) {
    ObjectHandler::Repository::instance().deleteObject(objectID);
}

