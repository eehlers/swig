
%feature("rp:group", "foo");
%feature("rp:include") %{
#include "FullLib/foo.hpp"
%}

namespace FullLib {
    class Foo {
    public:
        Foo(long f);
        long f();
    };
}
