
// features

// This does not work.
//#define %overrideobj      %feature("rp:override_obj")
//#define %clearoverrideobj %feature("rp:override_obj", "")
//#define %overrideadd      %feature("rp:override_add")
//#define %clearoverrideadd %feature("rp:override_add", "")

// rp_tm_xxx_* - typemaps shared by multiple addins

// rp_tm_add_oh_get:  This typemap is used for class member functions in the addin.
// We retrieve the object from the repository, and then extract the required reference.
// Default behavior: get a reference to the underlying library object.
%typemap(rp_tm_xxx_oh_get) SWIGTYPE %{
        OH_GET_REFERENCE(x, objectID, $rp_typedef_obj_add, $rp_typedef_obj_lib);
%}

// Sometimes the addin wants to override the library's behavior, in this case we get
// a reference to the addin object that wraps the library object:
%typemap(rp_tm_xxx_oh_get) rp_tp_add_obj %{
        OH_GET_OBJECT(x, objectID, $rp_typedef_obj_add);
%}

// rp_tm_val_* - typemaps for valueobjects

%typemap(rp_tm_val_prm) rp_tp_double "double";
%typemap(rp_tm_val_prm) rp_tp_string "const std::string &";
%typemap(rp_tm_val_prm) rp_tp_any "const ObjectHandler::property_t&";
%typemap(rp_tm_val_prm) rp_tp_enm "const std::string &";
%typemap(rp_tm_val_prm) rp_tp_obj1 "const std::string &";
%typemap(rp_tm_val_prm) SWIGTYPE "$1_type";

%typemap(rp_tm_val_dcl) rp_tp_double "double $1_name_";
%typemap(rp_tm_val_dcl) rp_tp_string "std::string $1_name_";
%typemap(rp_tm_val_dcl) rp_tp_any "ObjectHandler::property_t $1_name_";
%typemap(rp_tm_val_dcl) rp_tp_enm "std::string $1_name_";
%typemap(rp_tm_val_dcl) rp_tp_obj1 "std::string $1_name_";
%typemap(rp_tm_val_dcl) SWIGTYPE "$1_type $1_name_";

%typemap(rp_tm_val_ser) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

%typemap(rp_tm_val_nam) SWIGTYPE "\"$1_name\"";

%typemap(rp_tm_val_ini) SWIGTYPE "$1_name_($1_name)";

%typemap(rp_tm_val_cnv) rp_tp_double "ObjectHandler::convert2<double>(value)";
%typemap(rp_tm_val_cnv) rp_tp_string "ObjectHandler::convert2<std::string>(value)";
%typemap(rp_tm_val_cnv) rp_tp_any "value";
%typemap(rp_tm_val_cnv) rp_tp_enm "ObjectHandler::convert2<std::string>(value)";
%typemap(rp_tm_val_cnv) rp_tp_obj1 "ObjectHandler::convert2<std::string>(value)";
%typemap(rp_tm_val_cnv) SWIGTYPE "ObjectHandler::convert2<$1_type>(value)";

// rp_ser_* - typemaps for serialization

%typemap(rp_tm_cre_cnv) SWIGTYPE %{
    $1_type $1_name =
        ObjectHandler::convert2<$1_type>(
            valueObject->getProperty("$1_name"));
%}

// rp_tm_obj_* - typemaps for addin objects

%typemap(rp_tm_obj_ret) SWIGTYPE "$1_type";
%typemap(rp_tm_obj_rdc) SWIGTYPE "return ";
%typemap(rp_tm_obj_rdc) void "";

// rp_tm_add_* - typemaps for  the C++ addin

//%typemap(rp_tm_add_ret) SWIGTYPE "$1_type";

//%typemap(rp_tm_add_prm) rp_tp_double "double";
//%typemap(rp_tm_add_prm) rp_tp_cnv "const ObjectHandler::property_t&";
//%typemap(rp_tm_add_prm) rp_tp_crc "const ObjectHandler::property_t&";
//%typemap(rp_tm_add_prm) rp_tp_enm "const std::string&";
//%typemap(rp_tm_add_prm) rp_tp_enm_cls "const std::string&";
//%typemap(rp_tm_add_prm) SWIGTYPE "$rp_typedef_resolved";

