
#include <oh/enumerations/typefactory.hpp>
#include <clo/enumerations/register/register_types.hpp>
#include <cl/enumerated_types.hpp>

namespace ComplexLibAddin {

    void registerEnumeratedTypes() {

//        ObjectHandler::Create<ComplexLib::Account::Type> create;
//        create.registerType("Current", new ComplexLib::Account::Type(ComplexLib::Account::Current));
//        create.registerType("Savings", new ComplexLib::Account::Type(ComplexLib::Account::Savings));

        ObjectHandler::Create<ComplexLib::AccountType> create;
        create.registerType("Current", new ComplexLib::AccountType(ComplexLib::Current));
        create.registerType("Savings", new ComplexLib::AccountType(ComplexLib::Savings));

    }

    void unregisterEnumeratedTypes() {

//        ObjectHandler::Create<ComplexLib::Account::Type>().unregisterTypes();

        ObjectHandler::Create<ComplexLib::AccountType>().unregisterTypes();
    }
}

