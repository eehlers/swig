
%pragma(reposit) group="enumerated_classes";

%pragma(reposit) obj_include=%{
#include <cl/enumerated_classes.hpp>
%}

%pragma(reposit) add_include=%{
#include <boost/shared_ptr.hpp>
%}

namespace ComplexLib {
    std::string timeString(boost::shared_ptr<TimeZone> timeZone);
}

