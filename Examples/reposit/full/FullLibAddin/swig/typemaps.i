
// rp_val_* - valueobjects

%typemap(rp_val_declare) FullLib::Long "ObjectHandler::property_t $1_name_";
%typemap(rp_val_cnv) FullLib::Long "value";
%typemap(rp_val_in) FullLib::Long "const ObjectHandler::property_t&";
%typemap(rp_val_in) FullLib::Account::Type "const std::string&";
%typemap(rp_val_in) boost::shared_ptr<FullLib::Color> "const std::string&";

// rp_ser_* - serialization

// rp_obj_* - addin objects

%typemap(rp_obj_class) FullLib::Base * %{
    OH_LIB_CLASS(Base, FullLib::Base);
%}

// rp_add_* - C++ addin

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

// rp_xll_* - Excel addin

%typemap(rp_xll) FullLib::Long "P";
%typemap(rp_xll) FullLib::Account::Type "P";
%typemap(rp_xll) boost::shared_ptr<FullLib::Color> "P";

%typemap(rp_xll_in) FullLib::Long "OPER*";
%typemap(rp_xll_in) boost::shared_ptr<FullLib::Color> "const char*";

%typemap(rp_xll_cnv) FullLib::Long %{
        FullLib::Long $1_name_cnv = ObjectHandler::convert2<FullLib::Long>(
            ObjectHandler::ConvertOper(*$1_name), "$1_name", FullLib::Long());
%} 

%typemap(rp_xll_cnv) boost::shared_ptr<FullLib::Color> %{
        boost::shared_ptr<FullLib::Color> $1_name_enum =
            ObjectHandler::Create<boost::shared_ptr<FullLib::Color> >()($1_name);
%} 

%typemap(rp_xll_call) FullLib::Long "$1_name_cnv";
%typemap(rp_xll_call) boost::shared_ptr<FullLib::Color> "$1_name_enum";

