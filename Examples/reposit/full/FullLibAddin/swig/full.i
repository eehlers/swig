
//%typemap(rp_cre_cnv) FullLib::Long %{
//    ObjectHandler::property_t $1_name =
//        valueObject->getProperty("$1_name");
//%} 

%typemap(rp_add_class) FullLib::Base * %{
    OH_LIB_CLASS(Base, FullLib::Base);
%}

%typemap(rp_add_in) FullLib::Long "const ObjectHandler::property_t&";
%typemap(rp_add_in) FullLib::Account::Type "const std::string&";
%typemap(rp_add_in) boost::shared_ptr<FullLib::Color> "const std::string&";

%typemap(rp_add_cnv) FullLib::Long %{
    FullLib::Long $1_name_cnv =
        ObjectHandler::convert2<FullLib::Long, ObjectHandler::property_t>($1_name);
%} 

%typemap(rp_add_cnv) FullLib::Account::Type %{
    FullLib::Account::Type $1_name_enum =
        ObjectHandler::Create<$1_type>()($1_name);
%} 

%typemap(rp_add_cnv) boost::shared_ptr<FullLib::Color> %{
    boost::shared_ptr<FullLib::Color> $1_name_enum =
        ObjectHandler::Create<boost::shared_ptr<FullLib::Color> >()($1_name);
%} 

%typemap(rp_add_call) FullLib::Long "$1_name_cnv";
%typemap(rp_add_call) FullLib::Account::Type "$1_name_enum";
%typemap(rp_add_call) boost::shared_ptr<FullLib::Color> "$1_name_enum";

%typemap(rp_vo_declare) FullLib::Long "ObjectHandler::property_t $1_name_";
%typemap(rp_vo_cnv) FullLib::Long "value";

%typemap(rp_excel) FullLib::Long "P";
%typemap(rp_excel) FullLib::Account::Type "P";
%typemap(rp_excel) boost::shared_ptr<FullLib::Color> "P";

%typemap(rp_excel_in) FullLib::Long "OPER*";
%typemap(rp_excel_in) boost::shared_ptr<FullLib::Color> "const char*";

%typemap(rp_excel_cnv) FullLib::Long %{
        FullLib::Long $1_name_cnv = ObjectHandler::convert2<FullLib::Long>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", FullLib::Long());
%} 

%typemap(rp_excel_cnv) boost::shared_ptr<FullLib::Color> %{
        boost::shared_ptr<FullLib::Color> $1_name_enum =
            ObjectHandler::Create<boost::shared_ptr<FullLib::Color> >()($1_name);
%} 

%typemap(rp_excel_call) FullLib::Long "$1_name_cnv";
%typemap(rp_excel_call) boost::shared_ptr<FullLib::Color> "$1_name_enum";

%module(
    rp_obj_dir="qlo",
    rp_xl_dir="../QuantLibXL2"
    ) FullLibAddin

%include adder.i
%include adder2.i
%include color.i
%include foo.i
%include inheritance.i
%include noparm.i

