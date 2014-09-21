
%apply rp_tp_double { LongDouble };
%apply const rp_tp_double & { const LongDouble & };

%apply rp_tp_cnv { Grade };

%apply rp_tp_crc { Grade2 };

%apply rp_tp_enm { AccountType };
%apply rp_tp_enm { Account2::Type2 };
%apply rp_tp_enm_cls { boost::shared_ptr<TimeZone> };

// For this class, instead of extracting the underlying library object,
// we want to extract the addin wrapper object.
%apply rp_tp_add_obj { ComplexLib::Test2 };

