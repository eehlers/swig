
//*****************************************************************************
// defines
//*****************************************************************************

#define %override %pragma(reposit) override_obj="true"
#define %group(x)  %pragma(reposit) group="x"
#define %loop(func, parm) %feature("rp:loopParameter", "parm") func
#define %processor(cls, proc) %feature("rp:processorName", "proc") cls
#define %generate(lang, id) %feature("rp:generate:lang") id
#define %generateall(lang) %feature("rp:generate:lang")

//*****************************************************************************
// rp_tm_xxx_* - typemaps shared by multiple addins
//*****************************************************************************

// rp_tm_default - not a real typemap, implemented in code.  Always returns the type.

// rp_tm_xxx_rp_get:  Retrieve an object from the repository.
%typemap(rp_tm_xxx_rp_get) SWIGTYPE %{
        RP_GET_OBJECT(xxx, objectID, $1_type);
%}

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
%typemap(rp_tm_vob_parm) int "long";
%typemap(rp_tm_vob_parm) long "long";
%typemap(rp_tm_vob_parm) double "double";
%typemap(rp_tm_vob_parm) rp_tp_string "const std::string &";
%typemap(rp_tm_vob_parm) rp_tp_any "const reposit::property_t&";
%typemap(rp_tm_vob_parm) std::vector<int> const & "const std::vector<long>&";
%typemap(rp_tm_vob_parm) std::vector<long> const & "const std::vector<long>&";
%typemap(rp_tm_vob_parm) std::vector<double> const & "const std::vector<double>&";
%typemap(rp_tm_vob_parm) SWIGTYPE "$1_type";

// HPP

// rp_tm_vob_mbvr - Value Object class member variables (C)
%typemap(rp_tm_vob_mbvr) int "long $1_name_";
%typemap(rp_tm_vob_mbvr) long "long $1_name_";
%typemap(rp_tm_vob_mbvr) double "double $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_string "std::string $1_name_";
%typemap(rp_tm_vob_mbvr) rp_tp_any "reposit::property_t $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<bool> const & "std::vector<bool> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<int> const & "std::vector<long> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<long> const & "std::vector<long> $1_name_";
%typemap(rp_tm_vob_mbvr) std::vector<double> const & "std::vector<double> $1_name_";
%typemap(rp_tm_vob_mbvr) SWIGTYPE "$1_type $1_name_";

// rp_tm_vob_srmv - code to serialize a Value Object member variable (C)
%typemap(rp_tm_vob_srmv) SWIGTYPE "& boost::serialization::make_nvp(\"$1_name\", $1_name_)";

// CPP

// rp_tm_vob_name - Insert the parameter name into the vo property names array (C)
%typemap(rp_tm_vob_name) SWIGTYPE "\"$1_name\"";

// rp_tm_vob_init - vo ctor initializer list (C)
%typemap(rp_tm_vob_init) SWIGTYPE "$1_name_($1_name)";

// rp_tm_vob_cnvt - in ValueObject::setSystemProperty, convert value from property_t to native type (C)
%typemap(rp_tm_vob_cnvt) int "reposit::convert2<long>(value)";
%typemap(rp_tm_vob_cnvt) long "reposit::convert2<long>(value)";
%typemap(rp_tm_vob_cnvt) double "reposit::convert2<double>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_string "reposit::convert2<std::string>(value)";
%typemap(rp_tm_vob_cnvt) rp_tp_any "value";
%typemap(rp_tm_vob_cnvt) std::vector<bool> const & "reposit::vector::convert2<bool>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<int> const & "reposit::vector::convert2<long>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<long> const & "reposit::vector::convert2<long>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) std::vector<double> const & "reposit::vector::convert2<double>(value, nameUpper)";
%typemap(rp_tm_vob_cnvt) SWIGTYPE "reposit::convert2<$1_type>(value)";

//*****************************************************************************
// rp_tm_scr_* - typemaps for Serialization - Create
//*****************************************************************************

