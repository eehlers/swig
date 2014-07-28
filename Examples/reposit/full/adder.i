
%feature("rp:group", "adder");
%feature("rp:include") %{
#include "Library/adder.hpp"
%}

namespace FullLib {
    std::string func();
    class Adder {
    public:
        Adder(long x);
        long add(long y);
    };
}