//%typemap(rp_tm_add_cnv) rp_tp_cnv %{
//    $1_type $1_name_cnv =
//        ObjectHandler::convert2<$1_type, ObjectHandler::property_t>($1_name);
//%}

//%typemap(rp_tm_add_cnv) rp_tp_crc %{
//    $1_type $1_name_crc = ObjectHandler::CoerceImpl<
//        ObjectHandler::property_t,
//        $1_type>()($1_name);
//%}

//%typemap(rp_tm_add_cnv) rp_tp_enm %{
//    $1_type $1_name_enm =
//        ObjectHandler::Create<$1_type>()($1_name);
//%}

//%typemap(rp_tm_add_cnv) rp_tp_enm_cls %{
//    $1_type $1_name_enm_cls =
//        ObjectHandler::Create<$1_type >()($1_name);
//%}

//%typemap(rp_tm_add_cll) SWIGTYPE "$1_name";
//%typemap(rp_tm_add_cll) rp_tp_cnv "$1_name_cnv";
//%typemap(rp_tm_add_cll) rp_tp_crc "$1_name_crc";
//%typemap(rp_tm_add_cll) rp_tp_enm "$1_name_enm";
//%typemap(rp_tm_add_cll) rp_tp_enm_cls "$1_name_enm_cls";

//%typemap(rp_add_ret) SWIGTYPE "return ";

//%typemap(rp_add_ret) void "";

// rp_tm_xll_* - typemaps for the Excel addin

%typemap(rp_tm_xll_cod) bool "L";
%typemap(rp_tm_xll_cod) long "N";
%typemap(rp_tm_xll_cod) rp_tp_double "E";
%typemap(rp_tm_xll_cod) rp_tp_string "C";
%typemap(rp_tm_xll_cod) rp_tp_string * "C";
%typemap(rp_tm_xll_cod) rp_tp_string & "C";
%typemap(rp_tm_xll_cod) rp_tp_string const & "C";
%typemap(rp_tm_xll_cod) void "L";
%typemap(rp_tm_xll_cod) rp_tp_cnv "P";
%typemap(rp_tm_xll_cod) rp_tp_crc "P";
%typemap(rp_tm_xll_cod) rp_tp_enm "C";
%typemap(rp_tm_xll_cod) rp_tp_enm_cls "C";
%typemap(rp_tm_xll_cod) rp_tp_any "P";
%typemap(rp_tm_xll_cod) std::vector "P";
%typemap(rp_tm_xll_cod) std::vector const & "P";
%typemap(rp_tm_xll_cod) rp_tp_obj1 "C";
%typemap(rp_tm_xll_cod) SWIGTYPE * "C";

%typemap(rp_tm_xll_prm) bool "bool*";
%typemap(rp_tm_xll_prm) long "long*";
%typemap(rp_tm_xll_prm) rp_tp_double "double*";
%typemap(rp_tm_xll_prm) rp_tp_string "char*";
%typemap(rp_tm_xll_prm) rp_tp_string * "char*";
%typemap(rp_tm_xll_prm) rp_tp_string & "char*";
%typemap(rp_tm_xll_prm) rp_tp_string & "const char*";
%typemap(rp_tm_xll_prm) rp_tp_cnv "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_crc "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_any "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_enm "char*";
%typemap(rp_tm_xll_prm) rp_tp_enm_cls "char*";
%typemap(rp_tm_xll_prm) std::vector "OPER *";
%typemap(rp_tm_xll_prm) std::vector const & "OPER *";
%typemap(rp_tm_xll_prm) rp_tp_obj1 "char*";

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
    $rp_typedef_base $1_name_enm =
        ObjectHandler::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_xll_cnv) rp_tp_enm_cls %{
    $1_type $1_name_enm_cls =
        ObjectHandler::Create<$1_type >()($1_name);
%}