// GROUP

// CPP

// rp_tm_scr_cnvt - convert types from the Value Object to the corresponding Library Object (C)
%typemap(rp_tm_scr_cnvt) int %{
    long $1_name =
        reposit::convert2<long>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) long %{
    long $1_name =
        reposit::convert2<long>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) double %{
    double $1_name =
        reposit::convert2<double>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) std::string const & %{
    std::string $1_name =
        reposit::convert2<std::string>(
            valueObject->getProperty("$1_name"));
%}

%typemap(rp_tm_scr_cnvt) const std::vector<bool>& %{
   std::vector<bool> $1_name =
        reposit::vector::convert2<bool>(valueObject->getProperty("$1_name"), "$1_name");
%}

%typemap(rp_tm_scr_cnvt) SWIGTYPE %{
    $1_type $1_name =
        reposit::convert2<$1_type>(
            valueObject->getProperty("$1_name"));
%}

//*****************************************************************************
// rp_tm_cpp_* - typemaps for the C++ Addin
//*****************************************************************************

// GROUP

// XPP

// rp_tm_cpp_rttp - Function return type (F)
%typemap(rp_tm_cpp_rttp) void "bool";
%typemap(rp_tm_cpp_rttp) double "double";
%typemap(rp_tm_cpp_rttp) SWIGTYPE "$1_type";

// rp_tm_cpp_parm - Function input parameter (F/C/M)
%typemap(rp_tm_cpp_parm) double "double";
%typemap(rp_tm_cpp_parm) double const & "const double &";
%typemap(rp_tm_cpp_parm) rp_tp_string "const std::string&";
%typemap(rp_tm_cpp_parm) rp_tp_cnv "const reposit::property_t&";
%typemap(rp_tm_cpp_parm) rp_tp_crc "const reposit::property_t&";
%typemap(rp_tm_cpp_parm) SWIGTYPE "$1_type";

// rp_tm_cpp_rtmb - return type of a member function (M)
%typemap(rp_tm_cpp_rtmb) void "bool";
%typemap(rp_tm_cpp_rtmb) double "double";
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

// rp_tm_cpp_rtdc - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_cpp_rtdc) void "";
%typemap(rp_tm_cpp_rtdc) double "double returnValue =";
%typemap(rp_tm_cpp_rtdc) SWIGTYPE "return";

// rp_tm_cpp_rtst - return statement (F/M)
%typemap(rp_tm_cpp_rtst) void "return true;";
%typemap(rp_tm_cpp_rtst) double "return returnValue;";
%typemap(rp_tm_cpp_rtst) SWIGTYPE "";

// rp_tm_cpp_args - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_cpp_args) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_cpp_args) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_cpp_args) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_csh_* - typemaps for the C# Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_csh_rttp - Function return type (F)
%typemap(rp_tm_csh_rttp) void "void";
%typemap(rp_tm_csh_rttp) double "double";
%typemap(rp_tm_csh_rttp) rp_tp_string "char *";
%typemap(rp_tm_csh_rttp) std::vector<double> "double *";

// rp_tm_csh_parm - Function input parameter (F/C/M)
%typemap(rp_tm_csh_parm) bool "bool";
%typemap(rp_tm_csh_parm) long "long";
%typemap(rp_tm_csh_parm) double "double";
%typemap(rp_tm_csh_parm) rp_tp_string "const char *";
%typemap(rp_tm_csh_parm) reposit::property_t const & "long";
%typemap(rp_tm_csh_parm) std::vector const & "long $1_name_length, char const* const*";

// rp_tm_csh_rtmb - return type of a member function (M)
//%typemap(rp_tm_csh_rtmb) void "void";
//%typemap(rp_tm_csh_rtmb) SWIGTYPE "$1_type";

%typemap(rp_tm_csh_cnvt) rp_tp_cnv %{
    $1_type $1_name_cnv =
        reposit::convert2<$1_type, reposit::property_t>($1_name);
%}

