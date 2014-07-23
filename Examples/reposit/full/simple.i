
%typemap(cpp_in) SimpleLib::Long "const ObjectHandler::property_t&";
%typemap(cpp_in) SimpleLib::Account::Type "const std::string&";
//%typemap(cpp_in) SimpleLib::Color const & "const std::string&";

%typemap(cpp_cnv) SimpleLib::Long %{
    SimpleLib::Long $1_name_cnv=ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(cpp_cnv) SimpleLib::Account::Type %{
    SimpleLib::Account::Type $1_name_enum =
        ObjectHandler::Create<SimpleLib::Account::Type>()($1_name);
%} 

//%typemap(cpp_cnv) SimpleLib::Color const & %{
//    SimpleLib::Color $1_name_enum =
//        ObjectHandler::Create<SimpleLib::Color>()($1_name);
//%} 

%typemap(cpp_call) SimpleLib::Long "$1_name_cnv";
%typemap(cpp_call) SimpleLib::Account::Type "$1_name_enum";
//%typemap(cpp_call) SimpleLib::Color const & "$1_name_enum";

%typemap(excel) SimpleLib::Long "X";
%typemap(excel) SimpleLib::Account::Type "X";
//%typemap(excel) SimpleLib::Color const & "X";

%typemap(excel_in) SimpleLib::Long "X";

%module SimpleLibAddin
%include adder.i
%include adder2.i
%include color.i
%include foo.i

