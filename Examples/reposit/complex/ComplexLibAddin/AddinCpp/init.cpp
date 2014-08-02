
#include <AddinCpp/init.hpp>
#include <oh/repository.hpp>
#include <oh/processor.hpp>
#include <oh/enumerations/enumregistry.hpp>
//#include <clo/enumerations/register/register_all.hpp>
#include <clo/serialization/serializationfactory.hpp>
#include <clo/init.hpp>

void ComplexLibAddinCpp::initializeAddin() {

        // Instantiate the ObjectHandler Repository
        static ObjectHandler::Repository repository;

        //Instantiate the Processor Factory
        static ObjectHandler::ProcessorFactory processorFactory;

        // Instantiate the Serialization Factory
        static ComplexLibAddin::SerializationFactory factory;

        // Instantiate the Enumerated Type Registry
        static ObjectHandler::EnumTypeRegistry enumTypeRegistry;

        // Instantiate the Enumerated Class Registry
        static ObjectHandler::EnumClassRegistry enumClassRegistry;

        // Instantiate the Enumerated Pair Registry
        static ObjectHandler::EnumPairRegistry enumPairRegistry;

        // Initialize the Enumeration Registry
        ComplexLibAddin::initializeAddin();
}

void ComplexLibAddinCpp::closeAddin() {
    // Clear the Enumeration Registry
    ComplexLibAddin::initializeAddin();
}

