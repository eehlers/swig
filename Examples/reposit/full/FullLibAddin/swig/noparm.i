
%feature("rp:group", "noparm");
%feature("rp:include") %{
#include <fl/noparm.hpp>
%}

namespace FullLib {
    void func2();
    class NoParm {
    public:
        NoParm();
        void f();
    };
}

