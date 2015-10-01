
%module(
    rp_obj_dir="../clo",
    rp_add_dir="../AddinCpp",
    rp_csh_dir="../AddinCSharp",
    rp_cfy_dir="../AddinCfy",
    rp_xll_dir="../../ComplexLibXL/clxl",
    rp_obj_inc="clo",
    rp_add_inc="AddinCpp",
    rp_xll_inc="clxl",
    rp_cfy_inc="AddinCfy"
) ComplexLibAddin

%feature("rp:generate:c++");

%include typemaps/all.i
%include functions/all.i
