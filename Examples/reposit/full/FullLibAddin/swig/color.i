
%feature("rp:group", "color");
%feature("rp:include") %{
#include "Library/color.hpp"
%}

namespace FullLib {
    std::string getColorName(boost::shared_ptr<FullLib::Color> color);
    std::string accountTypeToString(FullLib::Account::Type type);
}
