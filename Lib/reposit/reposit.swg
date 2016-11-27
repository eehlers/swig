
#pragma SWIG nowarn=401,501,502

//*****************************************************************************
// defines
//*****************************************************************************

#define %group(x)  %pragma(reposit) group="x"
// do not generate the object wrapper hpp file
#define %override %pragma(reposit) override_obj="true"
// do not generate class constructor in objectwrapper hpp file
#define %noctor(class_name) %feature("rp:noctor") class_name
// do not generate class member functions in objectwrapper hpp file
#define %noexport(class_name) %feature("rp:noexport") class_name
// comment out the ctor implementation in the objectwrapper cpp file
#define %override2(f) %feature("rp:override") f
// xxx
#define %explicit_class(class_name) %feature("rp:explicit_class") class_name
#define %groupFunctionWizard(x)  %pragma(reposit) groupFunctionWizard="x"
#define %loop(func, parm) %feature("rp:loopParameter", "parm") func
#define %processor(cls, proc) %feature("rp:processorName", "proc") cls
#define %generate(lang, id) %feature("rp:generate:lang") id
#define %generateall(lang) %feature("rp:generate:lang")
#define %alias(id, id2) %feature("rp:alias", "id2") id
#define %rename2(id, id2) %feature("rp:rename2", "id2") id

//*****************************************************************************
// pragmas
//*****************************************************************************

%pragma(reposit) rp_obj_dir="../AddinObjects"
%pragma(reposit) rp_obj_inc="AddinObjects"
%pragma(reposit) rp_add_dir="../AddinCpp"
%pragma(reposit) rp_add_inc="AddinCpp"
%pragma(reposit) rp_xll_dir="../AddinXl"
%pragma(reposit) rp_xll_inc="AddinXl"

//*****************************************************************************
// rp_tm_xxx_* - typemaps shared by multiple addins
//*****************************************************************************

// Not a real typemap, implemented in code.  Always returns the type.
// rp_tm_default

// rp_tm_xxx_rp_get:  Retrieve an object from the repository.
%typemap(rp_tm_xxx_rp_get) SWIGTYPE %{
        RP_GET_OBJECT(xxx, objectID, $1_type);
%}

%typemap(rp_tm_xxx_rp_call) SWIGTYPE "xxx->";

//*****************************************************************************
// rp_tm_lib_* - typemaps for Library Objects
//*****************************************************************************

// GROUP

// HPP

// Not a real typemap. Defined in code.  RP_OBJ_CLASS (C)
// rp_tm_lib_cls

// CPP

// rp_tm_lib_rtst - Return statement (F)

%typemap(rp_tm_lib_loop) SWIGTYPE "$1_type";

//*****************************************************************************
// rp_tm_vob_* - typemaps for Value Objects
//*****************************************************************************

// GROUP

// XPP

// rp_tm_vob_parm - Value Object constructor parameters (C)

%typemap(rp_tm_vob_parm) bool "bool";
%typemap(rp_tm_vob_parm) rp_tp_long "long";
%typemap(rp_tm_vob_parm) double "double";
%typemap(rp_tm_vob_parm) double& "double";
%typemap(rp_tm_vob_parm) rp_tp_string "const std::string &";
%typemap(rp_tm_vob_parm) rp_tp_any "const reposit::property_t&";
%typemap(rp_tm_vob_parm) std::vector< std::vector<double> > const & "const std::vector< std::vector<double> >&";
%typemap(rp_tm_vob_parm) SWIGTYPE "$1_type";

// HPP

// rp_tm_vob_mbvr - Value Object class member variables (C)

%typemap(rp_tm_vob_mbvr) bool "bool $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_long "long $1_name_";
%typemap(rp_tm_vob_mbvr) double "double $1_name_";
%typemap(rp_tm_vob_mbvr) double& "double $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_string "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_any "reposit::property_t $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector< std::vector<double> > const & "std::vector< std::vector<double> > $1_name_";
%typemap(rp_tm_vob_mbvr) SWIGTYPE "$1_type $1_name_";

