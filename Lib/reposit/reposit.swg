
// features

// This does not work.
//#define %overrideobj      %feature("rp:override_obj")
//#define %clearoverrideobj %feature("rp:override_obj", "")
//#define %overrideadd      %feature("rp:override_add")
//#define %clearoverrideadd %feature("rp:override_add", "")

//*****************************************************************************
// rp_tm_xxx_* - typemaps shared by multiple addins
//*****************************************************************************

// rp_tm_default - not a real typemap, implemented in code.

// rp_tm_xxx_lop - arguments to boost::bind object for a looping function (F/M)
%typemap(rp_tm_xxx_lop) bool "*$1_name";
%typemap(rp_tm_xxx_lop) std::vector "_1";
%typemap(rp_tm_xxx_lop) rp_tp_cnv2 "$1_name_cnv";
%typemap(rp_tm_xxx_lop) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xxx_lop) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_lib_* - typemaps for Library Objects
//*****************************************************************************

// GROUP

// hpp files

// rp_tm_lib_ret - The return type of a function (F)
// FIXME could this be replaced by rp_tm_default?
%typemap(rp_tm_lib_ret) SWIGTYPE "$1_type";

// Not a real typemap. Defined in code.  RP_OBJ_CLASS (C)
// rp_tm_lib_cls

// cpp files

// rp_tm_lib_rts - Return statement (F)
%typemap(rp_tm_lib_rts) void "";
%typemap(rp_tm_lib_rts) SWIGTYPE "return ";

//*****************************************************************************
// rp_tm_vob_* - typemaps for Value Objects
//*****************************************************************************

// GROUP

// XPP

// rp_tm_vob_prm - Value Object constructor parameters (C)
%typemap(rp_tm_vob_prm) rp_tp_long "long";
%typemap(rp_tm_vob_prm) rp_tp_double "double";
%typemap(rp_tm_vob_prm) rp_tp_string "const std::string &";
%typemap(rp_tm_vob_prm) rp_tp_any "const reposit::property_t&";
%typemap(rp_tm_vob_prm) rp_tp_enm "const std::string &";
%typemap(rp_tm_vob_prm) rp_tp_obj1 "const std::string &";
%typemap(rp_tm_vob_prm) SWIGTYPE "$1_type";

// HPP

// rp_tm_vob_mbv - Value Object class member variables (C)
%typemap(rp_tm_vob_mbv) rp_tp_long "long $1_name_";
%typemap(rp_tm_vob_mbv) rp_tp_double "double $1_name_";
%typemap(rp_tm_vob_mbv) rp_tp_string "std::string $1_name_";
%typemap(rp_tm_vob_mbv) rp_tp_any "reposit::property_t $1_name_";
%typemap(rp_tm_vob_mbv) rp_tp_enm "std::string $1_name_";
%typemap(rp_tm_vob_mbv) rp_tp_obj1 "std::string $1_name_";
%typemap(rp_tm_vob_mbv) const std::vector<bool>& "std::vector<bool> $1_name_";
%typemap(rp_tm_vob_mbv) SWIGTYPE "$1_type $1_name_";

// rp_tm_vob_ser - code to serialize a Value Object member variable (C)
// FIXME could this be replaced with rp_tm_default?
%typemap(rp_tm_vob_ser) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

// CPP

// rp_tm_vob_nam - Insert the parameter name into the vo property names array (C)
// FIXME could this be replaced with rp_tm_default?
%typemap(rp_tm_vob_nam) SWIGTYPE "\"$1_name\"";

// rp_tm_vob_ini - vo ctor initializer list (C)
// FIXME could this be replaced with rp_tm_default?
%typemap(rp_tm_vob_ini) SWIGTYPE "$1_name_($1_name)";

