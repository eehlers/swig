
#include "obj_hw_adder2.hpp"

SimpleLibAddin::Adder2::Adder2(
    const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    bool permanent,
    SimpleLib::Long x)
    : ObjectHandler::LibraryObject<SimpleLib::Adder2>(properties, permanent) {
    libraryObject_ = boost::shared_ptr<SimpleLib::Adder2>(new SimpleLib::Adder2(x));
}

