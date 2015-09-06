
#include <rp/enumerations/typefactory.hpp>
#include <clo/enumerations/register/register_classes.hpp>
#include <clo/enumerations/constructors/classes.hpp>
#include <clo/enumerations/factories/all.hpp>

namespace ComplexLibAddin {

    void registerEnumeratedClasses() {

        reposit::Create<boost::shared_ptr<ComplexLib::TimeZone> > create;
        create.registerType("EST", reinterpret_cast<void*>(TimeZone_EST));
        create.registerType("UTC", reinterpret_cast<void*>(TimeZone_UTC));
        create.registerType("CST", reinterpret_cast<void*>(TimeZone_CST));
    }

    void unregisterEnumeratedClasses() {

        reposit::Create<ComplexLib::TimeZone>().unregisterTypes();
    }
}

