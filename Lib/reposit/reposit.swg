
//*****************************************************************************
// defines
//*****************************************************************************

#define %override %pragma(reposit) override_obj="true"
#define %group(x)  %pragma(reposit) group="x"
#define %groupCaption(x)  %pragma(reposit) groupCaption="x"
#define %groupFunctionWizard(x)  %pragma(reposit) groupFunctionWizard="x"
#define %loop(func, parm) %feature("rp:loopParameter", "parm") func
#define %processor(cls, proc) %feature("rp:processorName", "proc") cls
#define %generate(lang, id) %feature("rp:generate:lang") id
#define %generateall(lang) %feature("rp:generate:lang")
#define %alias(id, id2) %feature("rp:alias", "id2") id
#define %rename2(id, id2) %feature("rp:rename2", "id2") id

//*****************************************************************************
// rp_tm_xxx_* - typemaps shared by multiple addins
//*****************************************************************************

// Not a real typemap, implemented in code.  Always returns the type.
// rp_tm_default

//*****************************************************************************
// rp_tm_lib_* - typemaps for Library Objects
//*****************************************************************************

// GROUP

// HPP

// Not a real typemap. Defined in code.  RP_OBJ_CLASS (C)
// rp_tm_lib_cls

// CPP

// rp_tm_lib_rtst - Return statement (F)

//*****************************************************************************
// rp_tm_vob_* - typemaps for Value Objects
//*****************************************************************************

// GROUP

// XPP

// rp_tm_vob_parm - Value Object constructor parameters (C)

// HPP

// rp_tm_vob_mbvr - Value Object class member variables (C)

// rp_tm_vob_srmv - code to serialize a Value Object member variable (C)

// CPP

// rp_tm_vob_name - Insert the parameter name into the vo property names array (C)

// rp_tm_vob_init - vo ctor initializer list (C)

// rp_tm_vob_cnvt - in ValueObject::setSystemProperty, convert value from property_t to native type (C)

//*****************************************************************************
// rp_tm_scr_* - typemaps for Serialization - Create
//*****************************************************************************

// GROUP

// CPP

// rp_tm_scr_cnvt - convert types from the Value Object to the corresponding Library Object (C)

//*****************************************************************************
// rp_tm_cpp_* - typemaps for the C++ Addin
//*****************************************************************************

// GROUP

// XPP

// rp_tm_cpp_rttp - Function return type (F)

// rp_tm_cpp_parm - Function input parameter (F/C/M)

// rp_tm_cpp_rtmb - return type of a member function (M)

// CPP

// rp_tm_cpp_cnvt - Convert inputs from C++ Addin api types into underlying Library types (F/C/M)

// rp_tm_cpp_rtdc - declare variable to capture return value of Library function (F/M)

// rp_tm_cpp_rtst - return statement (F/M)

// rp_tm_cpp_args - arguments to the underlying Library function (F/C/M)

//*****************************************************************************
// rp_tm_xll_* - typemaps for the Excel Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_xll_rtft - function return type (F/M)

%typemap(rp_tm_xll_rtft) void "bool*";
%typemap(rp_tm_xll_rtft) bool "bool*";
%typemap(rp_tm_xll_rtft) rp_tp_long "long*";
%typemap(rp_tm_xll_rtft) double "double*";
%typemap(rp_tm_xll_rtft) rp_tp_string "char*";
%typemap(rp_tm_xll_rtft) std::vector "OPER*";
%typemap(rp_tm_xll_rtft) std::vector & "OPER*";

// rp_tm_xll_parm - function parameters (F/C/M)

// rp_tm_xll_parm - function parameters (F/C/M)
%typemap(rp_tm_xll_parm) bool "bool*";
%typemap(rp_tm_xll_parm) rp_tp_long "long*";
%typemap(rp_tm_xll_parm) double "double*";
%typemap(rp_tm_xll_parm) double & "double*";
%typemap(rp_tm_xll_parm) rp_tp_string "char*";
%typemap(rp_tm_xll_parm) std::vector<std::vector<double> > & "FP*";
%typemap(rp_tm_xll_parm) SWIGTYPE "OPER*";

