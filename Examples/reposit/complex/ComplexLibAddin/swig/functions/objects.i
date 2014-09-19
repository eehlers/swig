
%feature("rp:group", "objects");
%feature("rp:obj_include") %{
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

