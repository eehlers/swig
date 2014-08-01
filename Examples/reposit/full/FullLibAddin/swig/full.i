
%module(
    rp_obj_dir="../flo",
    rp_add_dir="../AddinCpp",
    rp_xll_dir="../../FullLibXL/flxl",
    rp_obj_inc="flo",
    rp_add_inc="AddinCpp",
    rp_xll_inc="flxl"
) FullLibAddin

%feature("rp:generation", "automatic");

%include typemaps.i
%include functions.i

