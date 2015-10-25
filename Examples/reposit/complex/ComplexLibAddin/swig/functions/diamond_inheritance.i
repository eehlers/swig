
%group(diamond_inheritance);
%override

%insert(diamond_inheritance_library_hpp) %{
#include <cl/diamond_inheritance.hpp>
%}

namespace ComplexLib {

    class Foo2 {
    public:
        Foo2();
        std::string f() const;
    };

    class Bar2 {
    public:
        Bar2();
        std::string f() const;
    };

    class FooBar2 : public Foo2, public Bar2 {
    public:
        FooBar2();
    };

    std::string functionUsingFoo2(const boost::shared_ptr<Foo2> &f);

    std::string functionUsingBar2(const boost::shared_ptr<Bar2> &b);
}

