
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

%include typemaps.i
%include all_functions.i

