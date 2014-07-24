
#include <oh/enumerations/typefactory.hpp>
#include "enumerations.hpp"
#include <Library/color.hpp>

namespace SimpleLibAddin {

    void registerEnumeratedTypes() {

        ObjectHandler::Create<SimpleLib::Color> create;
        create.registerType("Red", new SimpleLib::Red);
        create.registerType("Blue", new SimpleLib::Blue);

        ObjectHandler::Create<SimpleLib::Account::Type> create;
        create.registerType("Current", new SimpleLib::Account::Type(SimpleLib::Account::Current));
        create.registerType("Savings", new SimpleLib::Account::Type(SimpleLib::Account::Savings));

    }

    void unregisterEnumeratedTypes() {

        ObjectHandler::Create<SimpleLib::Color>().unregisterTypes();
        ObjectHandler::Create<SimpleLib::Account::Type>().unregisterTypes();

    }
}

