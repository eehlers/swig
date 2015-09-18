
//*****************************************************************************
// defines
//*****************************************************************************

#define %override %pragma(reposit) override_obj="true";
#define %group(x)  %pragma(reposit) group="x";
#define %loop(func, parm) %feature("rp:loopParameter", "parm") func;

//*****************************************************************************
// rp_tm_xxx_* - typemaps shared by multiple addins
//*****************************************************************************

// rp_tm_default - not a real typemap, implemented in code.  Always returns the type.

// rp_tm_xxx_rp_get:  This typemap is used for class member functions in the addin.
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

// rp_tm_xxx_loop - arguments to boost::bind object for a looping function (F/M)
%typemap(rp_tm_xxx_loop) bool "*$1_name";
%typemap(rp_tm_xxx_loop) std::vector "_1";
%typemap(rp_tm_xxx_loop) rp_tp_cnv2 "$1_name_cnv";
%typemap(rp_tm_xxx_loop) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xxx_loop) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_lib_* - typemaps for Library Objects
//*****************************************************************************

// GROUP

// HPP

// Not a real typemap. Defined in code.  RP_OBJ_CLASS (C)
// rp_tm_lib_cls

// CPP

// rp_tm_lib_rtst - Return statement (F)
%typemap(rp_tm_lib_rtst) void "";
%typemap(rp_tm_lib_rtst) SWIGTYPE "return ";

//*****************************************************************************
// rp_tm_vob_* - typemaps for Value Objects
//*****************************************************************************

// GROUP

// XPP

// rp_tm_vob_parm - Value Object constructor parameters (C)
%typemap(rp_tm_vob_parm) rp_tp_long "long";
%typemap(rp_tm_vob_parm) rp_tp_double "double";
%typemap(rp_tm_vob_parm) rp_tp_string "const std::string &";
%typemap(rp_tm_vob_parm) rp_tp_any "const reposit::property_t&";
%typemap(rp_tm_vob_parm) rp_tp_enm "const std::string &";
%typemap(rp_tm_vob_parm) rp_tp_obj1 "const std::string &";
%typemap(rp_tm_vob_parm) std::vector<rp_tp_long> const & "const std::vector<long>&";
%typemap(rp_tm_vob_parm) std::vector<rp_tp_double> const & "const std::vector<double>&";
%typemap(rp_tm_vob_parm) SWIGTYPE "$1_type";

// HPP

// rp_tm_vob_mbvr - Value Object class member variables (C)
%typemap(rp_tm_vob_mbvr) rp_tp_long "long $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_double "double $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_string "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_any "reposit::property_t $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_enm "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_obj1 "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<bool> const & "std::vector<bool> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<rp_tp_long> const & "std::vector<long> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<rp_tp_double> const & "std::vector<double> $1_name_";
%typemap(rp_tm_vob_mbvr) SWIGTYPE "$1_type $1_name_";

// rp_tm_vob_srmv - code to serialize a Value Object member variable (C)
%typemap(rp_tm_vob_srmv) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

// CPP

// rp_tm_vob_name - Insert the parameter name into the vo property names array (C)
%typemap(rp_tm_vob_name) SWIGTYPE "\"$1_name\"";

// rp_tm_vob_init - vo ctor initializer list (C)
%typemap(rp_tm_vob_init) SWIGTYPE "$1_name_($1_name)";

// rp_tm_vob_cnvt - in ValueObject::setSystemProperty, convert value from property_t to native type (C)
%typemap(rp_tm_vob_cnvt) rp_tp_long "reposit::convert2<long>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_double "reposit::convert2<double>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_string "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_any "value";
%typemap(rp_tm_vob_cnvt) rp_tp_enm "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_obj1 "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnvt) std::vector<bool> const & "reposit::vector::convert2<bool>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<rp_tp_long> const & "reposit::vector::convert2<long>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<rp_tp_double> const & "reposit::vector::convert2<double>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) SWIGTYPE "reposit::convert2<$1_type>(value)";

//*****************************************************************************
// rp_tm_scr_* - typemaps for Serialization - Create
//*****************************************************************************

// GROUP

// CPP

// rp_tm_scr_cnvt - convert types from the Value Object to the corresponding Library Object (C)
%typemap(rp_tm_scr_cnvt) rp_tp_long %{
    long $1_name =
        reposit::convert2<long>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) rp_tp_enm %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
    $rp_typedef_base $1_name =
        reposit::Create<$rp_typedef_base>()($1_name_str);
%}

