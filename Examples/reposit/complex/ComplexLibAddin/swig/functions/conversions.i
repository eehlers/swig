
%group(conversions);

%insert(conversions_library_hpp) %{
#include <cl/conversions.hpp>
%}

%insert(conversions_addin_cpp) %{
#include <cl/conversions.hpp>
%}

namespace ComplexLib {
    struct Grade;
    std::string showGrade(Grade score);
}

