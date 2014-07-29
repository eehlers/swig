
%feature("rp:group", "objecthandler");
%feature("rp:include") %{
#include "oh/utilities.hpp"
%}

namespace ObjectHandler {
    std::string version();
}

