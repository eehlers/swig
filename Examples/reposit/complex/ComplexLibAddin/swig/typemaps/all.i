
%apply rp_tp_cnv { ComplexLib::Grade };
%apply rp_tp_crc { ComplexLib::Grade2 };

ENUMERATED_TYPE(ComplexLib::AccountType)
ENUMERATED_TYPE(ComplexLib::Account2::Type2)

ENUMERATED_CLASS(boost::shared_ptr<ComplexLib::TimeZone>)

OBJECT_WRAPPER(ComplexLibAddin::Test, ComplexLib::Test)
OBJECT_WRAPPER(ComplexLibAddin::Test2, ComplexLib::Test2)
OBJECT_WRAPPER(ComplexLibAddin::Base, ComplexLib::Base)
OBJECT_WRAPPER(ComplexLibAddin::Derived, ComplexLib::Derived)
OBJECT_WRAPPER(ComplexLibAddin::A, ComplexLib::A)
OBJECT_WRAPPER(ComplexLibAddin::B, ComplexLib::B)
OBJECT_WRAPPER(ComplexLibAddin::Foo, ComplexLib::Foo)
OBJECT_WRAPPER(ComplexLibAddin::Bar, ComplexLib::Bar)
OBJECT_WRAPPER(ComplexLibAddin::FooBar, ComplexLib::FooBar)
OBJECT_WRAPPER(ComplexLibAddin::Foo2, ComplexLib::Foo2)
OBJECT_WRAPPER(ComplexLibAddin::Bar2, ComplexLib::Bar2)
OBJECT_WRAPPER(ComplexLibAddin::FooBar2, ComplexLib::FooBar2)
OBJECT_WRAPPER(ComplexLibAddin::Doc1, ComplexLib::Doc1)
OBJECT_WRAPPER(ComplexLibAddin::Doc2, ComplexLib::Doc2)

%typemap(rp_tm_cpp_cnvt) boost::shared_ptr<ComplexLib::Foo> const & %{
    RP_GET_REFERENCE($1_name_get, $1_name, ComplexLibAddin::FooBar, ComplexLib::FooBar);
%}

%typemap(rp_tm_cpp_cnvt) boost::shared_ptr<ComplexLib::Bar> const & %{
    RP_GET_REFERENCE($1_name_get, $1_name, ComplexLibAddin::FooBar, ComplexLib::FooBar);
%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<ComplexLib::Foo> const & %{
    RP_GET_REFERENCE($1_name_obj, $1_name, ComplexLibAddin::FooBar, ComplexLib::FooBar);
%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<ComplexLib::Bar> const & %{
    RP_GET_REFERENCE($1_name_obj, $1_name, ComplexLibAddin::FooBar, ComplexLib::FooBar);
%}

%typemap(rp_tm_cpp_cnvt) boost::shared_ptr<ComplexLib::Foo2> const & %{
    RP_GET_REFERENCE($1_name_get, $1_name, ComplexLibAddin::Foo2, ComplexLib::Foo2);
%}

%typemap(rp_tm_cpp_cnvt) boost::shared_ptr<ComplexLib::Bar2> const & %{
    RP_GET_REFERENCE($1_name_get, $1_name, ComplexLibAddin::Bar2, ComplexLib::Bar2);
%}

%typemap(rp_tm_cpp_cnvt) boost::shared_ptr<ComplexLib::FooBar2> const & %{
    RP_GET_REFERENCE($1_name_get, $1_name, ComplexLibAddin::FooBar2, ComplexLib::FooBar2);
%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<ComplexLib::Foo2> const & %{
    RP_GET_REFERENCE($1_name_obj, $1_name, ComplexLibAddin::Foo2, ComplexLib::Foo2);
%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<ComplexLib::Bar2> const & %{
    RP_GET_REFERENCE($1_name_obj, $1_name, ComplexLibAddin::Bar2, ComplexLib::Bar2);
%}

%typemap(rp_tm_xll_cnvt) boost::shared_ptr<ComplexLib::FooBar2> const & %{
    RP_GET_REFERENCE($1_name_obj, $1_name, ComplexLibAddin::FooBar2, ComplexLib::FooBar2);
%}


