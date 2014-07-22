
%typemap(cpp_in) SimpleLib::Long "const ObjectHandler::property_t&";

%typemap(cpp_cnv) SimpleLib::Long %{
    SimpleLib::Long $1_name_cnv=ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(cpp_call) SimpleLib::Long "$1_name_cnv";

%typemap(excel) SimpleLib::Long "X";
%typemap(excel_in) SimpleLib::Long "X";

%module SimpleLibAddin
%include adder.i
%include adder2.i
%include foo.i

