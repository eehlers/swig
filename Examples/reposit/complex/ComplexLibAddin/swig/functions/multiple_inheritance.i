
%group(multiple_inheritance);

%insert(multiple_inheritance_library_hpp) %{
#include <cl/multiple_inheritance.hpp>
%}

namespace ComplexLib {

    class FooBar {
    public:
        FooBar();
    };

    std::string functionUsingFoo(const boost::shared_ptr<Foo> &f);

    std::string functionUsingBar(const boost::shared_ptr<Bar> &b);
}

