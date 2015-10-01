
%group(enumerated_classes);

%insert(enumerated_classes_library_hpp) %{
#include <cl/enumerated_classes.hpp>
%}

%insert(enumerated_classes_addin_cpp) %{
#include <cl/enumerated_classes.hpp>
%}

namespace ComplexLib {
    class TimeZone;
    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
}

