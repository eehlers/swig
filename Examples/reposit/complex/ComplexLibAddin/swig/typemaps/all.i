
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

//%typemap(rp_tm_xll_cod) ComplexLib::Double const & "#$1";
//%typemap(rp_tm_xll_cod) LongDouble const & "#$2";
//%typemap(rp_tm_xll_cod) Grade "#$3";
//%typemap(rp_tm_xll_cod) Grade2 "#$4";
//%typemap(rp_tm_xll_cod) AccountType "#$5";
//%typemap(rp_tm_xll_cod) Account2::Type2 "#$6";
//%typemap(rp_tm_xll_cod) boost::shared_ptr< TimeZone > "#$7";

//%typemap(rp_tm_xll_prm) ComplexLib::Double const & "#$8";
//%typemap(rp_tm_xll_prm) LongDouble const & "#$9";
//%typemap(rp_tm_xll_prm) Grade "#$A";
//%typemap(rp_tm_xll_prm) Grade2 "#$B";
//%typemap(rp_tm_xll_prm) AccountType "#$C";
//%typemap(rp_tm_xll_prm) Account2::Type2 "#$D";
//%typemap(rp_tm_xll_prm) boost::shared_ptr< TimeZone > "#$E";
