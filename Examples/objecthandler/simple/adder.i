
%feature("oh:group", "adder");
%feature("oh:include") %{
#include "Library/adder.hpp"
%}

namespace SimpleLib {
    std::string func();
    class Adder {
    public:
        Adder(long x);
        long add(long y);
    };
}