// rp_tm_vob_cnv - in ValueObject::setSystemProperty, convert value from property_t to native type (C)
%typemap(rp_tm_vob_cnv) rp_tp_long "reposit::convert2<long>(value)";
%typemap(rp_tm_vob_cnv) rp_tp_double "reposit::convert2<double>(value)";
%typemap(rp_tm_vob_cnv) rp_tp_string "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnv) rp_tp_any "value";
%typemap(rp_tm_vob_cnv) rp_tp_enm "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnv) rp_tp_obj1 "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnv) const std::vector<bool>& "reposit::vector::convert2<bool>(value, nameUpper)";
%typemap(rp_tm_vob_cnv) SWIGTYPE "reposit::convert2<$1_type>(value)";

//*****************************************************************************
// rp_tm_scr_* - typemaps for Serialization - Create
//*****************************************************************************

// GROUP

// CPP

// rp_tm_scr_cnv - convert types from the Value Object to the corresponding Library Object (C)
// FIXME could this one be collapsed to use SWIGTYPE and $1_type/$1_name?
%typemap(rp_tm_scr_cnv) rp_tp_long %{
    long $1_name =
        reposit::convert2<long>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnv) rp_tp_enm %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
    $rp_typedef_base $1_name =
        reposit::Create<$rp_typedef_base>()($1_name_str);
%}

