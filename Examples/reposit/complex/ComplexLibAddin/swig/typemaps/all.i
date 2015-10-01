
%apply double { LongDouble };
%apply const double & { const LongDouble & };

%apply rp_tp_cnv { ComplexLib::Grade };

%apply rp_tp_crc { ComplexLib::Grade2 };

%apply rp_tp_enm { ComplexLib::AccountType };
%apply rp_tp_enm { ComplexLib::Account2::Type2 };
%apply rp_tp_enm_cls { boost::shared_ptr<ComplexLib::TimeZone> };

// For this class, instead of extracting the underlying library object,
// we want to extract the addin wrapper object.
%apply rp_tp_add_obj { ComplexLib::Test2 };
