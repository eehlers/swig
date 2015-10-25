
#include <cl/diamond_inheritance.hpp>

std::string ComplexLib::functionUsingFoo2(const boost::shared_ptr<Foo2> &f) {
    return f->f();
}

std::string ComplexLib::functionUsingBar2(const boost::shared_ptr<Bar2> &b) {
    return b->f();
}
