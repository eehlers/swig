
%feature("rp:group", "objects");
%feature("rp:include") %{
#include <cl/objects.hpp>
%}

namespace ComplexLib {
    class Test {
    public:
        Test(long input);
        long getInput() const;
    };
}

%feature("rp:group", "");

