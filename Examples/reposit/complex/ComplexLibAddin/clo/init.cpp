
#include <clo/init.hpp>
#include <oh/repository.hpp>
#include <oh/processor.hpp>
#include <oh/enumerations/enumregistry.hpp>
//#include <clo/enumerations/register/register_all.hpp>
//#include <clo/serialization/serializationfactory.hpp>
#include <clo/enumerations/register/register_types.hpp>
#include <clo/enumerations/register/register_classes.hpp>

void ComplexLibAddin::initializeAddin() {

        // Initialize the Enumeration Registry
        registerEnumeratedTypes();
        registerEnumeratedClasses();
}

void ComplexLibAddin::closeAddin() {

        // Clear the Enumeration Registry
        unregisterEnumeratedTypes();
}