// rp_tm_vob_srmv - code to serialize a Value Object member variable (C)
%typemap(rp_tm_vob_srmv) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

// CPP

// rp_tm_vob_name - Insert the parameter name into the vo property names array (C)
%typemap(rp_tm_vob_name) SWIGTYPE "\"$1_name\"";

// rp_tm_vob_init - vo ctor initializer list (C)
%typemap(rp_tm_vob_init) SWIGTYPE "$1_name_($1_name)";

// rp_tm_vob_cnvt - in ValueObject::setSystemProperty, convert value from property_t to native type (C)

%typemap(rp_tm_vob_cnvt) rp_tp_long "reposit::convert<long>(value)";
%typemap(rp_tm_vob_cnvt) double "reposit::convert<double>(value)";
%typemap(rp_tm_vob_cnvt) double& "reposit::convert<double>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_string "reposit::convert<std::string>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_any "value";
%typemap(rp_tm_vob_cnvt) std::vector<std::vector<double> > & "reposit::matrix::convert<double>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) SWIGTYPE "reposit::convert<$1_type>(value)";

//*****************************************************************************
// rp_tm_scr_* - typemaps for Serialization - Create
//*****************************************************************************

// GROUP

// CPP

// rp_tm_scr_cnvt - convert types from the Value Object to the corresponding Library Object (C)

%typemap(rp_tm_scr_cnvt) rp_tp_long %{
    long $1_name =
        reposit::convert<long>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) double %{
    double $1_name =
        reposit::convert<double>(
            valueObject->getProperty("$1_name"));
%}
%typemap(rp_tm_scr_cnvt) double& = double;

%typemap(rp_tm_scr_cnvt) rp_tp_string %{
    std::string $1_name =
        reposit::convert<std::string>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) SWIGTYPE %{
    $1_type $1_name =
        reposit::convert<$1_type>(
            valueObject->getProperty("$1_name"));
%}

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

%typemap(rp_tm_xll_cnvt2) rp_tp_long %{
        long $1_name_cnv = reposit::convert<long>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_value);
%}

%typemap(rp_tm_xll_cnvt2) double %{
        double $1_name_cnv = reposit::convert<double>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_value);
%}
%typemap(rp_tm_xll_cnvt2) double& = double;

%typemap(rp_tm_xll_cnvt2) std::string & %{
        std::string $1_name_cnv = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", $rp_value);
%}

// rp_tm_xll_argv - arguments to the Value Object constructor (C)

%typemap(rp_tm_xll_argv) bool "*$1_name";
%typemap(rp_tm_xll_argv) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argv) double "*$1_name";
%typemap(rp_tm_xll_argv) double & "*$1_name";
%typemap(rp_tm_xll_argv) std::vector "$1_name_vec";
%typemap(rp_tm_xll_argv) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argv) SWIGTYPE "$1_name";

%typemap(rp_tm_xll_argv2) std::vector "$1_name_vec";
%typemap(rp_tm_xll_argv2) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argv2) SWIGTYPE "$1_name_cnv";

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)

// NB We would like to do this:
//%typemap(rp_tm_xll_argf) std::vector "$1_name_vec";
// But the above needs to be overridden for some types.
// Because of the way SWIG's type matching rules interact with typedefs and templates,
// the above typemap cannot always be overridden as necessary.
// Therefore we need explicit typemaps for rp_tm_xll_argf/rp_tm_xll_argf2
// for every T in vector<T> and vector<T>&.

%typemap(rp_tm_xll_argf) bool "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_long "*$1_name";
%typemap(rp_tm_xll_argf) double "*$1_name";
%typemap(rp_tm_xll_argf) double & "*$1_name";
//%typemap(rp_tm_xll_argf) std::vector "$1_name_vec";
//%typemap(rp_tm_xll_argf) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argf) SWIGTYPE "$1_name";

