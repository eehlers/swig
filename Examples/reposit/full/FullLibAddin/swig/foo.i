
%feature("rp:group", "foo");
%feature("rp:include") %{
#include <fl/foo.hpp>
%}

namespace FullLib {
    class Foo {
    public:
        Foo(long f);
        long f();
    };
}
