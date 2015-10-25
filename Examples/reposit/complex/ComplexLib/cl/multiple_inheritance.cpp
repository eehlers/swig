
#include <cl/multiple_inheritance.hpp>

std::string ComplexLib::functionUsingFoo(const boost::shared_ptr<Foo> &f) {
    return f->f();
}

std::string ComplexLib::functionUsingBar(const boost::shared_ptr<Bar> &b) {
    return b->f();
}
