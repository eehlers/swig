
%feature("rp:group", "adder2");
%feature("rp:include") %{
#include <fl/adder2.hpp>
%}

namespace FullLib {
    class Adder2 {
    public:
        Adder2(FullLib::Long x);
        long add(FullLib::Long y);
    };
}

