
%group(coercions);

%insert(coercions_library_hpp) %{
#include <cl/coercions.hpp>
%}

%insert(coercions_addin_cpp) %{
#include <cl/coercions.hpp>
%}

namespace ComplexLib {
    struct Grade2;
    std::string showGrade2(Grade2 score);
}

