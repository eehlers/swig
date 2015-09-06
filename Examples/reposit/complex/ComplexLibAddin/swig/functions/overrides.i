
%pragma(reposit) group="overrides";
%pragma(reposit) override_obj="true";

%pragma(reposit) obj_include=%{
#include <cl/overrides.hpp>
%}

%pragma(reposit) add_include=%{
%}

namespace ComplexLib {
    class Test2 {
    public:
        Test2();
        std::string f();
    };
}