//%typemap(rp_tm_xll_argf2) std::vector "$1_name_vec";
//%typemap(rp_tm_xll_argf2) std::vector & "$1_name_vec";
%typemap(rp_tm_xll_argf2) SWIGTYPE "$1_name_cnv";

// rp_tm_xll_rtdc - declare variable to capture return value of Library function (F/M)

%typemap(rp_tm_xll_rtdc) void "";
%typemap(rp_tm_xll_rtdc) SWIGTYPE "$1_type returnValue =";

// rp_tm_xll_rtst - return statement (F/M)

%typemap(rp_tm_xll_rtst) void %{
        static bool ret = true;
        return &ret;
%}
// FIXME replace the 3 below with SWIGTYPE
%typemap(rp_tm_xll_rtst) bool %{
        static bool ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) rp_tp_long %{
        static long ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) double %{
        static double ret;
        ret = returnValue;
        return &ret;
%}

%typemap(rp_tm_xll_rtst) rp_tp_string %{
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(returnValue, ret);
        return ret;
%}

%typemap(rp_tm_xll_rtst) std::vector<std::vector<double> > %{
        static OPER xRet;
        reposit::matrixToOper(returnValue, xRet);
        return &xRet;
%}

%typemap(rp_tm_xll_rtst) std::vector<std::vector<reposit::property_t> > %{
        static OPER xRet;
        reposit::matrixToOper(returnValue, xRet);
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
%typemap(rp_tm_xll_loop) double "*$1_name";
%typemap(rp_tm_xll_loop) std::vector "_1";
%typemap(rp_tm_xll_loop) SWIGTYPE "$1_name";

//%typemap(rp_tm_xll_loop2) bool "*$1_name";
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
%apply rp_tp_long { unsigned int };
%apply rp_tp_long { std::size_t };
%apply rp_tp_long { long };
%apply rp_tp_long { long & };
%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_any { reposit::property_t & };

//*****************************************************************************
// vectors
//*****************************************************************************

// use this macro for typemaps which do the same thing for every T in vector<T>.

%define RP_VECTOR_IMPL(T,VEC_T...)

%typemap(rp_tm_xll_argf) VEC_T "$1_name_vec";
%typemap(rp_tm_xll_argf2) VEC_T "$1_name_vec";

// rp_tm_vob_parm - Value Object constructor parameters (C)

%typemap(rp_tm_vob_parm) VEC_T "const std::vector<T>&";

// rp_tm_vob_mbvr - Value Object class member variables (C)

%typemap(rp_tm_vob_mbvr) VEC_T "std::vector<T> $1_name_";

// rp_tm_vob_cnvt - in ValueObject::setSystemProperty, convert value from property_t to native type (C)

%typemap(rp_tm_vob_cnvt) VEC_T "reposit::vector::convert<T>(value, nameUpper)";

// rp_tm_scr_cnvt - convert types from the Value Object to the corresponding Library Object (C)

%typemap(rp_tm_scr_cnvt) VEC_T %{
   std::vector<T> $1_name =
        reposit::vector::convert<T>(valueObject->getProperty("$1_name"), "$1_name");
%}

// rp_tm_xll_cnvt - convert from Excel datatypes to the datatypes of the underlying Library

%typemap(rp_tm_xll_cnvt) VEC_T %{
        std::vector<T> $1_name_vec =
            reposit::operToVector<T>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnvt2) VEC_T %{
        std::vector<T> $1_name_vec =
            reposit::operToVector<T>(*$1_name, "$1_name");
%}

// rp_tm_xll_rtst - return statement (F/M)

%typemap(rp_tm_xll_rtst) VEC_T %{
        static OPER xRet;
        reposit::vectorToOper(returnValue, xRet);
        return &xRet;
%}

%enddef

%define RP_VECTOR(T...)
RP_VECTOR_IMPL(T,std::vector<T>)
RP_VECTOR_IMPL(T,std::vector<T>&)
%enddef

%define RP_VECTOR_LONG_IMPL(T,VEC_T...)
%typemap(rp_tm_vob_parm) VEC_T "const std::vector<long>&";
%typemap(rp_tm_vob_mbvr) VEC_T "std::vector<long> $1_name_";
%typemap(rp_tm_vob_cnvt) VEC_T "reposit::vector::convert<long>(value, nameUpper)";
%typemap(rp_tm_xll_cnvt) VEC_T %{
        std::vector<long> $1_name_vec =
            reposit::operToVector<long>(*$1_name, "$1_name");
        std::vector<T> $1_name_vec2 =
            QuantLibAddin::convertVector<long, T>($1_name_vec);
%}
%typemap(rp_tm_xll_argf) VEC_T "$1_name_vec2";
%typemap(rp_tm_xll_argf2) VEC_T "$1_name_vec2";
%enddef

%define RP_VECTOR_LONG(T...)
RP_VECTOR(T)
RP_VECTOR_LONG_IMPL(T,std::vector<T>)
RP_VECTOR_LONG_IMPL(T,std::vector<T>&)
%enddef

RP_VECTOR(bool)
RP_VECTOR(long)
RP_VECTOR(double)
RP_VECTOR(std::string)

// Use RP_VECTOR_LONG instead of RP_VECTOR for integer types other than long which should be treated like long.
// RP_VECTOR_LONG includes some other steps to convert T to long which are not necessary when T is long.
RP_VECTOR_LONG(unsigned int)

//*****************************************************************************
// Macros - for certain kinds of types - repository objects, enumerated types
// and classes - you can call the macros below to define all relevant typedefs
// for the given type.
//*****************************************************************************

// enumerated types
%define ENUMERATED_TYPE_IMPL2(T...)

%typemap(rp_tm_xxx_rp_get) T %{
        $1_basetype xxx =
            reposit::Create<$1_basetype>()(objectID);
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
    $1_basetype $1_name =
        reposit::Create<$1_basetype>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T "const std::string&";
%typemap(rp_tm_cpp_cnvt) T %{
    $1_basetype $1_name_enm =
        reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_cpp_args) T "$1_name_enm";
%typemap(rp_tm_csh_parm) T "const char *";
%typemap(rp_tm_csh_cnvt) T %{
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_csh_args) T "$1_name_enm";
%typemap(rp_tm_csh_clcp) T "StringBuilder";
%typemap(rp_tm_xll_rtft) T "char*";
%typemap(rp_tm_xll_parm) T "char*";
%typemap(rp_tm_xll_cnvt) T %{
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_argf2) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";

%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
        $1_basetype $1_name_enum =
            reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enum";
%typemap(rp_tm_xll_argv) T "$1_name";
%typemap(rp_tm_xll_argv2) T "$1_name_str";

%typemap(rp_tm_xll_rtst) T %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_dox_rtdc) T "string returnValue";
%typemap(rp_tm_dox_mbvr) T "string $1_name";

%enddef

%define ENUMERATED_TYPE_IMPL(T...)
ENUMERATED_TYPE_IMPL2(T)
ENUMERATED_TYPE_IMPL2(T&)
%enddef

%define ENUMERATED_TYPE(N,T...)
namespace N {
    enum T;
}
ENUMERATED_TYPE_IMPL(N::T)
%enddef

%define ENUMERATED_TYPE_STRUCT(N,S,T...)
namespace N {
    struct S {
        enum T;
    };
}
ENUMERATED_TYPE_IMPL(N::S::T)
%enddef

// enumerated class
%define ENUMERATED_CLASS_IMPL2(T,T_NAME...)

%typemap(rp_tm_xxx_rp_get) T %{
        $1_basetype xxx =
            reposit::Create<$1_basetype>()(objectID);
%}
%typemap(rp_tm_xxx_rp_call) T "xxx.";
%typemap(rp_tm_xll_loop) T "$1_name_enm";
%typemap(rp_tm_xll_loop2) T "$1_name_enm";
%typemap(rp_tm_vob_parm) T "const std::string &";
%typemap(rp_tm_vob_mbvr) T "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T "reposit::convert<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    $1_basetype $1_name =
        reposit::Create<$1_basetype>()($1_name_str);
%}
%typemap(rp_tm_cpp_parm) T "const std::string&";
%typemap(rp_tm_cpp_cnvt) T %{
    $1_basetype $1_name_enm =
        reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_cpp_args) T "$1_name_enm";