%typemap(rp_tm_xll_parm2) SWIGTYPE "OPER*";

// rp_tm_xll_cnvt - convert from Excel datatypes to the datatypes of the underlying Library

%typemap(rp_tm_xll_cnvt2) bool %{
        bool $1_name_cnv = reposit::convert<bool>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_value);
%}

%typemap(rp_tm_xll_cnvt2) std::vector<bool> & %{
        std::vector<bool> $1_name_vec =
            reposit::operToVector<bool>(*$1_name, "$1_name");
%}

// rp_tm_xll_argfv - arguments to the Value Object constructor (C)

%typemap(rp_tm_xll_argfv) bool "*$1_name";
%typemap(rp_tm_xll_argfv) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argfv) double "*$1_name";
%typemap(rp_tm_xll_argfv) std::vector const & "$1_name_vec";
%typemap(rp_tm_xll_argfv) SWIGTYPE "$1_name";

%typemap(rp_tm_xll_argfv2) std::vector const & "$1_name_vec";
%typemap(rp_tm_xll_argfv2) SWIGTYPE "$1_name_cnv";

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)

%typemap(rp_tm_xll_argf) bool "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argf) double "*$1_name";
%typemap(rp_tm_xll_argf) double & "*$1_name";
%typemap(rp_tm_xll_argf) std::vector "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argf) SWIGTYPE "$1_name";

%typemap(rp_tm_xll_argf2) std::vector "$1_name_vec";
%typemap(rp_tm_xll_argf2) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argf2) SWIGTYPE "$1_name_cnv";

// rp_tm_xll_rtdc - declare variable to capture return value of Library function (F/M)

%typemap(rp_tm_xll_rtdc) void "";
%typemap(rp_tm_xll_rtdc) SWIGTYPE "$1_type returnValue =";

// rp_tm_xll_rtst - return statement (F/M)

%typemap(rp_tm_xll_rtst) bool %{
        static bool ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) void %{
        static bool ret = true;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) rp_tp_string %{
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(returnValue, ret);
        return ret;
%}

%typemap(rp_tm_xll_rtst) std::vector<std::string> %{
        static OPER xRet;
        reposit::vectorToOper(returnValue, xRet);
        return &xRet;
%}

// rp_tm_xll_cdrt - code to register the return type with Excel

%typemap(rp_tm_xll_cdrt) void "L";
%typemap(rp_tm_xll_cdrt) bool "L";
%typemap(rp_tm_xll_cdrt) rp_tp_long "N";
%typemap(rp_tm_xll_cdrt) double "E";
%typemap(rp_tm_xll_cdrt) double & "E";
%typemap(rp_tm_xll_cdrt) rp_tp_string "C";
%typemap(rp_tm_xll_cdrt) rp_tp_any "P";
%typemap(rp_tm_xll_cdrt) std::vector "P";
%typemap(rp_tm_xll_cdrt) std::vector & "P";
%typemap(rp_tm_xll_cdrt) SWIGTYPE * "C";

// rp_tm_xll_code - code to register the parameter with Excel

%typemap(rp_tm_xll_code) void "L";
%typemap(rp_tm_xll_code) bool "L";
%typemap(rp_tm_xll_code) rp_tp_long "N";
%typemap(rp_tm_xll_code) double "E";
%typemap(rp_tm_xll_code) double & "E";
%typemap(rp_tm_xll_code) rp_tp_string "C";
%typemap(rp_tm_xll_code) rp_tp_any "P";
%typemap(rp_tm_xll_code) std::vector "P";
%typemap(rp_tm_xll_code) std::vector & "P";
%typemap(rp_tm_xll_code) SWIGTYPE * "C";

%typemap(rp_tm_xll_code2) SWIGTYPE "P";

// rp_tm_xll_loop - arguments to boost::bind object for a looping function (F/M)

%typemap(rp_tm_xll_loop) bool "*$1_name";
%typemap(rp_tm_xll_loop) std::vector "_1";
%typemap(rp_tm_xll_loop) SWIGTYPE "$1_name";

