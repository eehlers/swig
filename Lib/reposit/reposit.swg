
// rp_val_* - valueobjects

%typemap(rp_tm_val_prm) SWIGTYPE "$1_type";

%typemap(rp_tm_val_dcl) SWIGTYPE "$1_type $1_name_";

%typemap(rp_tm_val_ser) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

%typemap(rp_tm_val_nam) SWIGTYPE "\"$1_name\"";

//%typemap(rp_val_ctor_decl) SWIGTYPE %{                $1_type $1_name,
//%}

%typemap(rp_tm_val_ini) SWIGTYPE "$1_name_($1_name)";

%typemap(rp_tm_val_cnv) SWIGTYPE "ObjectHandler::convert2<$1_type>(value)";

// rp_ser_* - serialization

%typemap(rp_tm_cre_cnv) SWIGTYPE %{
    $1_type $1_name =
        ObjectHandler::convert2<$1_type>(
            valueObject->getProperty("$1_name"));
%}

//%typemap(rp_tm_cre_cnv) XXX %{
//    ObjectHandler::property_t $1_name =
//        valueObject->getProperty("$1_name");
//%}

//%typemap(rp_tm_cre_cnv) YYY %{
//    NameSpace::Type $1_name =
//        ObjectHandler::convert2<$1_type>(
//            valueObject->getProperty("$1_name"), "$1_name");
//%}

// rp_obj_* - addin objects

%typemap(rp_tm_obj_ret) SWIGTYPE "$1_type";
%typemap(rp_tm_obj_rdc) SWIGTYPE "return ";
%typemap(rp_tm_obj_rdc) void "";

// rp_add_* - C++ addin

%typemap(rp_tm_add_ret) SWIGTYPE "$1_type";

%typemap(rp_tm_add_prm) rp_tp_double "double";
%typemap(rp_tm_add_prm) rp_tp_double const & "const double&";
%typemap(rp_tm_add_prm) rp_tp_cnv "const ObjectHandler::property_t&";
%typemap(rp_tm_add_prm) rp_tp_crc "const ObjectHandler::property_t&";
%typemap(rp_tm_add_prm) SWIGTYPE "$1_type";

%typemap(rp_tm_add_cnv) rp_tp_cnv %{
    $1_type $1_name_cnv =
        ObjectHandler::convert2<$1_type, ObjectHandler::property_t>($1_name);
%}

%typemap(rp_tm_add_cnv) rp_tp_crc %{
    $1_type $1_name_crc = ObjectHandler::CoerceImpl<
        const ObjectHandler::property_t&,
        $1_type>()($1_name);
%}

%typemap(rp_tm_add_cll) SWIGTYPE "$1_name";
%typemap(rp_tm_add_cll) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_add_cll) rp_tp_crc "$1_name_crc";

%typemap(rp_add_ret) SWIGTYPE "return ";
%typemap(rp_add_ret) void "";

// rp_xll_* - Excel addin

%typemap(rp_tm_xll_cod) bool "L";
%typemap(rp_tm_xll_cod) long "N";
%typemap(rp_tm_xll_cod) rp_tp_double "E";
%typemap(rp_tm_xll_cod) char* "C";
%typemap(rp_tm_xll_cod) std::string "C";
%typemap(rp_tm_xll_cod) std::string& "C";
%typemap(rp_tm_xll_cod) std::string const & "C";
%typemap(rp_tm_xll_cod) void "L";
%typemap(rp_tm_xll_cod) SWIGTYPE * "C";

%typemap(rp_tm_xll_prm) bool "bool*";
%typemap(rp_tm_xll_prm) long "long*";
%typemap(rp_tm_xll_prm) rp_tp_double "double*";
%typemap(rp_tm_xll_prm) char* "char*";
%typemap(rp_tm_xll_prm) std::string "char*";
%typemap(rp_tm_xll_prm) std::string& "char*";
%typemap(rp_tm_xll_prm) std::string const & "char*";

%typemap(rp_tm_xll_cll_obj) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_obj) long "*$1_name";
%typemap(rp_tm_xll_cll_obj) rp_tp_double "*$1_name";

%typemap(rp_tm_xll_cll_val) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_val) long "*$1_name";
%typemap(rp_tm_xll_cll_val) rp_tp_double "*$1_name";

%typemap(rp_tm_xll_ret) bool "bool*";
%typemap(rp_tm_xll_ret) long "long*";
%typemap(rp_tm_xll_ret) rp_tp_double "double*";
%typemap(rp_tm_xll_ret) char* "char*";
%typemap(rp_tm_xll_ret) std::string "char*";
%typemap(rp_tm_xll_ret) void "bool*";

%typemap(rp_xll_get) SWIGTYPE "$1_type returnValue =";
%typemap(rp_xll_get) void "";

%typemap(rp_tm_xll_rdc) SWIGTYPE %{
        static $1_type ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rdc) void %{
        static bool ret = true;
        return &ret;
%}

%typemap(rp_tm_xll_rdc) std::string %{
        static char ret[XL_MAX_STR_LEN];
        ObjectHandler::stringToChar(returnValue, ret);
        return ret;
%}

%apply rp_tp_double { double };
//%apply rp_tp_double const & { double const & };

