
#include "init.hpp"
#include <oh/repository.hpp>
#include <oh/processor.hpp>
#include <oh/enumerations/enumregistry.hpp>
//#include <qlo/enumerations/register/register_all.hpp>
//#include <qlo/serialization/serializationfactory.hpp>
#include <AddinObjects/enumerations/register/register_types.hpp>
#include <AddinObjects/enumerations/register/register_classes.hpp>

void SimpleLibAddin::initializeAddin() {

        // Initialize the Enumeration Registry
        registerEnumeratedTypes();
        registerEnumeratedClasses();
}
