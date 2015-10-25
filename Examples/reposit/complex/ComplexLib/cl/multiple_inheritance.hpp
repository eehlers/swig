
#ifndef complex_lib_multiple_inheritance_hpp
#define complex_lib_multiple_inheritance_hpp

// Test multiple inheritance.

#include <string>
#include <boost/shared_ptr.hpp>

namespace ComplexLib {

    // An example of multiple inheritance.

    class Foo {
    public:
        std::string f() const { return "ComplexLib::Foo::f()"; }
        virtual ~Foo() {}
    };

    class Bar {
    public:
        std::string f() const { return "ComplexLib::Bar::f()"; }
        virtual ~Bar() {}
    };

    class FooBar : public Foo, public Bar {};

    std::string functionUsingFoo(const boost::shared_ptr<Foo> &f);

    std::string functionUsingBar(const boost::shared_ptr<Bar> &b);
};

#endif

