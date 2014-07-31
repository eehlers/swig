
#include "classes.hpp"

boost::shared_ptr<FullLib::Color> FullLibAddin::RED_Color() {
    return boost::shared_ptr<FullLib::Color>(
        new FullLib::Red);
}

boost::shared_ptr<FullLib::Color> FullLibAddin::BLUE_Color() {
    return boost::shared_ptr<FullLib::Color>(
        new FullLib::Blue);
}

