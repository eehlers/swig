
#include <oh/enumerations/typefactory.hpp>
#include "enum_reg_types.hpp"
#include <Library/color.hpp>

namespace SimpleLibAddin {

    void registerEnumeratedTypes() {

        ObjectHandler::Create<SimpleLib::Account::Type> create;
        create.registerType("Current", new SimpleLib::Account::Type(SimpleLib::Account::Current));
        create.registerType("Savings", new SimpleLib::Account::Type(SimpleLib::Account::Savings));

    }

    void unregisterEnumeratedTypes() {

        ObjectHandler::Create<SimpleLib::Account::Type>().unregisterTypes();

    }
}