%typemap(rp_tm_csh_parm) T "const char *";
%typemap(rp_tm_csh_cnvt) T %{
            $1_basetype $1_name_enm =
                reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_csh_cnvt) T %{
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_csh_args) T "$1_name_enm";
%typemap(rp_tm_csh_clcp) T "StringBuilder";
%typemap(rp_tm_csh_rttp) T "char *";

%typemap(rp_tm_xll_rtft) T "char*";
%typemap(rp_tm_xll_parm) T "char*";
%typemap(rp_tm_xll_cnvt) T %{
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_xll_cnvt2) T %{
        std::string $1_name_str = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "$rp_value");
        $1_basetype $1_name_enm =
            reposit::Create<$1_basetype>()($1_name_str);
%}
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_argf2) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";

%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
    $1_basetype $1_name_enm =
        reposit::Create<$1_basetype>()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enm";
%typemap(rp_tm_xll_argv) T "$1_name";
%typemap(rp_tm_xll_argv2) T "$1_name_str";

%typemap(rp_tm_xll_rtst) T %{
        std::ostringstream os;
        os << returnValue;
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(os.str(), ret);
        return ret;
%}

%typemap(rp_tm_dox_rtdc) T "string returnValue";
%typemap(rp_tm_dox_mbvr) T "string $1_name";
//%typemap(rp_tm_dox_rtd2) SWIGTYPE "ID of Enumeration of class T_NAME";

