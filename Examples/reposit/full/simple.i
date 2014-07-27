
%typemap(rp_cpp_in) SimpleLib::Long "const ObjectHandler::property_t&";
%typemap(rp_cpp_in) SimpleLib::Account::Type "const std::string&";
%typemap(rp_cpp_in) boost::shared_ptr<SimpleLib::Color> "const std::string&";

%typemap(rp_cpp_cnv) SimpleLib::Long %{
    SimpleLib::Long $1_name_cnv =
        ObjectHandler::convert2<SimpleLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(rp_cpp_cnv) SimpleLib::Account::Type %{
    SimpleLib::Account::Type $1_name_enum =
        ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_cpp_cnv) boost::shared_ptr<SimpleLib::Color> %{
    boost::shared_ptr<SimpleLib::Color> $1_name_enum =
        ObjectHandler::Create<boost::shared_ptr<SimpleLib::Color> >()($1_name);
%} 

%typemap(rp_cpp_call) SimpleLib::Long "$1_name_cnv";
%typemap(rp_cpp_call) SimpleLib::Account::Type "$1_name_enum";
%typemap(rp_cpp_call) boost::shared_ptr<SimpleLib::Color> "$1_name_enum";

%typemap(rp_excel) SimpleLib::Long "P";
%typemap(rp_excel) SimpleLib::Account::Type "P";
%typemap(rp_excel) boost::shared_ptr<SimpleLib::Color> "P";

%typemap(rp_excel_in) SimpleLib::Long "OPER*";
%typemap(rp_excel_in) boost::shared_ptr<SimpleLib::Color> "const char*";

%typemap(rp_excel_cnv) SimpleLib::Long %{
        SimpleLib::Long $1_name_cnv = ObjectHandler::convert2<SimpleLib::Long>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", SimpleLib::Long());
%} 

%typemap(rp_excel_cnv) boost::shared_ptr<SimpleLib::Color> %{
        boost::shared_ptr<SimpleLib::Color> $1_name_enum =
            ObjectHandler::Create<boost::shared_ptr<SimpleLib::Color> >()($1_name);
%} 

%typemap(rp_excel_call) SimpleLib::Long "$1_name_cnv";
%typemap(rp_excel_call) boost::shared_ptr<SimpleLib::Color> "$1_name_enum";

%module SimpleLibAddin
%include adder.i
%include adder2.i
%include color.i
%include foo.i

