
#include <oh/enumerations/typefactory.hpp>
#include "enum_reg_classes.hpp"
#include "enum_ctor_classes.hpp"
#include "enum_factories.hpp"

namespace SimpleLibAddin {

    void registerEnumeratedClasses() {

        ObjectHandler::Create<boost::shared_ptr<SimpleLib::Color> > create;
        create.registerType("Red", reinterpret_cast<void*>(RED_Color));
        create.registerType("Blue", reinterpret_cast<void*>(BLUE_Color));

    }

    void unregisterEnumeratedClasses() {

        //ObjectHandler::Create<SimpleLib::Color>().unregisterTypes();

    }
}

