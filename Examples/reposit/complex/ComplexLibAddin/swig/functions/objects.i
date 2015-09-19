
%group(objects);

%insert(objects_library_hpp) %{
#include <cl/objects.hpp>
%}

namespace ComplexLib {
    class Test {
    public:
        Test(long input);
        long getInput() const;
    };
}

