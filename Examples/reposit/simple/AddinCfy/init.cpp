
#include "init.hpp"
#include <rp/repository.hpp>
//#include <rp/processor.hpp>
//#include <rp/enumerations/enumregistry.hpp>
//#include <qlo/enumerations/register/register_all.hpp>
//#include <qlo/serialization/serializationfactory.hpp>
//#include "AddinObjects/init.hpp"

void initializeAddin() {

        // Instantiate the reposit Repository
        static reposit::Repository repository;

        //Instantiate the Processor Factory
        //static reposit::ProcessorFactory processorFactory;

        // Instantiate the Serialization Factory
        //static SimpleLibAddin::SerializationFactory factory;

        // Instantiate the Enumerated Type Registry
        //static reposit::EnumTypeRegistry enumTypeRegistry;

        // Instantiate the Enumerated Class Registry
        //static reposit::EnumClassRegistry enumClassRegistry;

        // Instantiate the Enumerated Pair Registry
        //static reposit::EnumPairRegistry enumPairRegistry;

        // Initialize the Enumeration Registry
        //SimpleLibAddin::initializeAddin();
}

