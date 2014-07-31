
%feature("rp:group", "adder");
%feature("rp:include") %{
#include <fl/adder.hpp>
%}

namespace FullLib {
    std::string func();
    class Adder {
    public:
        Adder(long x);
        long add(long y);
    };
}
