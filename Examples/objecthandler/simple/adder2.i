
%feature("oh:group", "adder2");
%feature("oh:include") %{
#include "Library/adder2.hpp"
%}

namespace SimpleLib {
    class Adder2 {
    public:
        Adder2(SimpleLib::Long x);
        long add(SimpleLib::Long y);
    };
}