%typemap(rp_tm_xll_loop2) SWIGTYPE "$1_name_cnv";

// rp_tm_xll_lpfn - datatype of the loop function for call to boost::bind
%typemap(rp_tm_xll_lpfn) SWIGTYPE "$1_type";

// rp_tm_xll_lppm - datatype of the loop parameter for call to boost::bind
%typemap(rp_tm_xll_lppm) rp_tp_long "long";
%typemap(rp_tm_xll_lppm) SWIGTYPE "$1_basetype";
%typemap(rp_tm_xll_lppm2) SWIGTYPE "$1_type";

//*****************************************************************************
// %apply statements - map native types to reposit types
//*****************************************************************************

%apply rp_tp_long { int };
%apply rp_tp_long { std::size_t };
%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_any { reposit::property_t & };

//*****************************************************************************
// Macros - for certain kinds of types - repository objects, enumerated types
// and classes - you can call the macros below to define all relevant typedefs
// for the given type.
//*****************************************************************************

// enumerated types
%define ENUMERATED_TYPE(T...)
// T
%typemap(rp_tm_xxx_rp_get) T %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_xll_loop) T "$1_name_enm";
%typemap(rp_tm_xll_loop2) T "$1_name_enm";
%typemap(rp_tm_csh_loop) T "$1_name_enm";
%typemap(rp_tm_vob_parm) T "const std::string &";
%typemap(rp_tm_vob_mbvr) T "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    T $1_name =
        reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T "const std::string&";
%typemap(rp_tm_cpp_cnvt) T %{
    T $1_name_enm =
        reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cpp_args) T "$1_name_enm";
%typemap(rp_tm_csh_parm) T "const char *";
%typemap(rp_tm_csh_cnvt) T %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_csh_args) T "$1_name_enm";
%typemap(rp_tm_csh_clcp) T "StringBuilder";
%typemap(rp_tm_xll_rtft) T "char*";
%typemap(rp_tm_xll_parm) T "char*";
%typemap(rp_tm_xll_cnvt) T %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        T $1_name_enm =
            reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_argf2) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";

%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
        T $1_name_enum =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enum";
%typemap(rp_tm_xll_argfv) T "$1_name";
%typemap(rp_tm_xll_argfv2) T "$1_name_str";

%typemap(rp_tm_xll_rtst) T %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_dox_rtdc) T "string returnValue";
%typemap(rp_tm_dox_mbvr) T "string $1_name";

// T &
%typemap(rp_tm_xxx_rp_get) T & %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_xll_loop) T & "$1_name_enm";
%typemap(rp_tm_xll_loop2) T & "$1_name_enm";
%typemap(rp_tm_csh_loop) T & "$1_name_enm";
%typemap(rp_tm_vob_parm) T & "const std::string &";
%typemap(rp_tm_vob_mbvr) T & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T & "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    T $1_name =
        reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T & "const std::string&";
%typemap(rp_tm_cpp_cnvt) T & %{
    T $1_name_enm =
        reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cpp_args) T & "$1_name_enm";
%typemap(rp_tm_csh_parm) T & "const char *";
%typemap(rp_tm_csh_cnvt) T & %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_csh_args) T & "$1_name_enm";
%typemap(rp_tm_csh_clcp) T & "StringBuilder";
%typemap(rp_tm_xll_rtft) T & "char*";
%typemap(rp_tm_xll_parm) T & "char*";
%typemap(rp_tm_xll_cnvt) T & %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T & %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        T $1_name_enm =
            reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T & "$1_name_enm";
%typemap(rp_tm_xll_argf2) T & "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T & "C";
%typemap(rp_tm_xll_code) T & "C";
%typemap(rp_tm_cfy_parm) T & "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T & %{
        T $1_name_enum =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cfy_args) T & "$1_name_enum";
%typemap(rp_tm_xll_argfv) T & "$1_name";
%typemap(rp_tm_xll_argfv2) T & "$1_name_str";

