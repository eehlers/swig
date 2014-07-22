
%feature("rp:group", "adder2");
%feature("rp:include") %{
#include "Library/adder2.hpp"
%}

namespace SimpleLib {
    class Adder2 {
    public:
        Adder2(SimpleLib::Long x);
        long add(SimpleLib::Long y);
    };
}

