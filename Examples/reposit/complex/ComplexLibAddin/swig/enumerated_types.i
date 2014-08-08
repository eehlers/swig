
%feature("rp:group", "enumerated_types");
%feature("rp:include") %{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    long getInterestRate(ComplexLib::AccountType accountType);
    long getInterestRate2(ComplexLib::Account2::Type2 accountType2);
}