%typemap(rp_tm_xll_rtst) T & %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_dox_rtdc) T & "string returnValue";
%typemap(rp_tm_dox_mbvr) T & "string $1_name";
%typemap(rp_tm_dox_rtd2) SWIGTYPE "ZZZ returnValue";

%enddef

// enumerated class
%define ENUMERATED_CLASS(T...)
// T
%typemap(rp_tm_xxx_rp_get) T %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_xll_loop) T "$1_name_enm";
%typemap(rp_tm_xll_loop2) T "$1_name_enm";
%typemap(rp_tm_vob_parm) T "const std::string &";
%typemap(rp_tm_vob_mbvr) T "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    T $1_name =
        reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T "const std::string&";
%typemap(rp_tm_cpp_cnvt) T %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cpp_args) T "$1_name_enm";

%typemap(rp_tm_csh_parm) T "const char *";
%typemap(rp_tm_csh_cnvt) T %{
            T $1_name_enm =
                reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_csh_cnvt) T %{
        T $1_name_enm =
            reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_csh_args) T "$1_name_enm";
%typemap(rp_tm_csh_clcp) T "StringBuilder";
%typemap(rp_tm_csh_rttp) T "char *";

%typemap(rp_tm_xll_rtft) T "char*";
%typemap(rp_tm_xll_parm) T "char*";
%typemap(rp_tm_xll_cnvt) T %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        T $1_name_enm =
            reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_argf2) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";

%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enm";
%typemap(rp_tm_xll_argfv) T "$1_name";
%typemap(rp_tm_xll_argfv2) T "$1_name_str";

%typemap(rp_tm_xll_rtst) T %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_dox_rtdc) T "string returnValue";
%typemap(rp_tm_dox_mbvr) T "string $1_name";
%typemap(rp_tm_dox_rtd2) SWIGTYPE "ID of Enumeration of class T";

// T &
%typemap(rp_tm_xxx_rp_get) T & %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_xll_loop) T & "$1_name_enm";
%typemap(rp_tm_xll_loop2) T & "$1_name_enm";
%typemap(rp_tm_vob_parm) T & "const std::string &";
%typemap(rp_tm_vob_mbvr) T & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T & "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    T $1_name =
        reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T & "const std::string&";
%typemap(rp_tm_cpp_cnvt) T & %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cpp_args) T & "$1_name_enm";

%typemap(rp_tm_csh_parm) T & "const char *";
%typemap(rp_tm_csh_cnvt) T & %{
            T $1_name_enm =
                reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_csh_cnvt) T & %{
        T $1_name_enm =
            reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_csh_args) T & "$1_name_enm";
%typemap(rp_tm_csh_clcp) T & "StringBuilder";
%typemap(rp_tm_csh_rttp) T & "char *";
%typemap(rp_tm_csh_rscp) T & "string";
%typemap(rp_tm_csh_rtst) T & %{
        std::ostringstream os;
        os << returnValue;
        ULONG size = os.str().length() + sizeof(char);
        char *ret = (char*)::CoTaskMemAlloc(size);
        strcpy(ret, os.str().c_str());
        return ret;
%}

%typemap(rp_tm_xll_rtft) T & "char*";
%typemap(rp_tm_xll_parm) T & "char*";
%typemap(rp_tm_xll_cnvt) T & %{
        T $1_name_enm =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T & %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        T $1_name_enm =
            reposit::Create<T>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T & "$1_name_enm";
%typemap(rp_tm_xll_argf2) T & "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T & "C";
%typemap(rp_tm_xll_code) T & "C";

%typemap(rp_tm_xll_rtst) T & %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_cfy_parm) T & "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T & %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cfy_args) T & "$1_name_enm";
%typemap(rp_tm_xll_argfv) T & "$1_name";
%typemap(rp_tm_xll_argfv2) T & "$1_name_str";

%typemap(rp_tm_dox_rtdc) T & "string returnValue";
%typemap(rp_tm_dox_mbvr) T & "string $1_name";
%typemap(rp_tm_dox_rtd2) SWIGTYPE "ID of Enumeration of class T";