%enddef

%define ENUMERATED_CLASS_IMPL(T...)
ENUMERATED_CLASS_IMPL2(T, T)
ENUMERATED_CLASS_IMPL2(T&, T)
%enddef

%define ENUMERATED_CLASS(N,T...)
namespace N {
    class T;
}
ENUMERATED_CLASS_IMPL(N::T)
%enddef

// repository objects
%define OBJECT_WRAPPER(T_ADDIN,T_LIB...)

%typemap(rp_tm_xxx_rp_get) T_LIB %{
        RP_GET_REFERENCE(xxx, objectID, T_ADDIN, T_LIB);
%}
%typemap(rp_tm_vob_parm) T_LIB const & "const std::string &";
%typemap(rp_tm_vob_parm) boost::shared_ptr<T_LIB> const & "const std::string &";
%typemap(rp_tm_vob_parm) boost::shared_ptr<T_ADDIN> const & "const std::string &";
%typemap(rp_tm_vob_parm) std::vector<T_LIB> const & "const std::vector<std::string> &";
%typemap(rp_tm_vob_parm) std::vector<boost::shared_ptr<T_ADDIN> > const & "const std::vector<std::string> &";
%typemap(rp_tm_vob_mbvr) T_LIB const & "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) boost::shared_ptr<T_LIB> const & "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) boost::shared_ptr<T_ADDIN> const & "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<T_LIB> const & "std::vector<std::string> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<boost::shared_ptr<T_ADDIN> > const & "std::vector<std::string> $1_name_";
%typemap(rp_tm_vob_cnvt) T_LIB const & "reposit::convert<std::string>(value)";
%typemap(rp_tm_vob_cnvt) boost::shared_ptr<T_LIB> const & "reposit::convert<std::string>(value)";
%typemap(rp_tm_vob_cnvt) boost::shared_ptr<T_ADDIN> const & "reposit::convert<std::string>(value)";
%typemap(rp_tm_vob_cnvt) std::vector<T_LIB> const & "reposit::vector::convert<std::string>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<boost::shared_ptr<T_ADDIN> > const & "reposit::vector::convert<std::string>(value, nameUpper)";

