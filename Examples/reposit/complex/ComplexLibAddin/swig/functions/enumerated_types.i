
%pragma(reposit) group="enumerated_types";

%pragma(reposit) obj_include=%{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    long getInterestRate(AccountType accountType);
    long getInterestRate2(Account2::Type2 accountType2);
}