%typemap(rp_tm_scr_cnv) rp_tp_obj1 %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_REFERENCE($1_name, $1_name_str,
        $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

%typemap(rp_tm_scr_cnv) SWIGTYPE %{
    $rp_typedef_base $1_name =
        reposit::convert2<$rp_typedef_base>(
            valueObject->getProperty("$1_name"));
%}

//*****************************************************************************
// rp_tm_cpp_* - typemaps for the C++ Addin
//*****************************************************************************

// GROUP

// XPP

// rp_tm_cpp_ret - Function return type (F)
%typemap(rp_tm_cpp_ret) rp_tp_double "double";
%typemap(rp_tm_cpp_ret) void "bool";
%typemap(rp_tm_cpp_ret) SWIGTYPE "$1_type";

// rp_tm_cpp_prm - Function input parameter (F/C/M)
%typemap(rp_tm_cpp_prm) rp_tp_double "double";
%typemap(rp_tm_cpp_prm) const rp_tp_double & "const double &";
%typemap(rp_tm_cpp_prm) rp_tp_string "const std::string&";
%typemap(rp_tm_cpp_prm) rp_tp_cnv "const reposit::property_t&";
%typemap(rp_tm_cpp_prm) rp_tp_crc "const reposit::property_t&";
%typemap(rp_tm_cpp_prm) rp_tp_enm "const std::string&";
%typemap(rp_tm_cpp_prm) rp_tp_enm_cls "const std::string&";
%typemap(rp_tm_cpp_prm) rp_tp_obj1 "const std::string&";
%typemap(rp_tm_cpp_prm) SWIGTYPE "$rp_typedef_resolved";

// rp_tm_cpp_rtm - return type of a member function (M)
%typemap(rp_tm_cpp_rtm) rp_tp_double "double";
%typemap(rp_tm_cpp_rtm) void "bool";
%typemap(rp_tm_cpp_rtm) SWIGTYPE "$1_type";

// CPP

// rp_tm_cpp_cnv - Convert inputs from C++ Addin api types into underlying Library types (F/C/M)
%typemap(rp_tm_cpp_cnv) rp_tp_cnv %{
    $1_type $1_name_cnv =
        reposit::convert2<$1_type, reposit::property_t>($1_name);
%}

%typemap(rp_tm_cpp_cnv) rp_tp_crc %{
    $1_type $1_name_crc = reposit::CoerceImpl<
        reposit::property_t,
        $1_type>()($1_name);
%}

%typemap(rp_tm_cpp_cnv) rp_tp_enm %{
    $rp_typedef_base $1_name_enm =
        reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_cpp_cnv) rp_tp_enm_cls %{
    $1_type $1_name_enm_cls =
        reposit::Create<$1_type >()($1_name);
%}

// rp_tm_cpp_rtd - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_cpp_rtd) void "";
%typemap(rp_tm_cpp_rtd) rp_tp_double "double returnValue =";
%typemap(rp_tm_cpp_rtd) SWIGTYPE "return";

// rp_tm_cpp_rts - return statement (F/M)
%typemap(rp_tm_cpp_rts) rp_tp_double "return returnValue;";
%typemap(rp_tm_cpp_rts) void "return true;";
%typemap(rp_tm_cpp_rts) SWIGTYPE "";

// rp_tm_cpp_arg - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_cpp_arg) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_cpp_arg) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_cpp_arg) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_cpp_arg) rp_tp_enm_cls "$1_name_enm_cls";
%typemap(rp_tm_cpp_arg) rp_tp_obj1 "$1_name_get";
%typemap(rp_tm_cpp_arg) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_xll_* - typemaps for the Excel Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_xll_ret - function return type (F/M)
%typemap(rp_tm_xll_ret) void "bool*";
%typemap(rp_tm_xll_ret) bool "bool*";
%typemap(rp_tm_xll_ret) rp_tp_long "long*";
%typemap(rp_tm_xll_ret) rp_tp_double "double*";
%typemap(rp_tm_xll_ret) rp_tp_string "char*";
%typemap(rp_tm_xll_ret) rp_tp_enm "char*";
%typemap(rp_tm_xll_ret) std::vector "OPER*";

// rp_tm_xll_prm - function parameters (F/C/M)
%typemap(rp_tm_xll_prm) bool "bool*";
%typemap(rp_tm_xll_prm) rp_tp_long "long*";
%typemap(rp_tm_xll_prm) rp_tp_double "double*";
%typemap(rp_tm_xll_prm) const rp_tp_double & "double*";
%typemap(rp_tm_xll_prm) rp_tp_string "char*";
%typemap(rp_tm_xll_prm) rp_tp_string * "char*";
%typemap(rp_tm_xll_prm) rp_tp_string & "char*"; // DELETME
%typemap(rp_tm_xll_prm) rp_tp_string & "const char*";
%typemap(rp_tm_xll_prm) rp_tp_cnv "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_crc "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_any "OPER*";
%typemap(rp_tm_xll_prm) rp_tp_enm "char*";
%typemap(rp_tm_xll_prm) rp_tp_enm_cls "char*";
%typemap(rp_tm_xll_prm) std::vector "OPER *";
%typemap(rp_tm_xll_prm) std::vector const & "OPER *";
%typemap(rp_tm_xll_prm) rp_tp_obj1 "char*";

// rp_tm_xll_cnv - convert from Excel datatypes to the datatypes of the underlying Library
%typemap(rp_tm_xll_cnv) rp_tp_cnv %{
        $1_type $1_name_cnv = reposit::convert2<$1_type, reposit::ConvertOper>(
            reposit::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnv) rp_tp_crc %{
        $1_type $1_name_crc = reposit::CoerceImpl<
            reposit::ConvertOper, $1_type>()
            (reposit::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnv) rp_tp_enm %{
        $rp_typedef_base $1_name_enm =
            reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_xll_cnv) rp_tp_enm_cls %{
        $1_type $1_name_enm_cls =
            reposit::Create<$1_type >()($1_name);
%}

// FIXME this typemap should be split into two
%typemap(rp_tm_xll_cnv) rp_tp_cnv2 %{
        $rp_typedef_base $1_name_cnv = reposit::convert2<$rp_typedef_base>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_typedef_base());

        reposit::property_t $1_name_cnv2 = reposit::convert2<reposit::property_t>(
            reposit::ConvertOper(*$1_name));
%}
%typemap(rp_tm_xll_cnv) rp_tp_obj1 %{
        RP_GET_REFERENCE($1_name_obj, $1_name,
            $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

%typemap(rp_tm_xll_cnv) std::vector %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            reposit::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnv) const std::vector & %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            reposit::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

// rp_tm_xll_arv - arguments to the Value Object constructor (C)
%typemap(rp_tm_xll_arv) bool "*$1_name";
%typemap(rp_tm_xll_arv) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_arv) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_arv) rp_tp_cnv2 "$1_name_cnv2";
%typemap(rp_tm_xll_arv) const std::vector & "$1_name_vec";
%typemap(rp_tm_xll_arv) SWIGTYPE "$1_name";

// rp_tm_xll_arg - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_xll_arg) bool "*$1_name";
%typemap(rp_tm_xll_arg) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_arg) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_arg) const rp_tp_double & "*$1_name";
%typemap(rp_tm_xll_arg) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_xll_arg) rp_tp_cnv2 "$1_name_cnv";
%typemap(rp_tm_xll_arg) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_xll_arg) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xll_arg) rp_tp_enm_cls "$1_name_enm_cls";
%typemap(rp_tm_xll_arg) std::vector "$1_name_vec";
%typemap(rp_tm_xll_arg) const std::vector & "$1_name_vec";
%typemap(rp_tm_xll_arg) rp_tp_obj1 "$1_name_obj";
%typemap(rp_tm_xll_arg) SWIGTYPE "$1_name";

