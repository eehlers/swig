
#include <clo/enumerations/constructors/classes.hpp>

boost::shared_ptr<ComplexLib::Color> ComplexLibAddin::RED_Color() {
    return boost::shared_ptr<ComplexLib::Color>(
        new ComplexLib::Red);
}

boost::shared_ptr<ComplexLib::Color> ComplexLibAddin::BLUE_Color() {
    return boost::shared_ptr<ComplexLib::Color>(
        new ComplexLib::Blue);
}

