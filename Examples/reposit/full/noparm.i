
%feature("rp:group", "noparm");
%feature("rp:include") %{
#include "Library/noparm.hpp"
%}

namespace FullLib {
    long f2();
    class NoParm {
    public:
        NoParm();
        long f();
    };
}

