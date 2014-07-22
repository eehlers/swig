
%feature("oh:group", "foo");
%feature("oh:include") %{
#include "Library/foo.hpp"
%}

namespace SimpleLib {
    class Foo {
    public:
        Foo(long f);
        long f();
    };
}