// FIXME this typemap should be split into two
%typemap(rp_tm_xll_cnv) rp_tp_cnv2 %{
        $rp_typedef_base $1_name_cnv = ObjectHandler::convert2<$rp_typedef_base>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", $rp_typedef_base());
            
        ObjectHandler::property_t $1_name_cnv2 = ObjectHandler::convert2<ObjectHandler::property_t>(
            ObjectHandler::ConvertOper(*$1_name));            
%} 
%typemap(rp_tm_xll_cnv) rp_tp_obj1 %{
        OH_GET_REFERENCE($1_name_obj, $1_name,
            $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);        
%} 

%typemap(rp_tm_xll_cnv) std::vector %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            ObjectHandler::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnv) const std::vector & %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            ObjectHandler::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cll_obj) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_obj) bool "*$1_name";
%typemap(rp_tm_xll_cll_obj) long "*$1_name";
%typemap(rp_tm_xll_cll_obj) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_cll_obj) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_xll_cll_obj) rp_tp_cnv2 "$1_name_cnv";
%typemap(rp_tm_xll_cll_obj) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_xll_cll_obj) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xll_cll_obj) rp_tp_enm_cls "$1_name_enm_cls";
%typemap(rp_tm_xll_cll_obj) const std::vector & "$1_name_vec";
%typemap(rp_tm_xll_cll_obj) std::vector "$1_name_vec";
%typemap(rp_tm_xll_cll_obj) rp_tp_obj1 "$1_name_obj";

%typemap(rp_xll_call) rp_tp_cnv "$1_name_cnv";

%typemap(rp_tm_xll_cll_val) SWIGTYPE "$1_name";
%typemap(rp_tm_xll_cll_val) long "*$1_name";
%typemap(rp_tm_xll_cll_val) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_cll_val) rp_tp_cnv2 "$1_name_cnv2";

%typemap(rp_tm_xll_ret) bool "bool*";
%typemap(rp_tm_xll_ret) long "long*";
%typemap(rp_tm_xll_ret) rp_tp_double "double*";
%typemap(rp_tm_xll_ret) rp_tp_string "char*";
%typemap(rp_tm_xll_ret) void "bool*";
%typemap(rp_tm_xll_ret) std::vector< std::string > "OPER*";

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

%typemap(rp_tm_xll_rdc) std::vector< std::string > %{
        static OPER xRet;
        ObjectHandler::vectorToOper(returnValue, xRet);
        return &xRet;
%}

// rp_tm_cfy_* - typemaps for the Countify addin

//%typemap(rp_tm_cfy_ret) std::string "const char *";
//%typemap(rp_tm_cfy_ret) void "bool";
//%typemap(rp_tm_cfy_ret) SWIGTYPE "$1_type";

//%typemap(rp_tm_cfy_rt2) SWIGTYPE "return 0;";

//%typemap(rp_tm_cfy_prm) rp_tp_double "double $1_name";
//%typemap(rp_tm_cfy_prm) rp_tp_double const & "const double& $1_name";
//%typemap(rp_tm_cfy_prm) const std::string & "const char * $1_name";
//%typemap(rp_tm_cfy_prm) rp_tp_cnv "const ObjectHandler::property_t& $1_name";
//%typemap(rp_tm_cfy_prm) rp_tp_crc "const ObjectHandler::property_t& $1_name";
//%typemap(rp_tm_cfy_prm) rp_tp_enm "const std::string& $1_name";
//%typemap(rp_tm_cfy_prm) rp_tp_enm_cls "const char * $1_name";
//%typemap(rp_tm_cfy_prm) SWIGTYPE "$rp_typedef_resolved $1_name";

//%typemap(rp_tm_cfy_cl1) std::string %{
//    static std::string ret;
//    ret =
//%}
//%typemap(rp_tm_cfy_cl1) bool "return ";

//%typemap(rp_tm_cfy_cl2) void "return true;";
//%typemap(rp_tm_cfy_cl2) std::string "return ret.c_str();";

//%typemap(rp_tm_cfy_cll) SWIGTYPE "$1_name";

//%typemap(rp_tm_cfy_mng) void "BOOL";
//%typemap(rp_tm_cfy_mng) rp_tp_double "REAL32";
//%typemap(rp_tm_cfy_mng) SWIGTYPE "CSTR";

// Map native types to reposit types

%apply rp_tp_double { double };

%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_string { std::string const & };

%apply rp_tp_any { const ObjectHandler::property_t& };
