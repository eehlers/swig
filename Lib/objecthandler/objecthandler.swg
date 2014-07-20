
%feature("oh:x", "abc") def;
%feature("featurename", "1");

%typemap(excel) std::string& "C";
%typemap(excel) std::string "C";
%typemap(excel) long "N";
%typemap(excel) double "E";
%typemap(excel) bool "L";

%typemap(excel_in) long "long*";
%typemap(excel_in) std::string& "char*";

%typemap(excel_out) std::string "char*";
%typemap(excel_out) long "long*";

%typemap(ohxl_ret) bool %{
        static bool ret = true;
        ret = returnValue;
        return &ret;
%}

%typemap(ohxl_ret) double %{
        static double ret = 0;
        ret = returnValue;
        return &ret;
%}

%typemap(ohxl_ret) std::string %{
        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;
%}

