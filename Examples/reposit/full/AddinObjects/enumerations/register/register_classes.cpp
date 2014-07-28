
#include <oh/enumerations/typefactory.hpp>
#include "register_classes.hpp"
#include "AddinObjects/enumerations/constructors/classes.hpp"
#include "AddinObjects/enumerations/factories/all.hpp"

namespace FullLibAddin {

    void registerEnumeratedClasses() {

        ObjectHandler::Create<boost::shared_ptr<FullLib::Color> > create;
        create.registerType("Red", reinterpret_cast<void*>(RED_Color));
        create.registerType("Blue", reinterpret_cast<void*>(BLUE_Color));

    }

    void unregisterEnumeratedClasses() {

        //ObjectHandler::Create<FullLib::Color>().unregisterTypes();

    }
}

