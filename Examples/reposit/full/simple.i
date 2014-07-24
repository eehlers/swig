
%typemap(rp_cpp_in) SimpleLib::Long "const ObjectHandler::property_t&";
%typemap(rp_cpp_in) SimpleLib::Account::Type "const std::string&";
%typemap(rp_cpp_in) SimpleLib::Color const & "const std::string&";

%typemap(rp_cpp_cnv) SimpleLib::Long %{
    SimpleLib::Long $1_name_cnv=ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(rp_cpp_cnv) SimpleLib::Account::Type %{
    SimpleLib::Account::Type $1_name_enum =
        ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_cpp_cnv) SimpleLib::Color const & %{
    SimpleLib::Color $1_name_enum =
        ObjectHandler::Create<SimpleLib::Color>()($1_name);
%} 

%typemap(rp_cpp_call) SimpleLib::Long "$1_name_cnv";
%typemap(rp_cpp_call) SimpleLib::Account::Type "$1_name_enum";
%typemap(rp_cpp_call) SimpleLib::Color const & "$1_name_enum";

%typemap(rp_excel) SimpleLib::Long "X";
%typemap(rp_excel) SimpleLib::Account::Type "X";
%typemap(rp_excel) SimpleLib::Color const & "X";

%typemap(rp_excel_in) SimpleLib::Long "X";

%module SimpleLibAddin
%include adder.i
%include adder2.i
%include color.i
%include foo.i

