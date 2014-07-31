
%feature("rp:group", "noparm");
%feature("rp:include") %{
#include "FullLib/noparm.hpp"
%}

namespace FullLib {
    void f2();
    class NoParm {
    public:
        NoParm();
        void f();
    };
}