// rp_tm_xll_rts - return statement (F/M)
%typemap(rp_tm_xll_rts) SWIGTYPE %{
        static $1_type ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rts) void %{
        static bool ret = true;
        return &ret;
%}

%typemap(rp_tm_xll_rts) std::string %{
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(returnValue, ret);
        return ret;
%}

%typemap(rp_tm_xll_rts) std::vector< std::string > %{
        static OPER xRet;
        reposit::vectorToOper(returnValue, xRet);
        return &xRet;
%}

// rp_tm_xll_cod - code to register the parameter with Excel
%typemap(rp_tm_xll_cod) void "L";
%typemap(rp_tm_xll_cod) bool "L";
%typemap(rp_tm_xll_cod) rp_tp_long "N";
%typemap(rp_tm_xll_cod) rp_tp_double "E";
%typemap(rp_tm_xll_cod) const rp_tp_double & "E";
%typemap(rp_tm_xll_cod) rp_tp_string "C";
%typemap(rp_tm_xll_cod) rp_tp_string * "C";
%typemap(rp_tm_xll_cod) rp_tp_string & "C";
%typemap(rp_tm_xll_cod) rp_tp_string const & "C";
%typemap(rp_tm_xll_cod) rp_tp_cnv "P";
%typemap(rp_tm_xll_cod) rp_tp_crc "P";
%typemap(rp_tm_xll_cod) rp_tp_enm "C";
%typemap(rp_tm_xll_cod) rp_tp_enm_cls "C";
%typemap(rp_tm_xll_cod) rp_tp_any "P";
%typemap(rp_tm_xll_cod) std::vector "P";
%typemap(rp_tm_xll_cod) std::vector const & "P";
%typemap(rp_tm_xll_cod) rp_tp_obj1 "C";
%typemap(rp_tm_xll_cod) SWIGTYPE * "C";

//*****************************************************************************
// rp_tm_cfy_* - typemaps for the =countify Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_cfy_ret - Function return type (F)
%typemap(rp_tm_cfy_ret) std::string "const char *";
%typemap(rp_tm_cfy_ret) void "bool";
%typemap(rp_tm_cfy_ret) SWIGTYPE "$1_type";

// rp_tm_cfy_rtm - return value of a member function (M)
%typemap(rp_tm_cfy_rtm) rp_tp_double "double";
%typemap(rp_tm_cfy_rtm) void "bool";
%typemap(rp_tm_cfy_rtm) SWIGTYPE "$1_type";

// rp_tm_cfy_prm - function parameters (F/C/M)
%typemap(rp_tm_cfy_prm) rp_tp_double "double $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_string "const char * $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_enm "const char * $1_name";
%typemap(rp_tm_cfy_prm) rp_tp_obj1 "const char * $1_name";
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a dummy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_prm)  reposit::property_t const & "bool $1_name";
%typemap(rp_tm_cfy_prm) SWIGTYPE "$rp_typedef_resolved $1_name";

