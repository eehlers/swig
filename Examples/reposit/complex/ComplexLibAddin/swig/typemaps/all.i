
%apply rp_tp_cnv { ComplexLib::Grade };

%apply rp_tp_crc { ComplexLib::Grade2 };

ENUMERATED_TYPE(ComplexLib::AccountType)
ENUMERATED_TYPE(ComplexLib::Account2::Type2)
ENUMERATED_CLASS(boost::shared_ptr<ComplexLib::TimeZone>)

// For this class, instead of extracting the underlying library object,
// we want to extract the addin wrapper object.
OBJECT_WRAPPER(ComplexLibAddin::Test, ComplexLib::Test)
OBJECT_WRAPPER(ComplexLibAddin::Test2, ComplexLib::Test2)
OBJECT_WRAPPER(ComplexLibAddin::Base, ComplexLib::Base)
OBJECT_WRAPPER(ComplexLibAddin::Derived, ComplexLib::Derived)
OBJECT_WRAPPER(ComplexLibAddin::A, ComplexLib::A)
OBJECT_WRAPPER(ComplexLibAddin::B, ComplexLib::B)
