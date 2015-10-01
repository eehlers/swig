
%group(enumerated_types);

%insert(enumerated_types_library_hpp) %{
#include <cl/enumerated_types.hpp>
%}

%insert(enumerated_types_addin_cpp) %{
#include <cl/enumerated_types.hpp>
%}

namespace ComplexLib {
    enum AccountType;
    struct Account2 {
        enum Type2;
    };    
    long getInterestRate(AccountType accountType);
    long getInterestRate2(Account2::Type2 accountType2);
}

