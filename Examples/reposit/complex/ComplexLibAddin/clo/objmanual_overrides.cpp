
#include <clo/objmanual_overrides.hpp>

std::string ComplexLibAddin::Test2::f() {
    return "ComplexLibAddin::Test2::f() " + libraryObject_->f();
}

