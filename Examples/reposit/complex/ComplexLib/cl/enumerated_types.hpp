
#ifndef complex_lib_enumerated_types_hpp
#define complex_lib_enumerated_types_hpp

// Test enumerated types.

//#include <string>

namespace ComplexLib {

    enum AccountType { Current, Savings };
    long getInterestRate(AccountType accountType);

//    enum Compounding { Simple = 0,
//                       Compounded = 1,
//                       Continuous = 2
//    };
//
//    struct Position {
//        enum Type { Long, Short };
//    };
};

#endif

