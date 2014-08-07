
%module(
    rp_obj_dir="../clo",
    rp_add_dir="../AddinCpp",
    rp_xll_dir="../../ComplexLibXL/clxl",
    rp_obj_inc="clo",
    rp_add_inc="AddinCpp",
    rp_xll_inc="clxl"
) ComplexLibAddin

%apply rp_tp_double { ComplexLib::Double };
%apply const rp_tp_double & { const ComplexLib::Double & };

%apply rp_tp_cnv { ComplexLib::Grade };

%typemap(rp_tm_add_cnv) rp_tp_crc %{
    $1_type $1_name_crc =
        ComplexLibAddin::CoerceGrade2()(
            $1_name);
%}
%apply rp_tp_crc { ComplexLib::Grade2 };

%include typemaps.i
%include all_functions.i

