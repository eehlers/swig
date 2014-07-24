
%feature("rp:group", "color");
%feature("rp:include") %{
#include "Library/color.hpp"
%}

namespace SimpleLib {
    std::string getColorName(const SimpleLib::Color &color);
    std::string accountTypeToString(SimpleLib::Account::Type type);
}
