
%group(enumerated_types);

%insert(enumerated_types_lib_inc) %{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    long getInterestRate(AccountType accountType);
    long getInterestRate2(Account2::Type2 accountType2);
}

