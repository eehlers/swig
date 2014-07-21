
%feature("oh:group", "adder2");
%feature("oh:include") %{
#include "Library/adder2.hpp"
%}

namespace SimpleLib {
    class Long {
    public:
        Long(long x);
        Long(std::string x);
        long x();
    };
    class Adder2 {
    public:
        Adder2(Long x);
        long add(Long y);
    };
}

