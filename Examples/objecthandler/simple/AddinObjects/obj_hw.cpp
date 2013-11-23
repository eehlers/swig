
#include "obj_hw.hpp"

std::string SimpleLibAddin::func() {
    return SimpleLib::func();
}

SimpleLibAddin::Adder::Adder(
    const boost::shared_ptr<ObjectHandler::ValueObject>& properties,
    bool permanent,
    int x)
    : ObjectHandler::LibraryObject<SimpleLib::Adder>(properties, permanent) {
    libraryObject_ = boost::shared_ptr<SimpleLib::Adder>(new SimpleLib::Adder(x));
}

