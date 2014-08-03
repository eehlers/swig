
%module(
    rp_obj_dir="../AddinObjects",
    rp_add_dir="../AddinCpp",
    rp_xll_dir="../AddinXL",
    rp_obj_inc="AddinObjects",
    rp_add_inc="AddinCpp",
    rp_xll_inc="AddinXL"
) SimpleLibAddin

%feature("rp:group", "adder");
%feature("rp:include") %{
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