%typemap(rp_tm_csh_cnvt) rp_tp_crc %{
        $1_type $1_name_crc = reposit::CoerceImpl<
            reposit::property_t,
            $1_type>()($1_name);
%}

// rp_tm_csh_rtdc - declare variable to capture return value of Library function (F/M)
%typemap(rp_tm_csh_rtdc) void "";
%typemap(rp_tm_csh_rtdc) double "double returnValue =";
%typemap(rp_tm_csh_rtdc) rp_tp_string "std::string returnValue =";
%typemap(rp_tm_csh_rtdc) SWIGTYPE "$1_type returnValue =";

// rp_tm_csh_argfv - arguments to the ValueObject constructor
%typemap(rp_tm_csh_argfv) SWIGTYPE "$1_name";
%typemap(rp_tm_csh_argfv) std::vector const & "$1_name_vec";

// rp_tm_csh_args - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_csh_args) SWIGTYPE "$1_name";
%typemap(rp_tm_csh_args) std::vector const & "$1_name_vec2";

// rp_tm_csh_rtst - return statement (F/M)
%typemap(rp_tm_csh_rtst) rp_tp_string %{
        ULONG size = returnValue.length() + sizeof(char);
        char *ret = (char*)::CoTaskMemAlloc(size);
        strcpy(ret, returnValue.c_str());
        return ret;
%}
%typemap(rp_tm_csh_rtst) double "return returnValue;";
%typemap(rp_tm_csh_rtst) SWIGTYPE "";

%typemap(rp_tm_csh_rtst) std::vector<double> %{
        double *ret = new double[returnValue.size()+1];
        ret[0]=returnValue.size();
        for (unsigned int i=0;i<returnValue.size();i++)
            ret[i+1]=returnValue[i];
        return ret;
%}

// CS

// rp_tm_csh_rtcp
%typemap(rp_tm_csh_rtcp) rp_tp_string "[return: MarshalAs(UnmanagedType.LPStr)]";
%typemap(rp_tm_csh_rtcp) SWIGTYPE "";

//rp_tm_csh_clcp - declare the type in the Export.cs file
%typemap(rp_tm_csh_clcp) bool "bool";
%typemap(rp_tm_csh_clcp) long "int";
%typemap(rp_tm_csh_clcp) double "double";
%typemap(rp_tm_csh_clcp) rp_tp_string "StringBuilder";
%typemap(rp_tm_csh_clcp) reposit::property_t const & "int";
%typemap(rp_tm_csh_clcp) std::vector const & "int $1_name_length, [MarshalAs(UnmanagedType.LPArray, ArraySubType=UnmanagedType.LPStr)] string[]";

//rp_tm_csh_rscp
%typemap(rp_tm_csh_rscp) void "void";
%typemap(rp_tm_csh_rscp) long "int";
%typemap(rp_tm_csh_rscp) double "double";
%typemap(rp_tm_csh_rscp) rp_tp_string "string";
%typemap(rp_tm_csh_rscp) std::vector<double> "IntPtr";

//rp_tm_csh_rtex - value to be returned in the event of an exception
%typemap(rp_tm_csh_rtex) void "return;";
%typemap(rp_tm_csh_rtex) SWIGTYPE "return 0;";

// rp_tm_csh_loop - arguments to boost::bind object for a looping function (F/M)
%typemap(rp_tm_csh_loop) std::vector "_1";
%typemap(rp_tm_csh_loop) SWIGTYPE "$1_name";

//*****************************************************************************
// rp_tm_xll_* - typemaps for the Excel Addin
//*****************************************************************************

// GROUP

// CPP

// rp_tm_xll_rtft - function return type (F/M)
%typemap(rp_tm_xll_rtft) void "bool*";
%typemap(rp_tm_xll_rtft) bool "bool*";
%typemap(rp_tm_xll_rtft) int "long*";
%typemap(rp_tm_xll_rtft) long "long*";
%typemap(rp_tm_xll_rtft) double "double*";
%typemap(rp_tm_xll_rtft) rp_tp_string "char*";
%typemap(rp_tm_xll_rtft) std::vector "OPER*";

