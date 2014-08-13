
#include <oh/enumerations/typefactory.hpp>
#include <clo/enumerations/register/register_classes.hpp>
#include <clo/enumerations/constructors/classes.hpp>
#include <clo/enumerations/factories/all.hpp>

namespace ComplexLibAddin {

    void registerEnumeratedClasses() {

        ObjectHandler::Create<boost::shared_ptr<ComplexLib::TimeZone> > create;
        create.registerType("EST", reinterpret_cast<void*>(TimeZone_EST));
        create.registerType("UTC", reinterpret_cast<void*>(TimeZone_UTC));
        create.registerType("CST", reinterpret_cast<void*>(TimeZone_CST));
    }

    void unregisterEnumeratedClasses() {

        ObjectHandler::Create<ComplexLib::TimeZone>().unregisterTypes();
    }
}