%typemap(rp_tm_scr_cnvt) rp_tp_obj1 %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_REFERENCE($1_name, $1_name_str,
        $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

%typemap(rp_tm_scr_cnvt) const std::vector<bool>& %{
   std::vector<bool> $1_name =
        reposit::vector::convert2<bool>(valueObject->getProperty("$1_name"), "$1_name");
%}

%typemap(rp_tm_scr_cnvt) SWIGTYPE %{
    $rp_typedef_base $1_name =
        reposit::convert2<$rp_typedef_base>(
            valueObject->getProperty("$1_name"));
%}

//*****************************************************************************
// rp_tm_cpp_* - typemaps for the C++ Addin
//*****************************************************************************

// GROUP

// XPP

// rp_tm_cpp_rttp - Function return type (F)
%typemap(rp_tm_cpp_rttp) void "bool";
%typemap(rp_tm_cpp_rttp) rp_tp_double "double";
%typemap(rp_tm_cpp_rttp) SWIGTYPE "$1_type";

// rp_tm_cpp_parm - Function input parameter (F/C/M)
%typemap(rp_tm_cpp_parm) rp_tp_double "double";
%typemap(rp_tm_cpp_parm) rp_tp_double const & "const double &";
%typemap(rp_tm_cpp_parm) rp_tp_string "const std::string&";
%typemap(rp_tm_cpp_parm) rp_tp_cnv "const reposit::property_t&";
%typemap(rp_tm_cpp_parm) rp_tp_crc "const reposit::property_t&";
%typemap(rp_tm_cpp_parm) rp_tp_enm "const std::string&";
%typemap(rp_tm_cpp_parm) rp_tp_enm_cls "const std::string&";
%typemap(rp_tm_cpp_parm) rp_tp_obj1 "const std::string&";
%typemap(rp_tm_cpp_parm) SWIGTYPE "$rp_typedef_resolved";

// rp_tm_cpp_rtmb - return type of a member function (M)
%typemap(rp_tm_cpp_rtmb) void "bool";
%typemap(rp_tm_cpp_rtmb) rp_tp_double "double";
%typemap(rp_tm_cpp_rtmb) SWIGTYPE "$1_type";

// CPP

// rp_tm_cpp_cnvt - Convert inputs from C++ Addin api types into underlying Library types (F/C/M)
%typemap(rp_tm_cpp_cnvt) rp_tp_cnv %{
    $1_type $1_name_cnv =
        reposit::convert2<$1_type, reposit::property_t>($1_name);
%}

%typemap(rp_tm_cpp_cnvt) rp_tp_crc %{
    $1_type $1_name_crc = reposit::CoerceImpl<
        reposit::property_t,
        $1_type>()($1_name);
%}

%typemap(rp_tm_cpp_cnvt) rp_tp_enm %{
    $rp_typedef_base $1_name_enm =
        reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_cpp_cnvt) rp_tp_enm_cls %{
    $1_type $1_name_enm_cls =
        reposit::Create<$1_type >()($1_name);
%}

// rp_tm_cpp_rtdc - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_cpp_rtdc) void "";
%typemap(rp_tm_cpp_rtdc) rp_tp_double "double returnValue =";
%typemap(rp_tm_cpp_rtdc) SWIGTYPE "return";

// rp_tm_cpp_rtst - return statement (F/M)
%typemap(rp_tm_cpp_rtst) void "return true;";
%typemap(rp_tm_cpp_rtst) rp_tp_double "return returnValue;";
%typemap(rp_tm_cpp_rtst) SWIGTYPE "";

// rp_tm_cpp_args - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_cpp_args) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_cpp_args) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_cpp_args) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_cpp_args) rp_tp_enm_cls "$1_name_enm_cls";
%typemap(rp_tm_cpp_args) rp_tp_obj1 "$1_name_get";
%typemap(rp_tm_cpp_args) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_xll_* - typemaps for the Excel Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_xll_rtft - function return type (F/M)
%typemap(rp_tm_xll_rtft) void "bool*";
%typemap(rp_tm_xll_rtft) bool "bool*";
%typemap(rp_tm_xll_rtft) rp_tp_long "long*";
%typemap(rp_tm_xll_rtft) rp_tp_double "double*";
%typemap(rp_tm_xll_rtft) rp_tp_string "char*";
%typemap(rp_tm_xll_rtft) rp_tp_enm "char*";
%typemap(rp_tm_xll_rtft) std::vector "OPER*";

// rp_tm_xll_parm - function parameters (F/C/M)
%typemap(rp_tm_xll_parm) bool "bool*";
%typemap(rp_tm_xll_parm) rp_tp_long "long*";
%typemap(rp_tm_xll_parm) rp_tp_double "double*";
%typemap(rp_tm_xll_parm) rp_tp_double const & "double*";
%typemap(rp_tm_xll_parm) rp_tp_string "char*";
%typemap(rp_tm_xll_parm) rp_tp_string * "char*";
%typemap(rp_tm_xll_parm) rp_tp_string & "char*";
%typemap(rp_tm_xll_parm) rp_tp_cnv "OPER*";
%typemap(rp_tm_xll_parm) rp_tp_crc "OPER*";
%typemap(rp_tm_xll_parm) rp_tp_any "OPER*";
%typemap(rp_tm_xll_parm) rp_tp_enm "char*";
%typemap(rp_tm_xll_parm) rp_tp_enm_cls "char*";
%typemap(rp_tm_xll_parm) std::vector "OPER *";
%typemap(rp_tm_xll_parm) std::vector const & "OPER *";
%typemap(rp_tm_xll_parm) rp_tp_obj1 "char*";

// rp_tm_xll_cnvt - convert from Excel datatypes to the datatypes of the underlying Library
%typemap(rp_tm_xll_cnvt) rp_tp_cnv %{
        $1_type $1_name_cnv = reposit::convert2<$1_type, reposit::ConvertOper>(
            reposit::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnvt) rp_tp_crc %{
        $1_type $1_name_crc = reposit::CoerceImpl<
            reposit::ConvertOper, $1_type>()
            (reposit::ConvertOper(*$1_name));
%}

%typemap(rp_tm_xll_cnvt) rp_tp_enm %{
        $rp_typedef_base $1_name_enm =
            reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_xll_cnvt) rp_tp_enm_cls %{
        $1_type $1_name_enm_cls =
            reposit::Create<$1_type >()($1_name);
%}

// FIXME this typemap should be split into two
%typemap(rp_tm_xll_cnvt) rp_tp_cnv2 %{
        $rp_typedef_base $1_name_cnv = reposit::convert2<$rp_typedef_base>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_typedef_base());

        reposit::property_t $1_name_cnv2 = reposit::convert2<reposit::property_t>(
            reposit::ConvertOper(*$1_name));
%}
%typemap(rp_tm_xll_cnvt) rp_tp_obj1 %{
        RP_GET_REFERENCE($1_name_obj, $1_name,
            $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

%typemap(rp_tm_xll_cnvt) std::vector %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            reposit::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnvt) const std::vector & %{
        std::vector<$rp_typedef_no_template> $1_name_vec =
            reposit::operToVector<$rp_typedef_no_template>(*$1_name, "$1_name");
%}

// rp_tm_xll_argfv - arguments to the Value Object constructor (C)
%typemap(rp_tm_xll_argfv) bool "*$1_name";
%typemap(rp_tm_xll_argfv) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argfv) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_argfv) rp_tp_cnv2 "$1_name_cnv2";
%typemap(rp_tm_xll_argfv) std::vector const & "$1_name_vec";
%typemap(rp_tm_xll_argfv) SWIGTYPE "$1_name";

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_xll_argf) bool "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_double "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_xll_argf) rp_tp_cnv2 "$1_name_cnv";
%typemap(rp_tm_xll_argf) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_xll_argf) rp_tp_enm "$1_name_enm";
%typemap(rp_tm_xll_argf) rp_tp_enm_cls "$1_name_enm_cls";
%typemap(rp_tm_xll_argf) std::vector "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector const & "$1_name_vec";
%typemap(rp_tm_xll_argf) rp_tp_obj1 "$1_name_obj";
%typemap(rp_tm_xll_argf) SWIGTYPE "$1_name";

