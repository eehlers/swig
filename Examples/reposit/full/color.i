
%feature("rp:group", "color");
%feature("rp:include") %{
#include "Library/color.hpp"
%}

namespace SimpleLib {
    std::string getColorName(boost::shared_ptr<SimpleLib::Color> color);
    std::string accountTypeToString(SimpleLib::Account::Type type);
}
