
%pragma(reposit) group="objects";

%pragma(reposit) obj_include=%{
#include <cl/objects.hpp>
%}

namespace ComplexLib {
    class Test {
    public:
        Test(long input);
        long getInput() const;
    };
}

