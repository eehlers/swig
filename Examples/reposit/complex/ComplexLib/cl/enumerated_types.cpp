
#include <cl/enumerated_types.hpp>
#include <sstream>

long ComplexLib::getInterestRate(AccountType accountType) {
    if (Current==accountType)
        return 0;
    else if (Savings==accountType)
        return 3;
    else
        //OH_FAIL("Unrecognized value for AccountType enum : '" << accountType << "'");
        return -1;
}

