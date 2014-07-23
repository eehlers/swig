
#include "init.hpp"
#include <oh/repository.hpp>
#include <oh/processor.hpp>
#include <oh/enumerations/enumregistry.hpp>
//#include <qlo/enumerations/register/register_all.hpp>
//#include <qlo/serialization/serializationfactory.hpp>
#include "enumerations.hpp"

void SimpleLibAddin::initializeAddin() {

        // Initialize the Enumeration Registry
        registerEnumeratedTypes();
}

