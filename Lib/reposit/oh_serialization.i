
%feature("rp:group", "oh_serialization");
%feature("rp:generation", "manual");
%feature("rp:include") %{
#include "oh/utilities.hpp"
%}

namespace ObjectHandler {
    std::string objectSaveString(const std::string &objectId);
    void loadObjectString(const std::string &xml, bool overwriteExisting);
}

%feature("rp:generation", "automatic");