%typemap(rp_tm_scr_cnvt) T_LIB const & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_UNDERLYING($1_name, $1_name_str,
        T_ADDIN, T_LIB);
%}

%typemap(rp_tm_scr_cnvt) boost::shared_ptr<T_LIB> const & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_REFERENCE($1_name, $1_name_str,
        T_ADDIN, T_LIB);
%}
%typemap(rp_tm_scr_cnvt2) boost::shared_ptr<T_LIB> const & %{
    std::string $1_name_str =
        reposit::convert<std::string>(valueObject->getProperty("$1_name"));
    valueObject->processPrecedentID($1_name_str);
    RP_GET_REFERENCE_DEFAULT($1_name, $1_name_str,
        T_ADDIN, T_LIB);
%}

%typemap(rp_tm_scr_cnvt) std::vector<T_LIB> & %{
    std::vector<std::string> $1_name_vec =
        reposit::vector::convert<std::string>(valueObject->getProperty("$1_name"), "$1_name");
    for (std::vector<std::string>::const_iterator i = $1_name_vec.begin();
            i != $1_name_vec.end(); ++i)
        valueObject->processPrecedentID(*i);
    std::vector<boost::shared_ptr<T_LIB> > $1_name_temp =
        reposit::getLibraryObjectVector<T_ADDIN, T_LIB>($1_name_vec);
    // FIXME implement a variation of function getLibraryObjectVector() which includes the conversion below:
    std::vector<T_LIB> $1_name($1_name_temp.size());
    for (QuantLib::Size i=0; i<$1_name_temp.size(); ++i) {
        boost::shared_ptr<T_LIB> $1_name_item = $1_name_temp[i];
        $1_name[i] = *($1_name_item.get());
    }
%}

%typemap(rp_tm_scr_cnvt) std::vector<boost::shared_ptr<T_LIB> > & %{
    std::vector<std::string> $1_name_vec =
        reposit::vector::convert<std::string>(valueObject->getProperty("$1_name"), "$1_name");
    for (std::vector<std::string>::const_iterator i = $1_name_vec.begin();
            i != $1_name_vec.end(); ++i)
        valueObject->processPrecedentID(*i);
    std::vector<boost::shared_ptr<T_LIB> > $1_name =
        reposit::getLibraryObjectVector<T_ADDIN, T_LIB>($1_name_vec);
%}

%typemap(rp_tm_scr_cnvt) std::vector<boost::shared_ptr<T_ADDIN> > & %{
    std::vector<std::string> $1_name_vec =
        reposit::vector::convert<std::string>(valueObject->getProperty("$1_name"), "$1_name");
    for (std::vector<std::string>::const_iterator i = $1_name_vec.begin();
            i != $1_name_vec.end(); ++i)
        valueObject->processPrecedentID(*i);
    std::vector<boost::shared_ptr<T_ADDIN> > $1_name =
        reposit::getObjectVector<T_ADDIN>($1_name_vec);
%}

%typemap(rp_tm_cpp_parm) boost::shared_ptr<T_LIB> const & "const std::string&";
%typemap(rp_tm_cpp_args) boost::shared_ptr<T_LIB> const & "$1_name_get";

%typemap(rp_tm_xll_parm) T_LIB & "char*";
%typemap(rp_tm_xll_parm) boost::shared_ptr<T_LIB> const & "char*";

// FIXME this is currently implemented in QuantLibAddin, it should be here.
//%typemap(rp_tm_xll_cnvt) T_LIB & %{
//        boost::shared_ptr<T_LIB> $1_name_temp = reposit::GetObject<T_ADDIN, T_LIB>::f($1_name);
//        const T_LIB &$1_name_obj = *($1_name_temp.get());
//%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<T_ADDIN> const & %{
        RP_GET_OBJECT($1_name_obj, $1_name, T_ADDIN);