// rp_tm_xll_parm - function parameters (F/C/M)
%typemap(rp_tm_xll_parm) bool "bool*";
%typemap(rp_tm_xll_parm) int "long*";
%typemap(rp_tm_xll_parm) long "long*";
%typemap(rp_tm_xll_parm) double "double*";
%typemap(rp_tm_xll_parm) double const & "double*";
%typemap(rp_tm_xll_parm) rp_tp_string "char*";
%typemap(rp_tm_xll_parm) rp_tp_string * "char*";
%typemap(rp_tm_xll_parm) rp_tp_string & "char*";
%typemap(rp_tm_xll_parm) rp_tp_cnv "OPER*";
%typemap(rp_tm_xll_parm) rp_tp_crc "OPER*";
%typemap(rp_tm_xll_parm) rp_tp_any "OPER*";
%typemap(rp_tm_xll_parm) std::vector "OPER *";
%typemap(rp_tm_xll_parm) std::vector const & "OPER *";

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

%typemap(rp_tm_xll_cnvt) std::vector<bool> const & %{
        std::vector<bool> $1_name_vec =
            reposit::operToVector<bool>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnvt) std::vector<long> const & %{
        std::vector<long> $1_name_vec =
            reposit::operToVector<long>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnvt) std::vector<double> const & %{
        std::vector<double> $1_name_vec =
            reposit::operToVector<double>(*$1_name, "$1_name");
%}

%typemap(rp_tm_xll_cnvt) std::vector<std::string> %{
        std::vector< std::string > $1_name_vec =
            reposit::operToVector< std::string >(*$1_name, "$1_name");
%}

// rp_tm_xll_argfv - arguments to the Value Object constructor (C)
%typemap(rp_tm_xll_argfv) bool "*$1_name";
%typemap(rp_tm_xll_argfv) int "*$1_name";
%typemap(rp_tm_xll_argfv) long "*$1_name";
%typemap(rp_tm_xll_argfv) double "*$1_name";
%typemap(rp_tm_xll_argfv) std::vector const & "$1_name_vec";
%typemap(rp_tm_xll_argfv) SWIGTYPE "$1_name";

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)
%typemap(rp_tm_xll_argf) bool "*$1_name";
%typemap(rp_tm_xll_argf) int "*$1_name";
%typemap(rp_tm_xll_argf) long "*$1_name";
%typemap(rp_tm_xll_argf) double "*$1_name";
%typemap(rp_tm_xll_argf) double const & "*$1_name";
%typemap(rp_tm_xll_argf) rp_tp_cnv "$1_name_cnv";
%typemap(rp_tm_xll_argf) rp_tp_crc "$1_name_crc";
%typemap(rp_tm_xll_argf) std::vector<bool> "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector<bool> const & "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector<long> "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector<long> const & "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector<double> "$1_name_vec";
%typemap(rp_tm_xll_argf) std::vector<double> const & "$1_name_vec";
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

// rp_tm_xll_cdrt - code to register the return type with Excel
%typemap(rp_tm_xll_cdrt) void "L";
%typemap(rp_tm_xll_cdrt) bool "L";
%typemap(rp_tm_xll_cdrt) int "N";
%typemap(rp_tm_xll_cdrt) long "N";
%typemap(rp_tm_xll_cdrt) double "E";
%typemap(rp_tm_xll_cdrt) double const & "E";
%typemap(rp_tm_xll_cdrt) rp_tp_string "C";
%typemap(rp_tm_xll_cdrt) rp_tp_string * "C";
%typemap(rp_tm_xll_cdrt) rp_tp_string & "C";
%typemap(rp_tm_xll_cdrt) rp_tp_string const & "C";
%typemap(rp_tm_xll_cdrt) rp_tp_cnv "P";
%typemap(rp_tm_xll_cdrt) rp_tp_crc "P";
%typemap(rp_tm_xll_cdrt) rp_tp_any "P";
%typemap(rp_tm_xll_cdrt) std::vector "P";
%typemap(rp_tm_xll_cdrt) std::vector const & "P";
%typemap(rp_tm_xll_cdrt) SWIGTYPE * "C";

