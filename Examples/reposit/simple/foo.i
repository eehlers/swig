
%feature("rp:group", "foo");
%feature("rp:include") %{
#include "Library/foo.hpp"
%}

namespace SimpleLib {
    class Foo {
    public:
        Foo(long f);
        long f();
    };
}
