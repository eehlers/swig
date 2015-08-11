
// features

// This does not work.
//#define %overrideobj      %feature("rp:override_obj")
//#define %clearoverrideobj %feature("rp:override_obj", "")
//#define %overrideadd      %feature("rp:override_add")
//#define %clearoverrideadd %feature("rp:override_add", "")

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
%typemap(rp_tm_add_prm) rp_tp_enm "const std::string&";
%typemap(rp_tm_add_prm) rp_tp_enm_cls "const std::string&";
//%typemap(rp_tm_add_prm) SWIGTYPE "$1_type";
%typemap(rp_tm_add_prm) SWIGTYPE "$rp_typedef_resolved";

%typemap(rp_tm_add_cnv) rp_tp_cnv %{
    $1_type $1_name_cnv =
        ObjectHandler::convert2<$1_type, ObjectHandler::property_t>($1_name);
%}

%typemap(rp_tm_add_cnv) rp_tp_crc %{
    $1_type $1_name_crc = ObjectHandler::CoerceImpl<
        ObjectHandler::property_t,
        $1_type>()($1_name);
%}

%typemap(rp_tm_add_cnv) rp_tp_enm %{
    $1_type $1_name_enm =
        ObjectHandler::Create<$1_type>()($1_name);
%}

%typemap(rp_tm_add_cnv) rp_tp_enm_cls %{
    $1_type $1_name_enm_cls =
        ObjectHandler::Create<$1_type >()($1_name);
%}

%typemap(rp_tm_add_cll) SWIGTYPE "$1_name";
%typemap(rp_tm_add_cll) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_add_cll) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_add_cll) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_add_cll) rp_tp_enm_cls "$1_name_enm_cls";

%typemap(rp_add_ret) SWIGTYPE "return ";

%typemap(rp_add_ret) void "";

// rp_tm_add_oh_get:  This typemap is used for class member functions in the addin.
// We retrieve the object from the repository, and then extract the required reference.
// Default behavior: get a reference to the underlying library object.
%typemap(rp_tm_add_oh_get) SWIGTYPE %{
    OH_GET_REFERENCE(x, objectID, $rp_typedef_obj_add, $rp_typedef_obj_lib);
%}

// Sometimes the addin wants to override the library's behavior, in this case we get
// a reference to the addin object that wraps the library object:
%typemap(rp_tm_add_oh_get) rp_tp_add_obj %{
    OH_GET_OBJECT(x, objectID, $rp_typedef_obj_add);
%}

// rp_xll_* - Excel addin

%typemap(rp_tm_xll_cod) bool "L";
%typemap(rp_tm_xll_cod) long "N";
%typemap(rp_tm_xll_cod) rp_tp_double "E";
//%typemap(rp_tm_xll_cod) double "E";
%typemap(rp_tm_xll_cod) char* "C";
%typemap(rp_tm_xll_cod) std::string "C";
%typemap(rp_tm_xll_cod) std::string& "C";
%typemap(rp_tm_xll_cod) std::string const & "C";
%typemap(rp_tm_xll_cod) void "L";
%typemap(rp_tm_xll_cod) SWIGTYPE * "C";
//NEW
%typemap(rp_tm_xll_cod) rp_tp_double const & "E";
%typemap(rp_tm_xll_cod) rp_tp_cnv "P";
%typemap(rp_tm_xll_cod) rp_tp_crc "P";
%typemap(rp_tm_xll_cod) rp_tp_enm "C";
%typemap(rp_tm_xll_cod) rp_tp_enm_cls "C";

%typemap(rp_tm_xll_prm) bool "bool*";
%typemap(rp_tm_xll_prm) long "long*";
%typemap(rp_tm_xll_prm) rp_tp_double "double*";
//%typemap(rp_tm_xll_prm) double "double*";
%typemap(rp_tm_xll_prm) char* "char*";
%typemap(rp_tm_xll_prm) std::string "char*";
%typemap(rp_tm_xll_prm) std::string& "char*";
%typemap(rp_tm_xll_prm) std::string const & "char*";
//NEW
%typemap(rp_tm_xll_prm) rp_tp_double const & "double*";
%typemap(rp_tm_xll_prm) rp_tp_cnv "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_crc "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_enm "char*";
%typemap(rp_tm_xll_prm) rp_tp_enm_cls "char*";

