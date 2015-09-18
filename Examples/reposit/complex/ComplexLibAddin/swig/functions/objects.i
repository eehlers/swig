
%group(objects);

%insert(objects_lib_inc) %{
#include <cl/objects.hpp>
%}

namespace ComplexLib {
    class Test {
    public:
        Test(long input);
        long getInput() const;
    };
}