// rp_tm_xll_code - code to register the parameter with Excel
%typemap(rp_tm_xll_code) void "L";
%typemap(rp_tm_xll_code) bool "L";
%typemap(rp_tm_xll_code) int "N";
%typemap(rp_tm_xll_code) long "N";
%typemap(rp_tm_xll_code) double "E";
%typemap(rp_tm_xll_code) double const & "E";
%typemap(rp_tm_xll_code) rp_tp_string "C";
%typemap(rp_tm_xll_code) rp_tp_string * "C";
%typemap(rp_tm_xll_code) rp_tp_string & "C";
%typemap(rp_tm_xll_code) rp_tp_string const & "C";
%typemap(rp_tm_xll_code) rp_tp_cnv "P";
%typemap(rp_tm_xll_code) rp_tp_crc "P";
%typemap(rp_tm_xll_code) rp_tp_any "P";
%typemap(rp_tm_xll_code) std::vector "P";
%typemap(rp_tm_xll_code) std::vector const & "P";
%typemap(rp_tm_xll_code) SWIGTYPE * "C";

// rp_tm_xll_loop - arguments to boost::bind object for a looping function (F/M)
%typemap(rp_tm_xll_loop) bool "*$1_name";
%typemap(rp_tm_xll_loop) std::vector "_1";
%typemap(rp_tm_xll_loop) SWIGTYPE "$1_name";

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
%typemap(rp_tm_cfy_rtmb) double "double";
%typemap(rp_tm_cfy_rtmb) SWIGTYPE "$1_type";

// rp_tm_cfy_parm - function parameters (F/C/M)
%typemap(rp_tm_cfy_parm) double "double $1_name";
%typemap(rp_tm_cfy_parm) rp_tp_string "const char * $1_name";
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a dummy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_parm)  reposit::property_t const & "bool $1_name";
%typemap(rp_tm_cfy_parm) SWIGTYPE "$1_type $1_name";

// rp_tm_cfy_cnvt - convert input values from =countify datatypes into datatypes of the underlying Library (F/C/M)

// rp_tm_cfy_rtdf - declare variable to capture return value of Library function (F)
%typemap(rp_tm_cfy_rtdf) std::string %{
        static std::string ret;
        ret =
%}
%typemap(rp_tm_cfy_rtdf) long %{
        long returnValue =
%}

// rp_tm_cfy_rtdm - declare variable to capture return value of Library member function (M)
%typemap(rp_tm_cfy_rtdm) void "";
%typemap(rp_tm_cfy_rtdm) double "double returnValue =";

// rp_tm_cfy_args - arguments to pass to underlying Library function
%typemap(rp_tm_cfy_args) SWIGTYPE "$1_name";

// rp_tm_cfy_rtsf - return statement of a Library function (F)
%typemap(rp_tm_cfy_rtsf) void "return true;";
%typemap(rp_tm_cfy_rtsf) long "return returnValue;";
%typemap(rp_tm_cfy_rtsf) std::string "return ret.c_str();";

// rp_tm_cfy_rtsm - return statement of a Library member function (M)
%typemap(rp_tm_cfy_rtsm) double "return returnValue;";
%typemap(rp_tm_cfy_rtsm) void "return true;";

// rp_tm_cfy_rtex - return statement in the event of an exception
%typemap(rp_tm_cfy_rtex) SWIGTYPE "return 0;";

// TXT

