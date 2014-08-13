
%feature("rp:group", "enumerated_types");
%feature("rp:include") %{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    long getInterestRate(AccountType accountType);
    long getInterestRate2(Account2::Type2 accountType2);
}

%feature("rp:group", "");

