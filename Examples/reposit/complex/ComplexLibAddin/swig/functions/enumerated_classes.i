
%group(enumerated_classes);

%insert(enumerated_classes_library_hpp) %{
#include <cl/enumerated_classes.hpp>
%}

%insert(enumerated_classes_cppaddin_cpp) %{
#include <boost/shared_ptr.hpp>
%}

namespace ComplexLib {
    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
}

