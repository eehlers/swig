
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

%typemap(rp_tm_xll_rtft) rp_tp_string "char*";

// rp_tm_xll_parm - function parameters (F/C/M)

%typemap(rp_tm_xll_parm) rp_tp_string "char*";
%typemap(rp_tm_xll_parm) SWIGTYPE "OPER*";

// rp_tm_xll_cnvt - convert from Excel datatypes to the datatypes of the underlying Library

// rp_tm_xll_argfv - arguments to the Value Object constructor (C)

// rp_tm_xll_argf - arguments to the underlying Library function (F/C/M)

// rp_tm_xll_rtdc - declare variable to capture return value of Library function (F/M)

%typemap(rp_tm_xll_rtdc) void "";
%typemap(rp_tm_xll_rtdc) SWIGTYPE "$1_type returnValue =";

// rp_tm_xll_rtst - return statement (F/M)

%typemap(rp_tm_xll_rtst) rp_tp_string %{
        static char ret[XL_MAX_STR_LEN];
        reposit::stringToChar(returnValue, ret);
        return ret;
%}

// rp_tm_xll_cdrt - code to register the return type with Excel

%typemap(rp_tm_xll_cdrt) rp_tp_string "C";
//%typemap(rp_tm_xll_cdrt) rp_tp_any "P";

// rp_tm_xll_code - code to register the parameter with Excel

%typemap(rp_tm_xll_code) rp_tp_any "P";

// rp_tm_xll_loop - arguments to boost::bind object for a looping function (F/M)

// Datatype of the loop function for call to boost::bind

// Datatype of the loop parameter for call to boost::bind

//*****************************************************************************
// %apply statements - map native types to reposit types
//*****************************************************************************

%apply rp_tp_string { char * };
%apply rp_tp_string { std::string };
%apply rp_tp_string { std::string & };
%apply rp_tp_any { reposit::property_t & };

//*****************************************************************************
// Macros - for certain kinds of types - repository objects, enumerated types
// and classes - you can call the macros below to define all relevant typedefs
// for the given type.
//*****************************************************************************
