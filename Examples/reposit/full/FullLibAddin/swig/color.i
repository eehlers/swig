
%feature("rp:group", "color");
%feature("rp:include") %{
#include "FullLib/color.hpp"
%}

namespace FullLib {
    std::string getColorName(boost::shared_ptr<FullLib::Color> color);
    std::string accountTypeToString(FullLib::Account::Type type);
}
