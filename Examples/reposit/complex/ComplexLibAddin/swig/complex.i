
%typemap(rp_tm_xll_cod) ComplexLib::Adder * "C";
%typemap(rp_tm_xll_cod) ComplexLib::Adder2 * "C";
%typemap(rp_tm_xll_cod) ComplexLib::Foo * "C";
%typemap(rp_tm_xll_cod) ComplexLib::Base * "C";
%typemap(rp_tm_xll_cod) ComplexLib::Derived * "C";
%typemap(rp_tm_xll_cod) ComplexLib::NoParm * "C";

%module(
    rp_obj_dir="../clo",
    rp_add_dir="../AddinCpp",
    rp_xll_dir="../../ComplexLibXL/clxl",
    rp_obj_inc="clo",
    rp_add_inc="AddinCpp",
    rp_xll_inc="clxl"
) ComplexLibAddin

%include typemaps.i
%include functions.i

