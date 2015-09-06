
#include <AddinCpp/init.hpp>
#include <rp/repository.hpp>
#include <rp/processor.hpp>
#include <rp/enumerations/enumregistry.hpp>
//#include <clo/enumerations/register/register_all.hpp>
#include <clo/serialization/serializationfactory.hpp>
#include <clo/init.hpp>

void ComplexLibAddinCpp::initializeAddin() {

        // Instantiate the reposit Repository
        static reposit::Repository repository;

        //Instantiate the Processor Factory
        static reposit::ProcessorFactory processorFactory;

        // Instantiate the Serialization Factory
        static ComplexLibAddin::SerializationFactory factory;

        // Instantiate the Enumerated Type Registry
        static reposit::EnumTypeRegistry enumTypeRegistry;

        // Instantiate the Enumerated Class Registry
        static reposit::EnumClassRegistry enumClassRegistry;

        // Instantiate the Enumerated Pair Registry
        static reposit::EnumPairRegistry enumPairRegistry;

        // Initialize the Enumeration Registry
        ComplexLibAddin::initializeAddin();
}

void ComplexLibAddinCpp::closeAddin() {
    // Clear the Enumeration Registry
    ComplexLibAddin::closeAddin();
}

