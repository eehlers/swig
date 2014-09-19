
%typemap(rp_tm_xll_cod) SimpleLib::Adder * "C";

%module(
    rp_obj_dir="../AddinObjects",
    rp_add_dir="../AddinCpp",
    rp_xll_dir="../AddinXl",
    rp_obj_inc="AddinObjects",
    rp_add_inc="AddinCpp",
    rp_xll_inc="AddinXl"
) SimpleLibAddin

%feature("rp:group", "adder");
%feature("rp:obj_include") %{
#include "Library/adder.hpp"
%}

namespace SimpleLib {
    std::string func();
    class Adder {
    public:
        Adder(long x);
        long add(long y);
    };
}

