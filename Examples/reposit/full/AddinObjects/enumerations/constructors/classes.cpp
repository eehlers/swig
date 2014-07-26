
#include "classes.hpp"

boost::shared_ptr<SimpleLib::Color> SimpleLibAddin::RED_Color() {
    return boost::shared_ptr<SimpleLib::Color>(
        new SimpleLib::Red);
}

boost::shared_ptr<SimpleLib::Color> SimpleLibAddin::BLUE_Color() {
    return boost::shared_ptr<SimpleLib::Color>(
        new SimpleLib::Blue);
}