// rp_tm_xll_rtdc - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_xll_rtdc) void "";
%typemap(rp_tm_xll_rtdc) SWIGTYPE "$1_type returnValue =";

// rp_tm_xll_rtst - return statement (F/M)
%typemap(rp_tm_xll_rtst) SWIGTYPE %{
        static $1_type ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) void %{
        static bool ret = true;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) std::string %{
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(returnValue, ret);
        return ret;
%}

%typemap(rp_tm_xll_rtst) std::vector< std::string > %{
        static OPER xRet;
        reposit::vectorToOper(returnValue, xRet);
        return &xRet;
%}

// rp_tm_xll_code - code to register the parameter with Excel
%typemap(rp_tm_xll_code) void "L";
%typemap(rp_tm_xll_code) bool "L";
%typemap(rp_tm_xll_code) rp_tp_long "N";
%typemap(rp_tm_xll_code) rp_tp_double "E";
%typemap(rp_tm_xll_code) rp_tp_double const & "E";
%typemap(rp_tm_xll_code) rp_tp_string "C";
%typemap(rp_tm_xll_code) rp_tp_string * "C";
%typemap(rp_tm_xll_code) rp_tp_string & "C";
%typemap(rp_tm_xll_code) rp_tp_string const & "C";
%typemap(rp_tm_xll_code) rp_tp_cnv "P";
%typemap(rp_tm_xll_code) rp_tp_crc "P";
%typemap(rp_tm_xll_code) rp_tp_enm "C";
%typemap(rp_tm_xll_code) rp_tp_enm_cls "C";
%typemap(rp_tm_xll_code) rp_tp_any "P";
%typemap(rp_tm_xll_code) std::vector "P";
%typemap(rp_tm_xll_code) std::vector const & "P";
%typemap(rp_tm_xll_code) rp_tp_obj1 "C";
%typemap(rp_tm_xll_code) SWIGTYPE * "C";