// rp_tm_cfy_cnv - convert input values from =countify datatypes into datatypes of the underlying Library (F/C/M)
%typemap(rp_tm_cfy_cnv) rp_tp_enm %{
        $rp_typedef_base $1_name_enum =
            reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_cfy_cnv) rp_tp_obj1 %{
        RP_GET_REFERENCE($1_name_get, $1_name, $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

// rp_tm_cfy_rtd - declare variable to capture return value of Library function (F)
%typemap(rp_tm_cfy_rtd) std::string %{
        static std::string ret;
        ret =
%}
%typemap(rp_tm_cfy_rtd) rp_tp_long %{
        long returnValue =
%}

// rp_tm_xll_rtd - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_xll_rtd) void "";
%typemap(rp_tm_xll_rtd) SWIGTYPE "$1_type returnValue =";

// rp_tm_cfy_rt2 - declare variable to capture return value of Library member function (M)
%typemap(rp_tm_cfy_rt2) void "";
%typemap(rp_tm_cfy_rt2) rp_tp_double "double returnValue =";

// rp_tm_cfy_arg - arguments to pass to underlying Library function
%typemap(rp_tm_cfy_arg) rp_tp_enm "$1_name_enum";
%typemap(rp_tm_cfy_arg) rp_tp_obj1 "$1_name_get";
%typemap(rp_tm_cfy_arg) SWIGTYPE "$1_name";

// rp_tm_cfy_rts - return statement of a Library function (F)
%typemap(rp_tm_cfy_rts) void "return true;";
%typemap(rp_tm_cfy_rts) rp_tp_long "return returnValue;";
%typemap(rp_tm_cfy_rts) std::string "return ret.c_str();";

// rp_tm_cfy_rtx - return statement of a Library member function (M)
%typemap(rp_tm_cfy_rtx) rp_tp_double "return returnValue;";
%typemap(rp_tm_cfy_rtx) void "return true;";

// rp_tm_cfy_rte - return statement in the event of an exception
%typemap(rp_tm_cfy_rte) SWIGTYPE "return 0;";

// TXT

// rp_tm_cfy_mng - generate the json to register functions with the mongo database
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a summy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_mng) reposit::property_t const & "BOOL";
%typemap(rp_tm_cfy_mng) void "BOOL";
%typemap(rp_tm_cfy_mng) bool "BOOL";
%typemap(rp_tm_cfy_mng) rp_tp_long "INT32";
%typemap(rp_tm_cfy_mng) rp_tp_double "REAL64";
%typemap(rp_tm_cfy_mng) SWIGTYPE "CSTR";

//*****************************************************************************
//*****************************************************************************
//*****************************************************************************

// rp_tm_cpp_rp_get:  This typemap is used for class member functions in the addin.
// We retrieve the object from the repository, and then extract the required reference.
// Default behavior: get a reference to the underlying library object.
%typemap(rp_tm_xxx_rp_get) SWIGTYPE %{
        RP_GET_REFERENCE(xxx, objectID, $rp_typedef_obj_add, $rp_typedef_obj_lib);
%}

// Sometimes the addin wants to override the library's behavior, in this case we get
// a reference to the addin object that wraps the library object:
%typemap(rp_tm_xxx_rp_get) rp_tp_add_obj %{
        RP_GET_OBJECT(xxx, objectID, $rp_typedef_obj_add);
%}

// Retrieve an instance of an enumerated class from the enumeration registry.
%typemap(rp_tm_xxx_rp_get) rp_tp_enm %{
        $1_type xxx =
            reposit::Create<$1_type>()(objectID);
%}

// Call a member function on an addin object rather than a library object
%typemap(rp_tm_xxx_rp_get) rp_tp_obj2 %{
        RP_GET_OBJECT(xxx, objectID, $rp_typedef_obj_add);
%}

// Map native types to reposit types

%apply rp_tp_long { long };

%apply rp_tp_double { double };
%apply const rp_tp_double & { const double & };

%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_string { std::string const & };

%apply rp_tp_any { const reposit::property_t& };

