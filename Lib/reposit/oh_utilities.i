
%feature("rp:group", "oh_utilities");
%feature("rp:include") %{
#include "oh/utilities.hpp"
%}

namespace ObjectHandler {
    std::string version();
}

