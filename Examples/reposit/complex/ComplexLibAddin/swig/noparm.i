
%feature("rp:group", "noparm");
%feature("rp:include") %{
#include <cl/noparm.hpp>
%}

namespace ComplexLib {
    void func2();
    class NoParm {
    public:
        NoParm();
        void f();
    };
}