//*****************************************************************************
// rp_tm_cfy_* - typemaps for the =countify Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_cfy_rtfn - Function return type (F)
%typemap(rp_tm_cfy_rtfn) void "bool";
%typemap(rp_tm_cfy_rtfn) std::string "const char *";
%typemap(rp_tm_cfy_rtfn) SWIGTYPE "$1_type";

// rp_tm_cfy_rtmb - return value of a member function (M)
%typemap(rp_tm_cfy_rtmb) void "bool";
%typemap(rp_tm_cfy_rtmb) rp_tp_double "double";
%typemap(rp_tm_cfy_rtmb) SWIGTYPE "$1_type";

// rp_tm_cfy_parm - function parameters (F/C/M)
%typemap(rp_tm_cfy_parm) rp_tp_double "double $1_name";
%typemap(rp_tm_cfy_parm) rp_tp_string "const char * $1_name";
%typemap(rp_tm_cfy_parm) rp_tp_enm "const char * $1_name";
%typemap(rp_tm_cfy_parm) rp_tp_obj1 "const char * $1_name";
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a dummy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_parm)  reposit::property_t const & "bool $1_name";
%typemap(rp_tm_cfy_parm) SWIGTYPE "$rp_typedef_resolved $1_name";

// rp_tm_cfy_cnvt - convert input values from =countify datatypes into datatypes of the underlying Library (F/C/M)
%typemap(rp_tm_cfy_cnvt) rp_tp_enm %{
        $rp_typedef_base $1_name_enum =
            reposit::Create<$rp_typedef_base>()($1_name);
%}

%typemap(rp_tm_cfy_cnvt) rp_tp_obj1 %{
        RP_GET_REFERENCE($1_name_get, $1_name, $rp_addin_namespace::$rp_typedef_no_namespace, $rp_typedef_no_template);
%}

// rp_tm_cfy_rtdf - declare variable to capture return value of Library function (F)
%typemap(rp_tm_cfy_rtdf) std::string %{
        static std::string ret;
        ret =
%}
%typemap(rp_tm_cfy_rtdf) rp_tp_long %{
        long returnValue =
%}

// rp_tm_cfy_rtdm - declare variable to capture return value of Library member function (M)
%typemap(rp_tm_cfy_rtdm) void "";
%typemap(rp_tm_cfy_rtdm) rp_tp_double "double returnValue =";

// rp_tm_cfy_args - arguments to pass to underlying Library function
%typemap(rp_tm_cfy_args) rp_tp_enm "$1_name_enum";
%typemap(rp_tm_cfy_args) rp_tp_obj1 "$1_name_get";
%typemap(rp_tm_cfy_args) SWIGTYPE "$1_name";

// rp_tm_cfy_rtsf - return statement of a Library function (F)
%typemap(rp_tm_cfy_rtsf) void "return true;";
%typemap(rp_tm_cfy_rtsf) rp_tp_long "return returnValue;";
%typemap(rp_tm_cfy_rtsf) std::string "return ret.c_str();";

// rp_tm_cfy_rtsm - return statement of a Library member function (M)
%typemap(rp_tm_cfy_rtsm) rp_tp_double "return returnValue;";
%typemap(rp_tm_cfy_rtsm) void "return true;";

// rp_tm_cfy_rtex - return statement in the event of an exception
%typemap(rp_tm_cfy_rtex) SWIGTYPE "return 0;";

// TXT

// rp_tm_cfy_mngo - generate the json to register functions with the mongo database
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a summy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_mngo) void "BOOL";
%typemap(rp_tm_cfy_mngo) bool "BOOL";
%typemap(rp_tm_cfy_mngo) rp_tp_long "INT32";
%typemap(rp_tm_cfy_mngo) rp_tp_double "REAL64";
%typemap(rp_tm_cfy_mngo) reposit::property_t const & "BOOL";
%typemap(rp_tm_cfy_mngo) SWIGTYPE "CSTR";

//*****************************************************************************
// %apply statements - map native types to reposit types
//*****************************************************************************

%apply rp_tp_long { long };

%apply rp_tp_double { double };
%apply const rp_tp_double & { const double & };

%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_string { std::string const & };

%apply rp_tp_any { const reposit::property_t& };

