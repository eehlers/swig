
#ifndef complex_lib_enumerated_types_hpp
#define complex_lib_enumerated_types_hpp

// Test enumerated types.

//#include <string>

namespace ComplexLib {

    enum AccountType { Current, Savings };
    long getInterestRate(AccountType accountType);

    struct Account2 {
        enum Type2 { Current2=0, Savings2=1 };
    };
    long getInterestRate2(Account2::Type2 accountType2);
};

#endif

