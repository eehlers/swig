
%group(enumerated_classes);

%insert(enumerated_classes_lib_inc) %{
#include <cl/enumerated_classes.hpp>
%}

%insert(enumerated_classes_cpp_inc) %{
#include <boost/shared_ptr.hpp>
%}

namespace ComplexLib {
    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
}

