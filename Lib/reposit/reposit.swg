
%feature("oh:x", "abc") def;
%feature("featurename", "1");

%typemap(rp_cpp_call) SWIGTYPE "$1_name";

%typemap(rp_excel) std::string& "C";
%typemap(rp_excel) std::string "C";
%typemap(rp_excel) long "N";
%typemap(rp_excel) double "E";
%typemap(rp_excel) bool "L";

%typemap(rp_excel_in) long "long*";
%typemap(rp_excel_in) double "double*";
%typemap(rp_excel_in) std::string& "char*";

%typemap(rp_excel_out) bool "bool*";
%typemap(rp_excel_out) long "long*";
%typemap(rp_excel_out) double "double*";
%typemap(rp_excel_out) std::string "char*";

%typemap(rp_ohxl_ret) bool %{
        static bool ret = true;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_ohxl_ret) long %{
        return 0;
%}

%typemap(rp_ohxl_ret) double %{
        static double ret = 0;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_ohxl_ret) std::string %{
        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;
%}

