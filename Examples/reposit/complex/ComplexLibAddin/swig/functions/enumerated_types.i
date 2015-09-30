
%group(enumerated_types);

%insert(enumerated_types_library_hpp) %{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    enum AccountType;
    long getInterestRate(AccountType accountType);
    long getInterestRate2(Account2::Type2 accountType2);
}

