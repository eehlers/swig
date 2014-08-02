
%feature("rp:group", "color");
%feature("rp:include") %{
#include <cl/color.hpp>
%}

namespace ComplexLib {
    std::string getColorName(boost::shared_ptr<ComplexLib::Color> color);
    std::string accountTypeToString(ComplexLib::Account::Type type);
}