// rp_tm_cfy_mngo - generate the json to register functions with the mongo database
// This is a temporary hack for trigger parameters, which are not supported by =countify.
// Declare the value as a summy bool.  Later we will suppress it altogether.
%typemap(rp_tm_cfy_mngo) void "BOOL";
%typemap(rp_tm_cfy_mngo) bool "BOOL";
%typemap(rp_tm_cfy_mngo) long "INT64";
%typemap(rp_tm_cfy_mngo) double "REAL64";
%typemap(rp_tm_cfy_mngo) reposit::property_t const & "BOOL";
%typemap(rp_tm_cfy_mngo) SWIGTYPE "CSTR";

//*****************************************************************************
// %apply statements - map native types to reposit types
//*****************************************************************************

%apply long { std::size_t };

%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_string { std::string const & };

%apply rp_tp_any { const reposit::property_t& };

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
%typemap(rp_tm_csh_loop) T "$1_name_enm";
%typemap(rp_tm_vob_parm) T "const std::string &";
%typemap(rp_tm_vob_mbvr) T "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T "reposit::convert2<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
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
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";
%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
        T $1_name_enum =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enum";

// T &
%typemap(rp_tm_xxx_rp_get) T & %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_xll_loop) T & "$1_name_enm";
%typemap(rp_tm_csh_loop) T & "$1_name_enm";
%typemap(rp_tm_vob_parm) T & "const std::string &";
%typemap(rp_tm_vob_mbvr) T & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T & "reposit::convert2<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T & %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
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
%typemap(rp_tm_xll_argf) T & "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T & "C";
%typemap(rp_tm_xll_code) T & "C";
%typemap(rp_tm_cfy_parm) T & "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T & %{
        T $1_name_enum =
            reposit::Create<T>()($1_name);
%}
%typemap(rp_tm_cfy_args) T & "$1_name_enum";
%enddef

// enumerated class
%define ENUMERATED_CLASS(T...)
// T
%typemap(rp_tm_xxx_rp_get) T %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_vob_parm) T "const std::string &";
%typemap(rp_tm_vob_mbvr) T "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T "reposit::convert2<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
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
%typemap(rp_tm_xll_argf) T "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T "C";
%typemap(rp_tm_xll_code) T "C";

%typemap(rp_tm_cfy_parm) T "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cfy_args) T "$1_name_enm";

// T &
%typemap(rp_tm_xxx_rp_get) T & %{
        T xxx =
            reposit::Create<T>()(objectID);
%}
%typemap(rp_tm_vob_parm) T & "const std::string &";
%typemap(rp_tm_vob_mbvr) T & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) T & "reposit::convert2<std::string>(value)";
%typemap(rp_tm_scr_cnvt) T & %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
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
%typemap(rp_tm_xll_argf) T & "$1_name_enm";
%typemap(rp_tm_xll_cdrt) T & "C";
%typemap(rp_tm_xll_code) T & "C";

%typemap(rp_tm_cfy_parm) T & "const char * $1_name";
%typemap(rp_tm_cfy_cnvt) T & %{
    T $1_name_enm =
        reposit::Create<T >()($1_name);
%}
%typemap(rp_tm_cfy_args) T & "$1_name_enm";

%enddef

// repository objects
%define OBJECT_WRAPPER(T_ADDIN,T_LIB...)

%typemap(rp_tm_xxx_rp_get) T_LIB %{
        RP_GET_REFERENCE(xxx, objectID, T_ADDIN, T_LIB);
%}
%typemap(rp_tm_vob_parm) boost::shared_ptr<T_LIB> const & "const std::string &";
%typemap(rp_tm_vob_mbvr) boost::shared_ptr<T_LIB> const & "std::string $1_name_";
%typemap(rp_tm_vob_cnvt) boost::shared_ptr<T_LIB> const & "reposit::convert2<std::string>(value)";
%typemap(rp_tm_scr_cnvt) boost::shared_ptr<T_LIB> const & %{
    std::string $1_name_str =
        reposit::convert2<std::string>(valueObject->getProperty("$1_name"));
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
%enddef
