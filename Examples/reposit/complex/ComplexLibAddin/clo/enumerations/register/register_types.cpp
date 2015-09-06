
#include <rp/enumerations/typefactory.hpp>
#include <clo/enumerations/register/register_types.hpp>
#include <cl/enumerated_types.hpp>

namespace ComplexLibAddin {

    void registerEnumeratedTypes() {

        {
            reposit::Create<ComplexLib::AccountType> create;
            create.registerType("Current", new ComplexLib::AccountType(ComplexLib::Current));
            create.registerType("Savings", new ComplexLib::AccountType(ComplexLib::Savings));
        }

        {
            reposit::Create<ComplexLib::Account2::Type2> create;
            create.registerType("Current2", new ComplexLib::Account2::Type2(ComplexLib::Account2::Current2));
            create.registerType("Savings2", new ComplexLib::Account2::Type2(ComplexLib::Account2::Savings2));
        }
    }

    void unregisterEnumeratedTypes() {

        reposit::Create<ComplexLib::AccountType>().unregisterTypes();
    }
}

