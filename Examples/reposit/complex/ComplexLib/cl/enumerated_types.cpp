
#include <cl/enumerated_types.hpp>
#include <sstream>

long ComplexLib::getInterestRate(AccountType accountType) {
    if (Current==accountType)
        return 0;
    else if (Savings==accountType)
        return 3;
    else
        //CL_FAIL("Unrecognized value for AccountType enum : '" << accountType << "'");
        return -1;
}

long ComplexLib::getInterestRate2(Account2::Type2 accountType) {
    if (Account2::Current2==accountType)
        return 0;
    else if (Account2::Savings2==accountType)
        return 3;
    else
        //CL_FAIL("Unrecognized value for AccountType enum : '" << accountType << "'");
        return -1;
}

