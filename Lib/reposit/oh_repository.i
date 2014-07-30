
%feature("rp:group", "oh_repository");
%feature("rp:generation", "manual");
%feature("rp:include") %{
#include "oh/repository.hpp"
%}

namespace ObjectHandler {
    void deleteObject(const std::string &objectID);
}

%feature("rp:generation", "automatic");

