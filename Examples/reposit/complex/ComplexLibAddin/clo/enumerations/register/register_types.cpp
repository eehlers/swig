
#include <oh/enumerations/typefactory.hpp>
#include <clo/enumerations/register/register_types.hpp>
#include <cl/color.hpp>

namespace ComplexLibAddin {

    void registerEnumeratedTypes() {

        ObjectHandler::Create<ComplexLib::Account::Type> create;
        create.registerType("Current", new ComplexLib::Account::Type(ComplexLib::Account::Current));
        create.registerType("Savings", new ComplexLib::Account::Type(ComplexLib::Account::Savings));

    }

    void unregisterEnumeratedTypes() {

        ObjectHandler::Create<ComplexLib::Account::Type>().unregisterTypes();

    }
}