//NEW
%typemap(rp_tm_xll_cnv) rp_tp_cnv %{
        $1_type $1_name_cnv = ObjectHandler::convert2<$1_type, ObjectHandler::ConvertOper>(
            ObjectHandler::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnv) rp_tp_crc %{
        $1_type $1_name_crc = ObjectHandler::CoerceImpl<
            ObjectHandler::ConvertOper, $1_type>()
            (ObjectHandler::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnv) rp_tp_enm %{
    $1_type $1_name_enm =
        ObjectHandler::Create<$1_type>()($1_name);
%}

%typemap(rp_tm_xll_cnv) rp_tp_enm_cls %{
    $1_type $1_name_enm_cls =
        ObjectHandler::Create<$1_type >()($1_name);
%}

%typemap(rp_tm_xll_cll_obj) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_obj) long "*$1_name";
%typemap(rp_tm_xll_cll_obj) rp_tp_double "*$1_name";
//%typemap(rp_tm_xll_cll_obj) double "*$1_name";
//NEW
%typemap(rp_tm_xll_cll_obj) rp_tp_double const & "*$1_name";
%typemap(rp_tm_xll_cll_obj) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_xll_cll_obj) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_xll_cll_obj) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xll_cll_obj) rp_tp_enm_cls "$1_name_enm_cls";

%typemap(rp_tm_xll_cll_val) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_val) long "*$1_name";
%typemap(rp_tm_xll_cll_val) rp_tp_double "*$1_name";
//%typemap(rp_tm_xll_cll_val) double "*$1_name";

%typemap(rp_tm_xll_ret) bool "bool*";
%typemap(rp_tm_xll_ret) long "long*";
%typemap(rp_tm_xll_ret) rp_tp_double "double*";
//%typemap(rp_tm_xll_ret) double "double*";
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

// rp_tm_cfy_* - Countify addin

%typemap(rp_tm_cfy_ret) std::string "const char *";
%typemap(rp_tm_cfy_ret) void "bool";
%typemap(rp_tm_cfy_ret) SWIGTYPE "$1_type";

%typemap(rp_tm_cfy_rt2) SWIGTYPE "return 0;";

%typemap(rp_tm_cfy_prm) rp_tp_double "double $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_double const & "const double& $1_name";
%typemap(rp_tm_cfy_prm) const std::string & "const char * $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_cnv "const ObjectHandler::property_t& $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_crc "const ObjectHandler::property_t& $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_enm "const std::string& $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_enm_cls "const char * $1_name";
//%typemap(rp_tm_cfy_prm) SWIGTYPE "$1_type $1_name";
%typemap(rp_tm_cfy_prm) SWIGTYPE "$rp_typedef_resolved $1_name";

%typemap(rp_tm_cfy_cl1) std::string %{
    static std::string ret;
    ret =
%}
%typemap(rp_tm_cfy_cl1) bool "return ";

%typemap(rp_tm_cfy_cl2) void "return true;";
%typemap(rp_tm_cfy_cl2) std::string "return ret.c_str();";

%typemap(rp_tm_cfy_cll) SWIGTYPE "$1_name";

//%typemap(rp_tm_cfy_mng) std::string "\"dataType\": \"CSTR\"";
//%typemap(rp_tm_cfy_mng) SWIGTYPE "\"dataType\": \"CSTR\"";
//%typemap(rp_tm_cfy_mng) std::string "CSTR";
%typemap(rp_tm_cfy_mng) void "BOOL";
%typemap(rp_tm_cfy_mng) rp_tp_double "REAL32";
%typemap(rp_tm_cfy_mng) SWIGTYPE "CSTR";

%apply rp_tp_double { double };
%apply rp_tp_double const & { double const & };