%}
%typemap(rp_tm_xll_cnvt) boost::shared_ptr<T_LIB> const & %{
        RP_GET_REFERENCE($1_name_obj, $1_name,
            T_ADDIN, T_LIB);
%}
%typemap(rp_tm_xll_cnvt2) boost::shared_ptr<T_LIB> const & %{
        std::string $1_name_cnv = reposit::convert<std::string>(
            reposit::ConvertOper(*$1_name), "$1_name", "");
        RP_GET_REFERENCE_DEFAULT($1_name_obj, $1_name_cnv,
            T_ADDIN, T_LIB);
%}

%typemap(rp_tm_xll_cnvt) std::vector<T_LIB> & %{
        std::vector<std::string> $1_name_vec =
            reposit::operToVector<std::string>(*$1_name, "$1_name");
        std::vector<boost::shared_ptr<T_LIB> > $1_name_temp =
            reposit::getLibraryObjectVector<T_ADDIN, T_LIB>($1_name_vec);
        // FIXME implement a variation of function getLibraryObjectVector() which includes the conversion below:
        std::vector<T_LIB> $1_name_vec2($1_name_temp.size());
        for (QuantLib::Size i=0; i<$1_name_temp.size(); ++i) {
            boost::shared_ptr<T_LIB> $1_name_item = $1_name_temp[i];
            $1_name_vec2[i] = *($1_name_item.get());
        }
%}

%typemap(rp_tm_xll_cnvt) std::vector<boost::shared_ptr<T_ADDIN> > & %{
        std::vector<std::string> $1_name_vec =
            reposit::operToVector<std::string>(*$1_name, "$1_name");
        std::vector<boost::shared_ptr<T_ADDIN> > $1_name_vec2 =
            reposit::getObjectVector<T_ADDIN>($1_name_vec);
%}

%typemap(rp_tm_xll_cnvt) std::vector<boost::shared_ptr<T_LIB> > & %{
        std::vector<std::string> $1_name_vec =
            reposit::operToVector<std::string>(*$1_name, "$1_name");
        std::vector<boost::shared_ptr<T_LIB> > $1_name_vec2 =
            reposit::getLibraryObjectVector<T_ADDIN, T_LIB>($1_name_vec);
%}

%typemap(rp_tm_xll_argf) T_LIB & "$1_name_obj";
%typemap(rp_tm_xll_argf) boost::shared_ptr<T_LIB> const & "$1_name_obj";
%typemap(rp_tm_xll_argf) std::vector<T_LIB> const & "$1_name_vec2";
%typemap(rp_tm_xll_argf) std::vector<boost::shared_ptr<T_ADDIN> > const & "$1_name_vec2";
%typemap(rp_tm_xll_argf) std::vector<boost::shared_ptr<T_LIB> > const & "$1_name_vec2";
%typemap(rp_tm_xll_argf2) boost::shared_ptr<T_LIB> const & "$1_name_obj";
%typemap(rp_tm_xll_cdrt) boost::shared_ptr<T_LIB> const & "C";
%typemap(rp_tm_xll_code) T_LIB const & "C";
%typemap(rp_tm_xll_code) boost::shared_ptr<T_LIB> const & "C";

%typemap(rp_tm_dox_rtdc) boost::shared_ptr<T_LIB> const & "string returnValue";
%typemap(rp_tm_dox_mbvr) boost::shared_ptr<T_LIB> const &  "string $1_name";
%typemap(rp_tm_dox_mbvr) boost::shared_ptr<T_ADDIN> const &  "string $1_name";

%typemap(rp_tm_dox_mbvr) std::vector<boost::shared_ptr<T_LIB> > const & "vector<string> $1_name";
%typemap(rp_tm_dox_mbvr) std::vector<boost::shared_ptr<T_ADDIN> > const & "vector<string> $1_name";

%typemap(rp_tm_lib_loop) T_LIB "boost::shared_ptr<T_LIB>";
%typemap(rp_tm_lib_loop) T_ADDIN "boost::shared_ptr<T_ADDIN>";

%enddef