%enddef

// repository objects
%define OBJECT_WRAPPER(T_ADDIN,T_LIB...)

%typemap(rp_tm_xxx_rp_get) T_LIB %{
        RP_GET_REFERENCE(xxx, objectID, T_ADDIN, T_LIB);
%}
%typemap(rp_tm_vob_parm) boost::shared_ptr<T_LIB> const & "const std::string &";
%typemap(rp_tm_vob_mbvr) boost::shared_ptr<T_LIB> const & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) boost::shared_ptr<T_LIB> const & "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) boost::shared_ptr<T_LIB> const & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_REFERENCE($1_name, $1_name_str,
        T_ADDIN ,  T_LIB );
%}
%typemap(rp_tm_cpp_parm) boost::shared_ptr<T_LIB> const & "const std::string&";
%typemap(rp_tm_cpp_args) boost::shared_ptr<T_LIB> const & "$1_name_get";
%typemap(rp_tm_csh_parm) boost::shared_ptr<T_LIB> const & "const char *";
%typemap(rp_tm_csh_args) boost::shared_ptr<T_LIB> const & "$1_name_get";
%typemap(rp_tm_csh_clcp) boost::shared_ptr<T_LIB> const & "StringBuilder";
%typemap(rp_tm_csh_cnvt) boost::shared_ptr<T_LIB> const & %{
        RP_GET_REFERENCE($1_name_obj, $1_name,
            T_ADDIN ,  T_LIB );
%}
%typemap(rp_tm_csh_cnvt) std::vector<boost::shared_ptr<T_LIB> > const & %{
        std::vector<std::string> $1_name_vec($1_name, $1_name + $1_name_length);
        std::vector<boost::shared_ptr<T_LIB> > $1_name_vec2 =
            reposit::getLibraryObjectVector<T_ADDIN, T_LIB>($1_name_vec);
%}

%typemap(rp_tm_csh_args) boost::shared_ptr<T_LIB> const & "$1_name_obj";
%typemap(rp_tm_xll_parm) boost::shared_ptr<T_LIB> const & "char*";
%typemap(rp_tm_xll_cnvt) boost::shared_ptr<T_ADDIN> const & %{
        RP_GET_OBJECT($1_name_obj, $1_name, T_ADDIN);
%}
%typemap(rp_tm_xll_cnvt) boost::shared_ptr<T_LIB> const & %{
        RP_GET_REFERENCE($1_name_obj, $1_name,
            T_ADDIN ,  T_LIB );
%}
%typemap(rp_tm_xll_argf) boost::shared_ptr<T_LIB> const & "$1_name_obj";
%typemap(rp_tm_xll_cdrt) boost::shared_ptr<T_LIB> const & "C";
%typemap(rp_tm_xll_code) boost::shared_ptr<T_LIB> const & "C";
%typemap(rp_tm_cfy_parm) boost::shared_ptr<T_LIB> const & "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) boost::shared_ptr<T_LIB> const & %{
        RP_GET_REFERENCE($1_name_get, $1_name, T_ADDIN, T_LIB);
%}
%typemap(rp_tm_cfy_args) boost::shared_ptr<T_LIB> const & "$1_name_get";

%typemap(rp_tm_dox_rtdc) boost::shared_ptr<T_LIB> const & "string returnValue";
%typemap(rp_tm_dox_mbvr) boost::shared_ptr<T_LIB> const &  "string $1_name";
%typemap(rp_tm_dox_mbvr) boost::shared_ptr<T_ADDIN> const &  "string $1_name";

%typemap(rp_tm_dox_mbvr) std::vector<boost::shared_ptr<T_LIB> > const & "vector<string> $1_name";
%typemap(rp_tm_dox_mbvr) std::vector<boost::shared_ptr<T_ADDIN> > const & "vector<string> $1_name";

%typemap(rp_tm_lib_loop) T_LIB "boost::shared_ptr<T_LIB>";
%typemap(rp_tm_lib_loop) T_ADDIN "boost::shared_ptr<T_ADDIN>";

%enddef
