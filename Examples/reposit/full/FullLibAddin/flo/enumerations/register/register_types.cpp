
#include <oh/enumerations/typefactory.hpp>
#include "register_types.hpp"
#include <fl/color.hpp>

namespace FullLibAddin {

    void registerEnumeratedTypes() {

        ObjectHandler::Create<FullLib::Account::Type> create;
        create.registerType("Current", new FullLib::Account::Type(FullLib::Account::Current));
        create.registerType("Savings", new FullLib::Account::Type(FullLib::Account::Savings));

    }

    void unregisterEnumeratedTypes() {

        ObjectHandler::Create<FullLib::Account::Type>().unregisterTypes();

    }
}

