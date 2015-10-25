
#ifndef complex_lib_diamond_inheritance_hpp
#define complex_lib_diamond_inheritance_hpp

// Test diamond inheritance.

#include <string>
#include <boost/shared_ptr.hpp>

namespace ComplexLib {

    // An example of multiple inheritance.

    class Foo2 {
    public:
        virtual std::string f() const { return "ComplexLib::Foo2::f()"; }
        virtual ~Foo2() {}
    };

    class Bar2 {
    public:
        virtual std::string f() const { return "ComplexLib::Bar2::f()"; }
        virtual ~Bar2() {}
    };

    class FooBar2 : public Foo2, public Bar2 {
    public:
        virtual std::string f() const { return "ComplexLib::FooBar2::f()"; }
        virtual ~FooBar2() {}
    };

    std::string functionUsingFoo2(const boost::shared_ptr<Foo2> &f);

    std::string functionUsingBar2(const boost::shared_ptr<Bar2> &b);
};

#endif

