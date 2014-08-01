
// rp_val_* - valueobjects

%typemap(rp_val_in) SWIGTYPE "$1_type";

%typemap(rp_val_declare) SWIGTYPE "$1_type $1_name_";

%typemap(rp_val_call) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

%typemap(rp_val_prop) SWIGTYPE "\"$1_name\"";

//%typemap(rp_val_ctor_decl) SWIGTYPE %{                $1_type $1_name,
//%}

%typemap(rp_val_ctor_init) SWIGTYPE "$1_name_($1_name)";

%typemap(rp_val_cnv) SWIGTYPE "ObjectHandler::convert2<$1_type>(value)";

// rp_ser_* - serialization

%typemap(rp_ser_cnv) SWIGTYPE %{
    $1_type $1_name =
        ObjectHandler::convert2<$1_type>(
            valueObject->getProperty("$1_name"));
%}

//%typemap(rp_ser_cnv) XXX %{
//    ObjectHandler::property_t $1_name =
//        valueObject->getProperty("$1_name");
//%}

//%typemap(rp_ser_cnv) YYY %{
//    NameSpace::Type $1_name =
//        ObjectHandler::convert2<$1_type>(
//            valueObject->getProperty("$1_name"), "$1_name");
//%}

// rp_obj_* - addin objects

%typemap(rp_obj_typ) SWIGTYPE "$1_type";
%typemap(rp_obj_ret) SWIGTYPE "return ";
%typemap(rp_obj_ret) void "";

// rp_add_* - C++ addin

%typemap(rp_add_typ) SWIGTYPE "$1_type";
%typemap(rp_add_in) SWIGTYPE "$1_type";
%typemap(rp_add_call) SWIGTYPE "$1_name";

%typemap(rp_add_ret) SWIGTYPE "return ";
%typemap(rp_add_ret) void "";

// rp_xll_* - Excel addin

%typemap(rp_xll) bool "L";
%typemap(rp_xll) long "N";
%typemap(rp_xll) double "E";
%typemap(rp_xll) std::string "C";
%typemap(rp_xll) std::string& "C";
%typemap(rp_xll) std::string const & "C";
%typemap(rp_xll) void "L";

%typemap(rp_xll_in) long "long*";
%typemap(rp_xll_in) double "double*";
%typemap(rp_xll_in) std::string& "char*";
%typemap(rp_xll_in) std::string const & "char*";

%typemap(rp_xll_call) SWIGTYPE "$1_name";
%typemap(rp_xll_call) long "*$1_name";
%typemap(rp_xll_call) double "*$1_name";

%typemap(rp_xll_out) bool "bool*";
%typemap(rp_xll_out) long "long*";
%typemap(rp_xll_out) double "double*";
%typemap(rp_xll_out) std::string "char*";
%typemap(rp_xll_out) void "bool*";

%typemap(rp_xll_get) SWIGTYPE "$1_type returnValue =";
%typemap(rp_xll_get) void "";

%typemap(rp_xll_ret) SWIGTYPE %{
        static $1_type ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_xll_ret) void %{
        static bool ret = true;
        return &ret;
%}

%typemap(rp_xll_ret) std::string %{
        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;
%}

