
%feature("rp:group", "foo");
%feature("rp:include") %{
#include <cl/foo.hpp>
%}

namespace ComplexLib {
    class Foo {
    public:
        Foo(long f);
        long f();
    };
}
