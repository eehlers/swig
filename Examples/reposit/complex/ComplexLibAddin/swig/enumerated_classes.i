
%feature("rp:group", "enumerated_classes");
%feature("rp:include") %{
#include <cl/enumerated_classes.hpp>
%}

namespace ComplexLib {
    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
}

%feature("rp:group", "");

