
%group(functions);

%insert(functions_library_hpp) %{
#include <cl/functions.hpp>
%}

%insert(functions_addin_cpp) %{
#include <cl/functions.hpp>
%}

namespace ComplexLib {
    std::string func1();
}

